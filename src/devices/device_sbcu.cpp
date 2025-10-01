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

#include "device_sbcu.h"
#include "device_sb_utils.h"

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

DeviceSwitchbotCurtain::DeviceSwitchbotCurtain(const QString &deviceAddr,
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

DeviceSwitchbotCurtain::DeviceSwitchbotCurtain(const QBluetoothDeviceInfo &d,
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

DeviceSwitchbotCurtain::~DeviceSwitchbotCurtain()
{
    delete m_serviceData;
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotCurtain::deviceConnected()
{
    qDebug() << "DeviceSwitchbotCurtain::deviceConnected(" << m_deviceAddress << ")";
    Device::deviceConnected();
}

/* ************************************************************************** */

void DeviceSwitchbotCurtain::deviceDisconnected()
{
    qDebug() << "DeviceSwitchbotCurtain::deviceDisconnected(" << m_deviceAddress << ")";
    Device::deviceDisconnected();
}

/* ************************************************************************** */

void DeviceSwitchbotCurtain::deviceErrored(QLowEnergyController::Error error)
{
    qDebug() << "DeviceSwitchbotCurtain::deviceErrored(" << error << ")";
    Device::deviceErrored(error);
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotCurtain::serviceScanDone()
{
    qDebug() << "DeviceSwitchbotCurtain::serviceScanDone(" << m_deviceAddress << ")";

    if (m_serviceData)
    {
        if (m_serviceData->state() == QLowEnergyService::RemoteService)
        {
            connect(m_serviceData, &QLowEnergyService::stateChanged, this, &DeviceSwitchbotCurtain::serviceDetailsDiscovered_data);

            connect(m_serviceData, &QLowEnergyService::characteristicChanged, this, &DeviceSwitchbotCurtain::bleReadNotify);
            connect(m_serviceData, &QLowEnergyService::characteristicRead, this, &DeviceSwitchbotCurtain::bleReadDone);
            connect(m_serviceData, &QLowEnergyService::characteristicWritten, this, &DeviceSwitchbotCurtain::bleWriteDone);

            connect(m_serviceData, &QLowEnergyService::descriptorRead, this, &DeviceSwitchbotCurtain::bleDescriptorRead);
            connect(m_serviceData, &QLowEnergyService::descriptorWritten, this, &DeviceSwitchbotCurtain::bleDescriptorWritten);

            connect(m_serviceData, &QLowEnergyService::errorOccurred, this, &DeviceSwitchbotCurtain::bleServiceError);

            // Windows hack, see: QTBUG-80770 and QTBUG-78488
            QTimer::singleShot(0, this, [=] () { m_serviceData->discoverDetails(QLowEnergyService::SkipValueDiscovery); });
        }
    }
}

/* ************************************************************************** */

void DeviceSwitchbotCurtain::addLowEnergyService(const QBluetoothUuid &uuid)
{
    qDebug() << "DeviceSwitchbotCurtain::addLowEnergyService(" << uuid.toString() << ")";

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

void DeviceSwitchbotCurtain::serviceDetailsDiscovered_data(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::RemoteServiceDiscovered)
    {
        qDebug() << "DeviceSwitchbotCurtain::serviceDetailsDiscovered_data(" << m_deviceAddress << ") > ServiceDiscovered";

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

void DeviceSwitchbotCurtain::bleDescriptorRead(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceSwitchbotCurtain::bleDescriptorRead()";
}

void DeviceSwitchbotCurtain::bleDescriptorWritten(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceSwitchbotCurtain::bleDescriptorWritten()";

    // Ask for device info
    m_serviceData->writeCharacteristic(m_charTX, QByteArray::fromHex("5702"), QLowEnergyService::WriteWithResponse);
}

/* ************************************************************************** */

void DeviceSwitchbotCurtain::bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotCurtain::bleWriteDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotCurtain::bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotCurtain::bleReadDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotCurtain::bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotCurtain::bleReadNotify(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();

    const uint8_t *data = reinterpret_cast<const quint8 *>(value.constData());

    if (c.uuid() == uuid_data_char_rx && value.size() == 8)
    {
        /// response from "CMD_GET_INFO" command
        // [ 1] Bat Per         The battery percentage
        // [ 2] FW Ver          Firmware Version
        // [ 3] Chain Length    Device Chain Length
        // [ 4] (State 1)
        //      (b7) > direction, 0: default (open to the left), 1: reverse
        //      (b6) > touch and go, 0: disable, 1: enable
        //      (b5) > lighting effect, 0: disable, 1: enable
        //      (b4) > reserved
        //      (b3) > fault, 0-none, 1-faulty
        // [ 5] (State 2)
        //      (b3) > Whether solar panel is plugged in, 0-No, 1-There is solar panel
        //      (b2) > calibrated, 0-not calibrated, 1-calibrated
        //     (b1:0) > 0-motion status, 0-static, 1-open window, 2-close window
        // [ 6] Position        Current location of the device (%)
        // [ 7] Timer Amount    Number of timers

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

        setPosition(position);
    }
}

void DeviceSwitchbotCurtain::bleServiceError(QLowEnergyService::ServiceError e)
{
    qDebug() << "DeviceSwitchbotCurtain::bleServiceError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotCurtain::actionAction(const int action)
{
    qDebug() << "DeviceSwitchbotCurtain::actionAction(" << action << ")";

    static uint8_t CLOSE[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x64};
    static uint8_t OPEN[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x00};
    static uint8_t MOVE[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x00};
    static uint8_t STOP[] = {0x57, 0x0f, 0x45, 0x01, 0x00, 0x01};
}

/* ************************************************************************** */
/* ************************************************************************** */
