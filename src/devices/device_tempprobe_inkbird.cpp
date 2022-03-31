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

#include "device_tempprobe_inkbird.h"

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

DeviceTempprobeInkBird::DeviceTempprobeInkBird(QString &deviceAddr, QString &deviceName, QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;

    m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;

    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;

    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_5;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_6;
}

DeviceTempprobeInkBird::DeviceTempprobeInkBird(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceTheengs(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;

    m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;

    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;

    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_5;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_6;
}

DeviceTempprobeInkBird::~DeviceTempprobeInkBird()
{
    //
}

/* ************************************************************************** */

void DeviceTempprobeInkBird::serviceScanDone()
{
    //qDebug() << "DeviceTempprobeInkBird::serviceScanDone(" << m_deviceAddress << ")";
}

void DeviceTempprobeInkBird::addLowEnergyService(const QBluetoothUuid &uuid)
{
    //qDebug() << "DeviceTempprobeInkBird::addLowEnergyService(" << uuid.toString() << ")";
}

/* ************************************************************************** */

void DeviceTempprobeInkBird::parseAdvertisementData(const QByteArray &value)
{
    //
}

/* ************************************************************************** */
