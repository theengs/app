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

#include "device_theengs.h"
#include "SettingsManager.h"

#include <cstdint>
#include <cmath>

#include <QBluetoothUuid>
#include <QBluetoothServiceInfo>
#include <QLowEnergyService>

#include <QSqlQuery>
#include <QSqlError>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDateTime>
#include <QDebug>

/* ************************************************************************** */

DeviceTheengs::DeviceTheengs(const QString &deviceAddr, const QString &deviceName, const QString &deviceModel, QObject *parent):
    DeviceSensor(deviceAddr, deviceName, parent)
{
    m_deviceModel = deviceModel;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
}

DeviceTheengs::DeviceTheengs(const QBluetoothDeviceInfo &d, const QString &deviceModel, QObject *parent):
    DeviceSensor(d, parent)
{
    m_deviceModel = deviceModel;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
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
    Q_UNUSED (uuid)
}

/* ************************************************************************** */

bool DeviceTheengs::hasData() const
{
    // If we have immediate data (<12h old)
/*
    if (isPlantSensor())
    {
        if (m_soilMoisture > -80.f)
            return true;
    }
    else if (isThermometer())
    {
        if (m_temperature > -80.f)
            return true;
    }
    else if (isEnvironmentalSensor())
    {
        if (m_dbInternal || m_dbExternal)
        {
            QSqlQuery hasData;
            hasData.prepare("SELECT COUNT(*) FROM sensorData WHERE deviceAddr = :deviceAddr;");
            hasData.bindValue(":deviceAddr", getAddress());

            if (hasData.exec() == false)
                qWarning() << "> hasData.exec(DeviceTheengs) ERROR" << hasData.lastError().type() << ":" << hasData.lastError().text();

            while (hasData.next())
            {
                if (hasData.value(0).toInt() > 0) // data count
                    return true;
            }
        }
    }
    else
*/
    if (isProbe())
    {
        if (m_temperature1 > -80.f || m_temperature2 > -80.f)
            return true;
    }
    else if (isScale())
    {
        if (m_weight > -80.f)
            return true;
    }
    else
    {
        return DeviceSensor::hasData();
    }

    // TODO // also check db?

    return false;
}

/* ************************************************************************** */

float DeviceTheengs::getTemp1() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp1F();
    return getTemp1C();
}
float DeviceTheengs::getTemp2() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp2F();
    return getTemp2C();
}
float DeviceTheengs::getTemp3() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp3F();
    return getTemp3C();
}
float DeviceTheengs::getTemp4() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp4F();
    return getTemp4C();
}
float DeviceTheengs::getTemp5() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp5F();
    return getTemp5C();
}
float DeviceTheengs::getTemp6() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp6F();
    return getTemp6C();
}

/* ************************************************************************** */

void DeviceTheengs::parseAdvertisementData(const QByteArray &value)
{
    //qDebug() << "DeviceTheengs::parseAdvertisementData()";
    //qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceTheengs::parseAdvertisementTheengs(const QString &json)
{
    //qDebug() << "DeviceTheengs::parseAdvertisementTheengs()";
    //qDebug() << "JSON:" << json;

    parseTheengsAdvertisement(json);
}

void DeviceTheengs::parseTheengsProps(const QString &json)
{
    //
}

void DeviceTheengs::parseTheengsAdvertisement(const QString &json)
{
    //
}

/* ************************************************************************** */
