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

    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
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

    m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
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
    Q_UNUSED(uuid)
}

/* ************************************************************************** */

void DeviceTempprobeInkBird::parseAdvertisementData(const QByteArray &value)
{
    //qDebug() << "DeviceTempprobeInkBird::parseAdvertisementData(" << m_deviceAddress << ")" << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();

    // x bytes message
    if (value.size() >= 2 + 8 + 4)
    {
        const quint8 *data = reinterpret_cast<const quint8 *>(value.constData());

        float temp1 = -99;
        float temp2 = -99;
        float temp3 = -99;
        float temp4 = -99;
        float temp5 = -99;
        float temp6 = -99;

        int offset = 10;

        if (value.size() >= 14)
        {
            temp1 = static_cast<int16_t>(data[offset] + (data[offset+1] << 8)) / 10.f;
            offset += 2;
            if (temp1 != m_temperature1)
            {
                if (temp1 > -20.f && temp1 < 200.f)
                {
                    m_temperature1 = temp1;
                    Q_EMIT dataUpdated();
                }
            }

            temp2 = static_cast<int16_t>(data[offset] + (data[offset+1] << 8)) / 10.f;
            offset += 2;
            if (temp2 != m_temperature2)
            {
                if (temp2 > -20.f && temp2 < 200.f)
                {
                    m_temperature2 = temp2;
                    Q_EMIT dataUpdated();
                }
            }
        }
        if (value.size() >= 18)
        {
            temp3 = static_cast<int16_t>(data[offset] + (data[offset+1] << 8)) / 10.f;
            offset += 2;
            if (temp3 != m_temperature3)
            {
                if (temp3 > -20.f && temp3 < 200.f)
                {
                    m_temperature3 = temp3;
                    Q_EMIT dataUpdated();
                }
            }

            temp4 = static_cast<int16_t>(data[offset] + (data[offset+1] << 8)) / 10.f;
            offset += 2;
            if (temp4 != m_temperature4)
            {
                if (temp4 > -20.f && temp4 < 200.f)
                {
                    m_temperature4 = temp4;
                    Q_EMIT dataUpdated();
                }
            }
        }
        if (value.size() >= 22)
        {
            temp5 = static_cast<int16_t>(data[offset] + (data[offset+1] << 8)) / 10.f;
            offset += 2;
            if (temp5 != m_temperature5)
            {
                if (temp5 > -20.f && temp5 < 200.f)
                {
                    m_temperature5 = temp5;
                    Q_EMIT dataUpdated();
                }
            }

            temp6 = static_cast<int16_t>(data[offset] + (data[offset+1] << 8)) / 10.f;
            offset += 2;
            if (temp6 != m_temperature6)
            {
                if (temp6 > -20.f && temp6 < 200.f)
                {
                    m_temperature6 = temp6;
                    Q_EMIT dataUpdated();
                }
            }
        }

        //if (m_temperature > -99 && m_humidity > -99 && m_hcho > -99)
        {
            m_lastUpdate = QDateTime::currentDateTime();
            refreshDataFinished(true);

            if (needsUpdateDb())
            {
                // TODO // UPDATE DB
            }
        }

        if (temp1 > -99 || temp2 > -99 || temp3 > -99 || temp4 > -99 || temp5 > -99 || temp6 > -99)
        {
            qDebug() << "* InkBird manufacturer data:" << getName() << getAddress() << "(" << value.size() << ") bytes";
            //if (!mac.isEmpty()) qDebug() << "- MAC:" << mac;
            if (temp1 > -99) qDebug() << "- temperature1:" << temp1;
            if (temp2 > -99) qDebug() << "- temperature2:" << temp2;
            if (temp3 > -99) qDebug() << "- temperature3:" << temp3;
            if (temp4 > -99) qDebug() << "- temperature4:" << temp4;
            if (temp5 > -99) qDebug() << "- temperature5:" << temp5;
            if (temp6 > -99) qDebug() << "- temperature6:" << temp6;
        }
    }
}

/* ************************************************************************** */
