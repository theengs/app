/*
    Theengs - Decode things and devices
    Copyright: (c) Florian ROBERT

    This file is part of Theengs.

    Theengs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Theengs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "device_sbbt.h"
#include "device_utils_switchbot.h"

#include <QBluetoothUuid>
#include <QBluetoothServiceInfo>
#include <QLowEnergyService>

#include <QSqlQuery>
#include <QSqlError>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QListIterator>
#include <QDebug>

/* ************************************************************************** */

DeviceSwitchbotBlindTilt::DeviceSwitchbotBlindTilt(const QString &deviceAddr,
                                                   const QString &deviceName,
                                                   const QString &deviceModel,
                                                   const QString &devicePropsJson,
                                                   QObject *parent):
    DeviceTheengsWindowActuators(deviceAddr, deviceName, deviceModel, devicePropsJson, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR_WINDOW;
    //m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceSwitchbotBlindTilt::DeviceSwitchbotBlindTilt(const QBluetoothDeviceInfo &d,
                                                   const QString &deviceModel,
                                                   const QString &devicePropsJson,
                                                   QObject *parent):
    DeviceTheengsWindowActuators(d, deviceModel, devicePropsJson, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR_WINDOW;
    //m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

/* ************************************************************************** */

DeviceSwitchbotBlindTilt::~DeviceSwitchbotBlindTilt()
{
    delete m_serviceData;
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::deviceConnected()
{
    qDebug() << "DeviceSwitchbotBlindTilt::deviceConnected(" << m_deviceAddress << ")";
    Device::deviceConnected();
}

/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::deviceDisconnected()
{
    qDebug() << "DeviceSwitchbotBlindTilt::deviceDisconnected(" << m_deviceAddress << ")";
    Device::deviceDisconnected();
}

/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::deviceErrored(QLowEnergyController::Error error)
{
    qDebug() << "DeviceSwitchbotBlindTilt::deviceErrored(" << error << ")";
    Device::deviceErrored(error);
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::serviceScanDone()
{
    qDebug() << "DeviceSwitchbotBlindTilt::serviceScanDone(" << m_deviceAddress << ")";

    if (m_serviceData)
    {
        if (m_serviceData->state() == QLowEnergyService::RemoteService)
        {
            connect(m_serviceData, &QLowEnergyService::stateChanged, this, &DeviceSwitchbotBlindTilt::serviceDetailsDiscovered_data);

            connect(m_serviceData, &QLowEnergyService::characteristicChanged, this, &DeviceSwitchbotBlindTilt::bleReadNotify);
            connect(m_serviceData, &QLowEnergyService::characteristicRead, this, &DeviceSwitchbotBlindTilt::bleReadDone);
            connect(m_serviceData, &QLowEnergyService::characteristicWritten, this, &DeviceSwitchbotBlindTilt::bleWriteDone);

            connect(m_serviceData, &QLowEnergyService::descriptorRead, this, &DeviceSwitchbotBlindTilt::bleDescriptorRead);
            connect(m_serviceData, &QLowEnergyService::descriptorWritten, this, &DeviceSwitchbotBlindTilt::bleDescriptorWritten);

            connect(m_serviceData, &QLowEnergyService::errorOccurred, this, &DeviceSwitchbotBlindTilt::bleServiceError);

            // Windows hack, see: QTBUG-80770 and QTBUG-78488
            QTimer::singleShot(0, this, [=] () { m_serviceData->discoverDetails(QLowEnergyService::SkipValueDiscovery); });
        }
    }
}

/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::addLowEnergyService(const QBluetoothUuid &uuid)
{
    qDebug() << "DeviceSwitchbotBlindTilt::addLowEnergyService(" << uuid.toString() << ")";

    if (uuid == uuid_data_srv)
    {
        delete m_serviceData;
        m_serviceData = nullptr;

        m_serviceData = m_bleController->createServiceObject(uuid);
        if (!m_serviceData)
        {
            qWarning() << "Cannot create service (volt) for uuid:" << uuid.toString();
        }
    }
}

/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::serviceDetailsDiscovered_data(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::RemoteServiceDiscovered)
    {
        qDebug() << "DeviceSwitchbotBlindTilt::serviceDetailsDiscovered_data(" << m_deviceAddress << ") > ServiceDiscovered";

        if (m_serviceData)
        {
            // RX Characteristic
            m_charRX = m_serviceData->characteristic(uuid_data_char_rx);
            m_notificationDesc = m_charRX.clientCharacteristicConfiguration();
            m_serviceData->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

            // TX Characteristic
            m_charTX = m_serviceData->characteristic(uuid_data_char_tx);

            // Debug
            if (!m_charTX.isValid()) { qWarning() << "m_charTX invalid"; }
            if (!m_charRX.isValid()) { qWarning() << "m_charRX invalid"; }
            if (!m_notificationDesc.isValid()) { qWarning() << "m_notificationDesc on m_charRX invalid"; }
        }
    }
}

/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::bleDescriptorRead(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceSwitchbotBlindTilt::bleDescriptorRead()";
}

void DeviceSwitchbotBlindTilt::bleDescriptorWritten(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceSwitchbotBlindTilt::bleDescriptorWritten()";

    // Ask for device info
    m_serviceData->writeCharacteristic(m_charTX, QByteArray::fromHex("5702"), QLowEnergyService::WriteWithResponse);
}

/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotBlindTilt::bleWriteDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotBlindTilt::bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotBlindTilt::bleReadDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotBlindTilt::bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotBlindTilt::bleReadNotify(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();

    const uint8_t *data = reinterpret_cast<const quint8 *>(value.constData());

    if (c.uuid() == uuid_data_char_rx && value.size() == 8)
    {
        /// response from "CMD_GET_INFO" command
        // [ 1] Bat Per         The battery percentage
        // [ 2] FW Ver          Firmware Version

        int battery = data[1];
        setBattery(battery);

        QString firmware = QString::number(data[2]);
        setFirmware(firmware);

        int direction = (data[4] >> 7) & 0x01;
        int solar = ((data[5] >> 3) & 0x01);
        int calibrated = ((data[5] >> 2) & 0x01);
        int moving = (data[5] & 0x02);
        int position = data[6];
        int timers = data[7];
/*
        qDebug() << "direction" << direction;
        qDebug() << "solar" << solar;
        qDebug() << "calibrated" << calibrated;
        qDebug() << "moving" << moving;
        qDebug() << "timers" << timers;
*/
        setDirection(direction);
        setSolar(solar);
        setCalibrated(calibrated);
        setMoving(moving);
        setPosition(position);

        m_lastUpdate = QDateTime::currentDateTime();
        refreshDataFinished(true);
    }
}

void DeviceSwitchbotBlindTilt::bleServiceError(QLowEnergyService::ServiceError e)
{
    qDebug() << "DeviceSwitchbotBlindTilt::bleServiceError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DeviceSwitchbotBlindTilt::actionAction(const int action)
{
    qDebug() << "DeviceSwitchbotBlindTilt::actionAction(" << action << ")";

    //static uint8_t OPEN[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x32};
    //static uint8_t CLOSE_DOWN[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x00};
    //static uint8_t CLOSE_UP[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x64};
    //static uint8_t MOVE[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x00};
    //static uint8_t STOP[] = {0x57, 0x0f, 0x45, 0x01, 0x00, 0x01};

    if (m_charTX.isValid())
    {
        QByteArray cmd;

        // Magic Number
        cmd.push_back(0x57);
        // Header / Command
        cmd.push_back(DeviceUtilsSwitchBot::CMD_ACTION_EXTENDED);
        // Payload...
        cmd.push_back(0x45); // 0x45 Curtain settings command / 0x46 Curtain get command
        cmd.push_back(0x01); // Function Code // 0x01-action / 0x04-Basic attributes / 0x81-Command status

        if (action == DeviceUtilsSwitchBot::ACTION_OPEN ||
            action == DeviceUtilsSwitchBot::ACTION_CLOSE_UP ||
            action == DeviceUtilsSwitchBot::ACTION_CLOSE_DOWN)
        {
            char position = 50;
            if (action == DeviceUtilsSwitchBot::ACTION_CLOSE_UP) position = 100;
            if (action == DeviceUtilsSwitchBot::ACTION_CLOSE_DOWN) position = 0;

            cmd.push_back(0x01);
            cmd.push_back(0x01);
            cmd.push_back(position); // position in %
        }
        else //if (action == DeviceUtilsSwitchBot::ACTION_STOP)
        {
            cmd.push_back(char(0x00)); // ?
            cmd.push_back(0x01); // ?
        }

        m_serviceData->writeCharacteristic(m_charTX, cmd,  QLowEnergyService::WriteWithResponse);
        return true;
    }

    return false;
}

/* ************************************************************************** */

bool DeviceSwitchbotBlindTilt::actionMove(const int position)
{
    qDebug() << "DeviceSwitchbotBlindTilt::actionMove(" << position << ")";

    if (position >= 0 && position <= 100)
    {
        if (m_charTX.isValid())
        {
            QByteArray cmd;

            // Magic Number
            cmd.push_back(0x57);
            // Header / Command
            cmd.push_back(DeviceUtilsSwitchBot::CMD_ACTION_EXTENDED);
            // Payload...
            cmd.push_back(0x45);
            cmd.push_back(0x01);
            cmd.push_back(0x01);
            cmd.push_back(0x01);
            cmd.push_back(position); // position in %

            m_serviceData->writeCharacteristic(m_charTX, cmd,  QLowEnergyService::WriteWithResponse);
            return true;
        }
    }
    else
    {
        qWarning() << "DeviceSwitchbotBlindTilt::actionMove(" << position << ") INVALID";
    }

    return false;
}

/* ************************************************************************** */
