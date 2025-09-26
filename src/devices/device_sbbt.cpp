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
            //
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
}

void DeviceSwitchbotBlindTilt::bleServiceError(QLowEnergyService::ServiceError e)
{
    qDebug() << "DeviceSwitchbotBlindTilt::bleServiceError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceSwitchbotBlindTilt::actionAction(const int action)
{
    qDebug() << "DeviceSwitchbotBlindTilt::actionAction(" << action << ")";

    static uint8_t OPEN[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x32};
    static uint8_t CLOSE_DOWN[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x00};
    static uint8_t CLOSE_UP[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x64};
    static uint8_t MOVE[] = {0x57, 0x0f, 0x45, 0x01, 0x01, 0x01, 0x00};
    static uint8_t STOP[] = {0x57, 0x0f, 0x45, 0x01, 0x00, 0x01};
}

/* ************************************************************************** */
/* ************************************************************************** */
