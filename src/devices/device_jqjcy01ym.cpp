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

#include "device_jqjcy01ym.h"

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

DeviceJQJCY01YM::DeviceJQJCY01YM(QString &deviceAddr, QString &deviceName, QObject *parent):
    DeviceSensor(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
    m_deviceCapabilities += DeviceUtils::DEVICE_BATTERY;
    m_deviceSensors += DeviceUtils::SENSOR_HCHO;
    m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
}

DeviceJQJCY01YM::DeviceJQJCY01YM(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceSensor(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
    m_deviceCapabilities += DeviceUtils::DEVICE_BATTERY;
    m_deviceSensors += DeviceUtils::SENSOR_HCHO;
    m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
}

DeviceJQJCY01YM::~DeviceJQJCY01YM()
{
    //
}

/* ************************************************************************** */

void DeviceJQJCY01YM::serviceScanDone()
{
    //qDebug() << "DeviceJQJCY01YM::serviceScanDone(" << m_deviceAddress << ")";
}

void DeviceJQJCY01YM::addLowEnergyService(const QBluetoothUuid &uuid)
{
    //qDebug() << "DeviceJQJCY01YM::addLowEnergyService(" << uuid.toString() << ")";
}

/* ************************************************************************** */

void DeviceJQJCY01YM::parseAdvertisementData(const QByteArray &value)
{
    //
}

/* ************************************************************************** */
