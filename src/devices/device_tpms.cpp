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

#include "device_tpms.h"
#include "utils/utils_versionchecker.h"

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

DeviceTPMS::DeviceTPMS(QString &deviceAddr, QString &deviceName, QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;

    m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    m_deviceSensors += DeviceUtils::SENSOR_PRESSURE;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_2;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_4;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_3;
}

DeviceTPMS::DeviceTPMS(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceTheengs(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;

    m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    m_deviceSensors += DeviceUtils::SENSOR_PRESSURE;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_2;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_4;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;
    m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_3;
}

DeviceTPMS::~DeviceTPMS()
{
    if (m_bleController) m_bleController->disconnectFromDevice();
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTPMS::serviceScanDone()
{
    //qDebug() << "DeviceTPMS::serviceScanDone(" << m_deviceAddress << ")";
}

/* ************************************************************************** */

void DeviceTPMS::addLowEnergyService(const QBluetoothUuid &uuid)
{
    //qDebug() << "DeviceTPMS::addLowEnergyService(" << uuid.toString() << ")";
}

/* ************************************************************************** */

void DeviceTPMS::parseAdvertisementData(const QByteArray &value)
{
    //
}

/* ************************************************************************** */
