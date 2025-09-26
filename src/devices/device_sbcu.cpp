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
            //
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
