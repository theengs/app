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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDateTime>
#include <QDebug>

/* ************************************************************************** */

DeviceTheengsGeneric::DeviceTheengsGeneric(QString &deviceAddr, QString &deviceName, QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS;
}

DeviceTheengsGeneric::DeviceTheengsGeneric(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceTheengs(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS;
}

DeviceTheengsGeneric::~DeviceTheengsGeneric()
{
    //
}

/* ************************************************************************** */

void DeviceTheengsGeneric::parseAdvertisementTheengs(const QString &json)
{
    //qDebug() << "DeviceTheengsGeneric::parseAdvertisementTheengs()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setSetting("mac", obj["mac"].toString());

    if (obj.contains("tempc")) {
        if (!hasTemperatureSensor()) {
            m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
            m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
            Q_EMIT sensorUpdated();
        }

        if (m_temperature != obj["tempc"].toDouble()) {
            m_temperature = obj["tempc"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("hum")) {
        if (!hasTemperatureSensor()) {
            m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
            m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
            Q_EMIT sensorUpdated();
        }

        if (m_humidity != obj["hum"].toDouble()) {
            m_humidity = obj["hum"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("pres")) {
        if (!hasTemperatureSensor()) {
            m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
            m_deviceSensors += DeviceUtils::SENSOR_PRESSURE;
            Q_EMIT sensorUpdated();
        }

        if (m_pressure != obj["pres"].toDouble() * 10.0) {
            m_pressure = obj["pres"].toDouble() * 10.0;
            Q_EMIT dataUpdated();
        }
    }
}

/* ************************************************************************** */
