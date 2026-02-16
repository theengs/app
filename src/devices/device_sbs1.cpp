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

#include "device_sbs1.h"
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

DeviceSwitchbotSmartSwitch::DeviceSwitchbotSmartSwitch(const QString &deviceAddr,
                                                       const QString &deviceName,
                                                       const QString &deviceModel,
                                                       const QString &devicePropsJson,
                                                       QObject *parent):
    DeviceTheengsActuators(deviceAddr, deviceName, deviceModel, devicePropsJson, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR;
    //m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (hasSetting("switchMode")) m_switchMode = getSetting("switchMode").toString().toInt();
    if (hasSetting("switchHoldTime")) m_switchHoldTime = getSetting("switchHoldTime").toString().toInt();
    if (hasSetting("switchInverted")) m_switchInverted = getSetting("switchInverted").toString().toInt();
}

DeviceSwitchbotSmartSwitch::DeviceSwitchbotSmartSwitch(const QBluetoothDeviceInfo &d,
                                                       const QString &deviceModel,
                                                       const QString &devicePropsJson,
                                                       QObject *parent):
    DeviceTheengsActuators(d, deviceModel, devicePropsJson, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR;
    //m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (hasSetting("switchMode")) m_switchMode = getSetting("switchMode").toString().toInt();
    if (hasSetting("switchHoldTime")) m_switchHoldTime = getSetting("switchHoldTime").toString().toInt();
    if (hasSetting("switchInverted")) m_switchInverted = getSetting("switchInverted").toString().toInt();
}

/* ************************************************************************** */

DeviceSwitchbotSmartSwitch::~DeviceSwitchbotSmartSwitch()
{
    delete m_serviceData;
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::deviceConnected()
{
    qDebug() << "DeviceSwitchbotSmartSwitch::deviceConnected(" << m_deviceAddress << ")";
    Device::deviceConnected();
}

/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::deviceDisconnected()
{
    qDebug() << "DeviceSwitchbotSmartSwitch::deviceDisconnected(" << m_deviceAddress << ")";
    Device::deviceDisconnected();
}

/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::deviceErrored(QLowEnergyController::Error error)
{
    qWarning() << "DeviceSwitchbotSmartSwitch::deviceErrored(" << error << ")";
    Device::deviceErrored(error);
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::serviceScanDone()
{
    qDebug() << "DeviceSwitchbotSmartSwitch::serviceScanDone(" << m_deviceAddress << ")";

    if (m_serviceData)
    {
        if (m_serviceData->state() == QLowEnergyService::RemoteService)
        {
            connect(m_serviceData, &QLowEnergyService::stateChanged, this, &DeviceSwitchbotSmartSwitch::serviceDetailsDiscovered_data);

            connect(m_serviceData, &QLowEnergyService::characteristicChanged, this, &DeviceSwitchbotSmartSwitch::bleReadNotify);
            connect(m_serviceData, &QLowEnergyService::characteristicRead, this, &DeviceSwitchbotSmartSwitch::bleReadDone);
            connect(m_serviceData, &QLowEnergyService::characteristicWritten, this, &DeviceSwitchbotSmartSwitch::bleWriteDone);

            connect(m_serviceData, &QLowEnergyService::descriptorRead, this, &DeviceSwitchbotSmartSwitch::bleDescriptorRead);
            connect(m_serviceData, &QLowEnergyService::descriptorWritten, this, &DeviceSwitchbotSmartSwitch::bleDescriptorWritten);

            connect(m_serviceData, &QLowEnergyService::errorOccurred, this, &DeviceSwitchbotSmartSwitch::bleServiceError);

            // Windows hack, see: QTBUG-80770 and QTBUG-78488
            QTimer::singleShot(0, this, [=] () { m_serviceData->discoverDetails(QLowEnergyService::SkipValueDiscovery); });
        }
    }
}

/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::addLowEnergyService(const QBluetoothUuid &uuid)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::addLowEnergyService(" << uuid.toString() << ")";

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

void DeviceSwitchbotSmartSwitch::serviceDetailsDiscovered_data(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::RemoteServiceDiscovered)
    {
        qDebug() << "DeviceSwitchbotSmartSwitch::serviceDetailsDiscovered_data(" << m_deviceAddress << ") > ServiceDiscovered";

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
            //if (!m_notificationDesc.isValid()) { qWarning() << "m_notificationDesc on m_charRX invalid"; }
        }
    }
}

/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::bleDescriptorRead(const QLowEnergyDescriptor &, const QByteArray &)
{
    //qDebug() << "DeviceSwitchbotSmartSwitch::bleDescriptorRead()";
}

void DeviceSwitchbotSmartSwitch::bleDescriptorWritten(const QLowEnergyDescriptor &, const QByteArray &)
{
    //qDebug() << "DeviceSwitchbotSmartSwitch::bleDescriptorWritten()";

    // Ask for device info
    m_serviceData->writeCharacteristic(m_charTX, QByteArray::fromHex("5702"), QLowEnergyService::WriteWithResponse);
}

