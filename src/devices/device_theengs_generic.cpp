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

#include "device_theengs_generic.h"

#include <cstdint>
#include <cmath>

#include <QBluetoothUuid>
#include <QBluetoothServiceInfo>
#include <QLowEnergyService>

#include <QSqlQuery>
#include <QSqlError>

#include <QDateTime>
#include <QDebug>

/* ************************************************************************** */

DeviceTheengs::DeviceTheengs(QString &deviceAddr, QString &deviceName, QObject *parent):
    DeviceSensor(deviceAddr, deviceName, parent)
{
    if (deviceName == "thermometer")
    {
        m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
        m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;
        m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    }
    else if (deviceName == "hygrometer")
    {
        m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
        m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;
        m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
        m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
    }
}

DeviceTheengs::DeviceTheengs(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceSensor(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;
    m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
}

DeviceTheengs::~DeviceTheengs()
{
    //
}

/* ************************************************************************** */

void DeviceTheengs::serviceScanDone()
{
    //qDebug() << "DeviceTheengs::serviceScanDone(" << m_deviceAddress << ")";
}

void DeviceTheengs::addLowEnergyService(const QBluetoothUuid &uuid)
{
    //qDebug() << "DeviceTheengs::addLowEnergyService(" << uuid.toString() << ")";
}

/* ************************************************************************** */

void DeviceTheengs::parseAdvertisementData(const QByteArray &value)
{
    //
}

/* ************************************************************************** */
