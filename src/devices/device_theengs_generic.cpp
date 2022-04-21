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

DeviceTheengsGeneric::DeviceTheengsGeneric(const QString &deviceAddr, const QString &deviceName,
                                           const QString &deviceModel, const QString &devicePropsJson,
                                           QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsGeneric::DeviceTheengsGeneric(const QBluetoothDeviceInfo &d,
                                           const QString &deviceModel, const QString &devicePropsJson,
                                           QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsGeneric::~DeviceTheengsGeneric()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsGeneric::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsGeneric::parseTheengsProps()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject prop = doc.object()["properties"].toObject();

    // Capabilities
    if (prop.contains("batt")) m_deviceCapabilities += DeviceUtils::DEVICE_BATTERY;
    Q_EMIT capabilitiesUpdated();

    // Sensors
    if (prop.contains("moi")) m_deviceSensors += DeviceUtils::SENSOR_SOIL_MOISTURE;
    if (prop.contains("fer")) m_deviceSensors += DeviceUtils::SENSOR_SOIL_CONDUCTIVITY;
    if (prop.contains("tempc")) m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    if (prop.contains("hum")) m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
    if (prop.contains("lux")) m_deviceSensors += DeviceUtils::SENSOR_LUMINOSITY;
    if (prop.contains("pres")) m_deviceSensors += DeviceUtils::SENSOR_PRESSURE;
    if (prop.contains("for")) m_deviceSensors += DeviceUtils::SENSOR_HCHO;
    Q_EMIT sensorsUpdated();

    // Device mode
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    // Device type
    if (hasSoilMoistureSensor() && hasSoilConductivitySensor()) m_deviceType = DeviceUtils::DEVICE_PLANTSENSOR;
    else if (hasHchoSensor()) m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
    else if (hasWeight()) m_deviceType = DeviceUtils::DEVICE_SCALE;
    else if (hasTemperatureSensor() || hasHumiditySensor()) m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
    else m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
}

/* ************************************************************************** */

void DeviceTheengsGeneric::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsGeneric::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setSetting("mac", obj["mac"].toString());

    if (obj.contains("moi")) {
        if (m_soilMoisture != obj["moi"].toDouble()) {
            m_soilMoisture = obj["moi"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("fer")) {
        if (m_soilConductivity != obj["fer"].toDouble()) {
            m_soilConductivity = obj["fer"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("tempc")) {
        if (m_temperature != obj["tempc"].toDouble()) {
            m_temperature = obj["tempc"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("hum")) {
        if (m_humidity != obj["hum"].toDouble()) {
            m_humidity = obj["hum"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("lux")) {
        if (m_luminosityLux != obj["lux"].toDouble()) {
            m_luminosityLux = obj["lux"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("pres")) {
        if (m_pressure != obj["pres"].toDouble() * 10.0) {
            m_pressure = obj["pres"].toDouble() * 10.0;
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("for")) {
        if (m_hcho != obj["for"].toDouble()) {
            m_hcho = obj["for"].toDouble() * 1000.0;
            Q_EMIT dataUpdated();
        }
    }

    {
        m_lastUpdate = QDateTime::currentDateTime();

        //if (needsUpdateDb()) // always on for theengs advertising
        {
            if (isPlantSensor())
            {
                addDatabaseRecord_plants(m_lastUpdate.toSecsSinceEpoch(),
                                         m_soilMoisture, m_soilConductivity,
                                         m_temperature, m_humidity);
            }
            else if (isThermometer())
            {
                if (hasTemperatureSensor() && hasHumiditySensor())
                {
                    addDatabaseRecord_hygrometer(m_lastUpdate.toSecsSinceEpoch(),
                                                 m_temperature, m_humidity);
                }
                else if (hasTemperatureSensor())
                {
                    addDatabaseRecord_thermometer(m_lastUpdate.toSecsSinceEpoch(),
                                                  m_temperature);
                }
            }
            else if (isEnvironmentalSensor())
            {
                //
            }
        }

        refreshDataFinished(true);
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DeviceTheengsGeneric::areValuesValid_plants(const int soilMoisture, const int soilConductivity,
                                                 const float temperature, const int luminosity) const
{
    if (hasSoilMoistureSensor() && (soilMoisture < 0 || soilMoisture > 100)) return false;
    if (hasSoilConductivitySensor() && (soilConductivity < 0 || soilConductivity > 20000)) return false;
    if (hasTemperatureSensor() && (temperature < -30.f || temperature > 100.f)) return false;
    if (hasLuminositySensor() && (luminosity < 0 || luminosity > 150000)) return false;

    return true;
}

bool DeviceTheengsGeneric::addDatabaseRecord_plants(const int64_t timestamp,
                                                    const int soilMoisture, const int soilConductivity,
                                                    const float temperature, const int luminosity)
{
    bool status = false;

    if (areValuesValid_plants(soilMoisture, soilConductivity, temperature, luminosity))
    {
        if (m_dbInternal || m_dbExternal)
        {
            // SQL date format YYYY-MM-DD HH:MM:SS
            // We only save one record every 60m

            QDateTime tmcd = QDateTime::fromSecsSinceEpoch(timestamp);

            QSqlQuery addData;
            addData.prepare("REPLACE INTO plantData (deviceAddr, ts, ts_full, soilMoisture, soilConductivity, temperature, luminosity)"
                            " VALUES (:deviceAddr, :ts, :ts_full, :hygro, :condu, :temp, :lumi)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":ts", tmcd.toString("yyyy-MM-dd hh:00:00"));
            addData.bindValue(":ts_full", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
            addData.bindValue(":hygro", soilMoisture);
            addData.bindValue(":condu", soilConductivity);
            addData.bindValue(":temp", temperature);
            addData.bindValue(":lumi", luminosity);
            status = addData.exec();

            if (status)
            {
                m_lastUpdateDatabase = tmcd;
            }
            else
            {
                qWarning() << "> DeviceTheengsGeneric addData.exec() ERROR" << addData.lastError().type() << ":" << addData.lastError().text();
            }
        }
    }
    else
    {
        qWarning() << "DeviceTheengsGeneric values are INVALID";
    }

    return status;
}

/* ************************************************************************** */

bool DeviceTheengsGeneric::areValuesValid_thermometer(const float t) const
{
    if (t < -30.f || t > 100.f) return false;

    return true;
}

bool DeviceTheengsGeneric::addDatabaseRecord_thermometer(const int64_t timestamp, const float t)
{
    bool status = false;

    //qDebug() << "DeviceTheengsGeneric::addDatabaseRecord_thermometer()" << t;

    if (areValuesValid_thermometer(t))
    {
        if (m_dbInternal || m_dbExternal)
        {
            // SQL date format YYYY-MM-DD HH:MM:SS
            // We only save one record every 30m

            QDateTime tmcd = QDateTime::fromSecsSinceEpoch(timestamp);
            QDateTime tmcd_rounded = QDateTime::fromSecsSinceEpoch(timestamp + (1800 - timestamp % 1800) - 1800);

            QSqlQuery addData;
            addData.prepare("REPLACE INTO plantData (deviceAddr, ts, ts_full, temperature)"
                            " VALUES (:deviceAddr, :ts, :ts_full, :temp)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":ts", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
            addData.bindValue(":ts_full", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
            addData.bindValue(":temp", t);
            status = addData.exec();

            if (status)
            {
                m_lastUpdateDatabase = tmcd;
            }
            else
            {
                qWarning() << "> DeviceTheengsGeneric addData.exec() ERROR" << addData.lastError().type() << ":" << addData.lastError().text();
            }
        }
    }
    else
    {
        qWarning() << "DeviceTheengsGeneric values are INVALID";
    }

    return status;
}

/* ************************************************************************** */

bool DeviceTheengsGeneric::areValuesValid_hygrometer(const float t, const float h) const
{
    if (t <= 0.f && h <= 0.f) return false;
    if (t < -30.f || t > 100.f) return false;
    if (h < 0.f || t > 100.f) return false;

    return true;
}

bool DeviceTheengsGeneric::addDatabaseRecord_hygrometer(const int64_t timestamp, const float t, const float h)
{
    bool status = false;

    //qDebug() << "DeviceTheengsGeneric::addDatabaseRecord_hygrometer()" << t << h;

    if (areValuesValid_hygrometer(t, h))
    {
        if (m_dbInternal || m_dbExternal)
        {
            // SQL date format YYYY-MM-DD HH:MM:SS
            // We only save one record every 30m

            QDateTime tmcd = QDateTime::fromSecsSinceEpoch(timestamp);
            QDateTime tmcd_rounded = QDateTime::fromSecsSinceEpoch(timestamp + (1800 - timestamp % 1800) - 1800);

            QSqlQuery addData;
            addData.prepare("REPLACE INTO plantData (deviceAddr, ts, ts_full, temperature, humidity)"
                            " VALUES (:deviceAddr, :ts, :ts_full, :temp, :hygro)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":ts", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
            addData.bindValue(":ts_full", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
            addData.bindValue(":temp", t);
            addData.bindValue(":hygro", h);
            status = addData.exec();

            if (status)
            {
                m_lastUpdateDatabase = tmcd;
            }
            else
            {
                qWarning() << "> DeviceTheengsGeneric addData.exec() ERROR" << addData.lastError().type() << ":" << addData.lastError().text();
            }
        }
    }
    else
    {
        qWarning() << "DeviceTheengsGeneric values are INVALID";
    }

    return status;
}

/* ************************************************************************** */