/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::bleWriteDone(const QLowEnergyCharacteristic &, const QByteArray &)
{
    //qDebug() << "DeviceSwitchbotSmartSwitch::bleWriteDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotSmartSwitch::bleReadDone(const QLowEnergyCharacteristic &, const QByteArray &)
{
    //qDebug() << "DeviceSwitchbotSmartSwitch::bleReadDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotSmartSwitch::bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    //qDebug() << "DeviceSwitchbotSmartSwitch::bleReadNotify(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();

    const uint8_t *data = reinterpret_cast<const quint8 *>(value.constData());

    if (c.uuid() == uuid_data_char_rx && value.size() == 3)
    {
        /// response from "CMD_ACTION" or "CMD_SET_INFO" commands
        // [00] status
        // [1+] payload sent?

        if (data[0] == DeviceUtilsSwitchBot::RSP_OK)
        {
            if (data[1] == 0xff) // CMD_ACTION?
            {
                //
            }
            else if (data[1] == 0x64) // CMD_SET_INFO?
            {
                //
            }
            else // ?
            {
                // ?
            }
        }
    }
    else if (c.uuid() == uuid_data_char_rx && value.size() == 13)
    {
        /// response from "CMD_GET_INFO" command
        // [ 1] Bat Per         The battery percentage
        // [ 2] FW Ver          Firmware Version
        // [3-7]                (not used by this device)
        // [ 8] Timer Num       The number of Timer
        // [ 9] Act Mode        The act mode of Bot
        // [10] Hold Times
        // [11] Service data byte 0
        // [12] Service data byte 1

        int battery = data[1];
        setBattery(battery);

        QString firmware = QString::number(data[2]);
        setFirmware(firmware);

        //int timers = data[8];
        int actionmode = data[9];
        int holdtime = data[10];

        setSwitchMode(actionmode);
        setSwitchTime(holdtime);

        m_lastUpdate = QDateTime::currentDateTime();
        refreshDataFinished(true);
    }
}

void DeviceSwitchbotSmartSwitch::bleServiceError(QLowEnergyService::ServiceError e)
{
    qWarning() << "DeviceSwitchbotSmartSwitch::bleServiceError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::setSwitchMode(const int m)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::setSwitchMode(" << m << ")";

    if (m == 0 || m == 1)
    {
        if (m != m_switchMode)
        {
            m_switchMode = m;
            Q_EMIT switchmodeUpdated();

            setSetting("switchMode", m);
        }
    }
    else
    {
        qWarning() << "DeviceSwitchbotSmartSwitch::setSwitchMode(" << m << ") INVALID";
    }
}

void DeviceSwitchbotSmartSwitch::setSwitchTime(const int t)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::setSwitchTime(" << t << ")";

    if (t >= 0 && t <= 60)
    {
        if (t != m_switchHoldTime)
        {
            m_switchHoldTime = t;
            Q_EMIT switchtimeUpdated();

            setSetting("switchHoldTime", t);
        }
    }
    else
    {
        qWarning() << "DeviceSwitchbotSmartSwitch::setSwitchTime(" << t << ") INVALID";
    }
}

void DeviceSwitchbotSmartSwitch::setSwitchInverted(const bool i)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::setSwitchInverted(" << i << ")";

    if (i != m_switchInverted)
    {
        m_switchInverted = i;
        Q_EMIT switchinvertedUpdated();

        setSetting("switchInverted", i);
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DeviceSwitchbotSmartSwitch::actionMode(const int mode, const int inverted)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::actionMode(" << mode << inverted << ")";

    if (m_charTX.isValid())
    {
        QByteArray cmd;

        char md = 0;
        if (mode == DeviceUtilsSwitchBot::MODE_PRESS) md = DeviceUtilsSwitchBot::BOT_MODE_PRESS;
        else if (mode == DeviceUtilsSwitchBot::MODE_SWITCH) md = DeviceUtilsSwitchBot::BOT_MODE_SWITCH;
        md <<= 4;
        if (inverted) md &= 0x01;

        // Magic Number
        cmd.push_back(0x57);
        // Header / Command
        cmd.push_back(DeviceUtilsSwitchBot::CMD_SET_INFO);
        // Payload...
        cmd.push_back(0x64); // hardcoded
        cmd.push_back(md); // mode

        setSwitchMode(mode);
        setSwitchInverted(inverted);

        m_serviceData->writeCharacteristic(m_charTX, cmd,  QLowEnergyService::WriteWithResponse);
        return true;
    }

    return false;
}

bool DeviceSwitchbotSmartSwitch::actionAction(const int action)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::actionAction(" << action << ")";

    if (m_charTX.isValid())
    {
        QByteArray cmd;

        // Magic Number
        cmd.push_back(0x57);
        // Header / Command
        cmd.push_back(DeviceUtilsSwitchBot::CMD_ACTION);
        // Payload...
        if (action == DeviceUtilsSwitchBot::ACTION_ON) cmd.push_back(DeviceUtilsSwitchBot::BOT_ACT_ON);
        else if (action == DeviceUtilsSwitchBot::ACTION_OFF) cmd.push_back(DeviceUtilsSwitchBot::BOT_ACT_OFF);
        else if (action == DeviceUtilsSwitchBot::ACTION_PUSH_PULL) cmd.push_back(DeviceUtilsSwitchBot::BOT_ACT_PUSHPULL);
        else if (action == DeviceUtilsSwitchBot::ACTION_PUSH_STOP) cmd.push_back(DeviceUtilsSwitchBot::BOT_ACT_STOP);
        else cmd.push_back(DeviceUtilsSwitchBot::BOT_ACT_BACK); // default?

        m_serviceData->writeCharacteristic(m_charTX, cmd,  QLowEnergyService::WriteWithResponse);
        return true;
    }

    return false;
}

/* ************************************************************************** */
