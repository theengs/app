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
    qDebug() << "DeviceSwitchbotSmartSwitch::deviceErrored(" << error << ")";
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
            // TX Characteristic
            m_charTX = m_serviceData->characteristic(uuid_data_char_tx);

            // RX Characteristic
            m_charRX = m_serviceData->characteristic(uuid_data_char_rx);
            m_notificationDesc = m_charRX.clientCharacteristicConfiguration();
            m_serviceData->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

            // Debug
            if (!m_charTX.isValid()) { qWarning() << "m_charTX invalid"; }
            if (!m_charRX.isValid()) { qWarning() << "m_charRX invalid"; }
            if (!m_notificationDesc.isValid()) { qWarning() << "m_notificationDesc on m_charRX invalid"; }
        }
    }
}

/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::bleDescriptorRead(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::bleDescriptorRead()";
}

void DeviceSwitchbotSmartSwitch::bleDescriptorWritten(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::bleDescriptorWritten()";
}

/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::bleWriteDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotSmartSwitch::bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::bleReadDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceSwitchbotSmartSwitch::bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::bleReadNotify(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();

    if (c.uuid() == uuid_data_char_rx && value.size() == 16)
    {
        const uint8_t *data = reinterpret_cast<const quint8 *>(value.constData());

        //
    }
}

void DeviceSwitchbotSmartSwitch::bleServiceError(QLowEnergyService::ServiceError e)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::bleServiceError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotSmartSwitch::actionAction(const int action)
{
    qDebug() << "DeviceSwitchbotSmartSwitch::actionAction(" << action << ")";

    static uint8_t ON[] = {0x57, 0x01, 0x01};
    static uint8_t OFF[] = {0x57, 0x01, 0x02};
    static uint8_t PRESS[] = {0x57, 0x01, 0x00};
    static uint8_t DOWN[] = {0x57, 0x01, 0x03};
    static uint8_t UP[] = {0x57, 0x01, 0x04};
}

/* ************************************************************************** */
/* ************************************************************************** */
