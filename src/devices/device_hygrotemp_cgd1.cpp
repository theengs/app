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

#include "device_hygrotemp_cgd1.h"

#include <cstdint>
#include <cmath>

#include <QBluetoothUuid>
#include <QLowEnergyService>

#include <QDateTime>
#include <QDebug>

/* ************************************************************************** */

DeviceHygrotempCGD1::DeviceHygrotempCGD1(const QString &deviceAddr, const QString &deviceName, QObject *parent):
    DeviceThermometer(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;
    m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
}

DeviceHygrotempCGD1::DeviceHygrotempCGD1(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceThermometer(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;
    m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
}

DeviceHygrotempCGD1::~DeviceHygrotempCGD1()
{
    //
}

/* ************************************************************************** */

void DeviceHygrotempCGD1::serviceScanDone()
{
    //qDebug() << "DeviceHygrotempCGD1::serviceScanDone(" << m_deviceAddress << ")";
}

void DeviceHygrotempCGD1::addLowEnergyService(const QBluetoothUuid &uuid)
{
    //qDebug() << "DeviceHygrotempCGD1::addLowEnergyService(" << uuid.toString() << ")";
    Q_UNUSED (uuid)
}

/* ************************************************************************** */

void DeviceHygrotempCGD1::parseAdvertisementData(const uint16_t adv_mode,
                                                 const uint16_t adv_id,
                                                 const QByteArray &ba)
{
/*
    qDebug() << "DeviceHygrotempCGD1::parseAdvertisementData(" << m_deviceAddress
             << " - " << adv_mode << " - 0x" << adv_id << ")";
    qDebug() << "DATA (" << ba.size() << "bytes)   >  0x" << ba.toHex();
*/

    // MiBeacon? // 12 bytes messages?
    // MiBeacon? // 14 bytes messages?

    if (ba.size() == 17) // Qingping data protocol // 17 bytes messages
    {
        const quint8 *data = reinterpret_cast<const quint8 *>(ba.constData());

        // Save mac address (for macOS and iOS)
        if (!hasAddressMAC())
        {
            QString mac;

            mac += ba.mid(10,1).toHex().toUpper();
            mac += ':';
            mac += ba.mid(9,1).toHex().toUpper();
            mac += ':';
            mac += ba.mid(8,1).toHex().toUpper();
            mac += ':';
            mac += ba.mid(7,1).toHex().toUpper();
            mac += ':';
            mac += ba.mid(6,1).toHex().toUpper();
            mac += ':';
            mac += ba.mid(5,1).toHex().toUpper();

            setAddressMAC(mac);
        }

        int batt = -99;
        float temp = -99.f;
        float humi = -99.f;

        // get data
        if (data[1] == 0x16 || // CGG1
            data[1] == 0x07 || // CGG1
            data[1] == 0x10 || // CGDK2
            data[1] == 0x09 || // CGP1W
            data[1] == 0x0c)   // CGD1
        {
            temp = static_cast<int32_t>(data[10] + (data[11] << 8)) / 10.f;
            if (temp != m_temperature)
            {
                if (temp > -30.f && temp < 100.f)
                {
                    m_temperature = temp;
                    Q_EMIT dataUpdated();
                }
            }

            humi = static_cast<int32_t>(data[12] + (data[13] << 8)) / 10.f;
            if (humi != m_humidity)
            {
                if (humi >= 0.f && humi <= 100.f)
                {
                    m_humidity = humi;
                    Q_EMIT dataUpdated();
                }
            }

            batt = static_cast<int8_t>(data[16]);
            setBattery(batt);
        }

        if (m_temperature > -99.f && m_humidity > -99.f)
        {
            m_lastUpdate = QDateTime::currentDateTime();

            if (needsUpdateDb_mini())
            {
                addDatabaseRecord_hygrometer(m_lastUpdate.toSecsSinceEpoch(), m_temperature, m_humidity);
            }

            refreshDataFinished(true);
        }
/*
        if (batt > -99 || temp > -99.f || humi > -99.f)
        {
            qDebug() << "* CGD1 service data:" << getName() << getAddress() << "(" << value.size() << ") bytes";
            if (batt > -99) qDebug() << "- battery:" << batt;
            if (temp > -99) qDebug() << "- temperature:" << temp;
            if (humi > -99) qDebug() << "- humidity:" << humi;
        }
*/
    }
}

/* ************************************************************************** */
