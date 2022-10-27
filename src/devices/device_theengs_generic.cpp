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

    // Load initial data into the GUI (if they are no more than 12h old)
    if (m_deviceType == DeviceUtils::DEVICE_PLANTSENSOR)
    {
        getSqlDeviceInfos();
        getSqlPlantData(12*60);
    }
    else if (m_deviceType == DeviceUtils::DEVICE_ENVIRONMENTAL)
    {
        getSqlSensorData(12*60);
    }
    else if (m_deviceType == DeviceUtils::DEVICE_THERMOMETER ||
             m_deviceType == DeviceUtils::DEVICE_THEENGS_THERMOMETER)
    {
        getSqlThermoData(12*60);
    }
    else if (m_deviceType == DeviceUtils::DEVICE_THEENGS_SCALE)
    {
        getSqlScaleData(12*60);
    }
    else // if (m_deviceType == DeviceUtils::DEVICE_THEENGS_GENERIC)
    {
        getSqlSensorData(12*60);
    }
}

DeviceTheengsGeneric::DeviceTheengsGeneric(const QBluetoothDeviceInfo &d,
                                           const QString &deviceModel, const QString &devicePropsJson,
                                           QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;

    parseTheengsProps(devicePropsJson);

    // Load initial data into the GUI (if they are no more than 12h old)
    if (m_deviceType == DeviceUtils::DEVICE_PLANTSENSOR)
    {
        getSqlDeviceInfos();
        getSqlPlantData(12*60);
    }
    else if (m_deviceType == DeviceUtils::DEVICE_ENVIRONMENTAL)
    {
        getSqlSensorData(12*60);
    }
    else if (m_deviceType == DeviceUtils::DEVICE_THERMOMETER ||
             m_deviceType == DeviceUtils::DEVICE_THEENGS_THERMOMETER)
    {
        getSqlThermoData(12*60);
    }
    else if (m_deviceType == DeviceUtils::DEVICE_THEENGS_SCALE)
    {
        getSqlScaleData(12*60);
    }
    else // if (m_deviceType == DeviceUtils::DEVICE_THEENGS_GENERIC)
    {
        getSqlSensorData(12*60);
    }
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
    if (prop.contains("temp")) m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    if (prop.contains("tempc")) m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    if (prop.contains("tempf")) m_deviceSensors += DeviceUtils::SENSOR_TEMPERATURE;
    if (prop.contains("hum")) m_deviceSensors += DeviceUtils::SENSOR_HUMIDITY;
    if (prop.contains("lux")) m_deviceSensors += DeviceUtils::SENSOR_LUMINOSITY;
    if (prop.contains("pres")) m_deviceSensors += DeviceUtils::SENSOR_PRESSURE;
    if (prop.contains("for")) m_deviceSensors += DeviceUtils::SENSOR_HCHO;
    if (prop.contains("pm25")) m_deviceSensors += DeviceUtils::SENSOR_PM25;
    if (prop.contains("pm10")) m_deviceSensors += DeviceUtils::SENSOR_PM10;
    if (prop.contains("co2")) m_deviceSensors += DeviceUtils::SENSOR_CO2;
    Q_EMIT sensorsUpdated();

    // Device mode
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    // Device type
    if (hasSoilMoistureSensor() && hasSoilConductivitySensor()) m_deviceType = DeviceUtils::DEVICE_PLANTSENSOR;
    else if (hasWeight()) m_deviceType = DeviceUtils::DEVICE_THEENGS_SCALE;
    else if (hasHchoSensor() || hasCo2Sensor() ||hasPM10Sensor()) m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
    else
    {
        /// full generic ///
        m_deviceType = DeviceUtils::DEVICE_THEENGS_GENERIC;

        for (auto it = prop.begin(), end = prop.end(); it != end; ++it)
        {
            QString prop_key = it.key();
            QJsonObject prop_value = it.value().toObject();

            QString value_name;
            QString value_unit;
            if (prop_value.contains("name")) value_name = prop_value["name"].toString();
            if (prop_value.contains("unit")) value_unit = prop_value["unit"].toString();

            {
                TheengsGenericData *dat = new TheengsGenericData(value_name, value_unit, this);
                m_genericData.push_back(dat);
            }
        }
        Q_EMIT genericDataUpdated();

        /// maybe not ///
        if (m_genericData.size() == 2 && hasTemperatureSensor() && hasHumiditySensor()) m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
        if (m_genericData.size() == 3 && hasBatteryLevel() && hasTemperatureSensor() && hasHumiditySensor()) m_deviceType = DeviceUtils::DEVICE_THERMOMETER;
        if (m_genericData.size() == 3 && hasTemperatureSensor() && hasHumiditySensor() && hasLuminositySensor()) m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
        if (m_genericData.size() == 4 && hasBatteryLevel() && hasTemperatureSensor() && hasHumiditySensor() && hasLuminositySensor()) m_deviceType = DeviceUtils::DEVICE_ENVIRONMENTAL;
    }
}

/* ************************************************************************** */

void DeviceTheengsGeneric::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsGeneric::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    {
        /// full generic ///
        for (auto it = obj.begin(), end = obj.end(); it != end; ++it)
        {
            QString prop_key = it.key();
            QJsonValue prop_value = it.value();
            //QJsonObject prop_value = it.value().toObject();

            for (auto gd: m_genericData)
            {
                if (static_cast<TheengsGenericData*>(gd)->getName() == prop_key)
                {
                    static_cast<TheengsGenericData*>(gd)->setData(it.value());
                    break;
                }
            }
        }
        Q_EMIT genericDataUpdated();
    }

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setAddressMAC(obj["mac"].toString());

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

    if (obj.contains("temp")) {
        if (m_temperature != obj["temp"].toDouble()) {
            m_temperature = obj["temp"].toDouble();
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
        if (m_pressure != obj["pres"].toDouble()) {
            m_pressure = obj["pres"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("for")) {
        if (m_hcho != obj["for"].toDouble()) {
            m_hcho = obj["for"].toDouble() * 1000.0;
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("pm25")) {
        if (m_hcho != obj["pm25"].toDouble()) {
            m_pm_25 = obj["pm25"].toDouble();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("pm10")) {
        if (m_pm_10 != obj["pm10"].toDouble()) {
            m_pm_10 = obj["pm10"].toDouble();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("co2")) {
        if (m_co2 != obj["co2"].toDouble()) {
            m_co2 = obj["co2"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    {
        m_lastUpdate = QDateTime::currentDateTime();

        if (needsUpdateDb())
        {
            if (isPlantSensor())
            {
                addDatabaseRecord_plants(m_lastUpdate.toSecsSinceEpoch(),
                                         m_soilMoisture, m_soilConductivity,
                                         m_temperature, m_humidity);
            }
            else if (isThermometer())
            {
                //
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
            addData.prepare("REPLACE INTO plantData (deviceAddr, timestamp_rounded, timestamp, soilMoisture, soilConductivity, temperature, luminosity)"
                            " VALUES (:deviceAddr, :timestamp_rounded, :timestamp, :hygro, :condu, :temp, :lumi)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":timestamp_rounded", tmcd.toString("yyyy-MM-dd hh:00:00"));
            addData.bindValue(":timestamp", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
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
                qWarning() << "> DeviceTheengsGeneric addData.exec() ERROR"
                           << addData.lastError().type() << ":" << addData.lastError().text();
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
/* ************************************************************************** */

void DeviceTheengsGeneric::updateChartData_environmentalVoc(int maxDays)
{
    if (maxDays <= 0) return;
    int maxMonths = 2;

    qDeleteAll(m_chartData_env);
    m_chartData_env.clear();
    ChartDataVoc *previousdata = nullptr;

    if (m_dbInternal || m_dbExternal)
    {
        QString strftime_mid = "strftime('%Y-%m-%d', timestamp)"; // sqlite
        if (m_dbExternal) strftime_mid = "DATE_FORMAT(timestamp, '%Y-%m-%d')"; // mysql

        QString datetime_months = "datetime('now','-" + QString::number(maxMonths) + " month')"; // sqlite
        if (m_dbExternal) datetime_months = "DATE_SUB(NOW(), INTERVAL -" + QString::number(maxMonths) + " MONTH)"; // mysql

        QSqlQuery graphData;
        graphData.prepare("SELECT " + strftime_mid + "," \
                            "min(voc), avg(voc), max(voc)," \
                            "min(hcho), avg(hcho), max(hcho)," \
                            "min(co2), avg(co2), max(co2) " \
                          "FROM sensorData " \
                          "WHERE deviceAddr = :deviceAddr AND timestamp >= " + datetime_months + " " \
                          "GROUP BY " + strftime_mid + " " \
                          "ORDER BY timestamp DESC "
                          "LIMIT :maxDays;");
        graphData.bindValue(":deviceAddr", getAddress());
        graphData.bindValue(":maxDays", maxDays);

        if (graphData.exec() == false)
        {
            qWarning() << "> graphData.exec() ERROR"
                       << graphData.lastError().type() << ":" << graphData.lastError().text();
            return;
        }

        while (graphData.next())
        {
            if (m_chartData_env.size() < maxDays)
            {
                // missing day(s)?
                if (previousdata)
                {
                    QDateTime datefromsql = graphData.value(0).toDateTime();
                    int diff = datefromsql.daysTo(previousdata->getDateTime());
                    for (int i = diff; i > 1; i--)
                    {
                        if (m_chartData_env.size() < (maxDays-1))
                        {
                            QDateTime fakedate(datefromsql.addDays(i-1));
                            m_chartData_env.push_front(new ChartDataVoc(fakedate, -99, -99, -99, -99, -99, -99, -99, -99, -99, this));
                        }
                    }
                }

                // data
                ChartDataVoc *d = new ChartDataVoc(graphData.value(0).toDateTime(),
                                                   graphData.value(1).toFloat(), graphData.value(2).toFloat(), graphData.value(3).toFloat(),
                                                   graphData.value(4).toFloat(), graphData.value(5).toFloat(), graphData.value(6).toFloat(),
                                                   graphData.value(7).toFloat(), graphData.value(8).toFloat(), graphData.value(9).toFloat(),
                                                   this);
                m_chartData_env.push_front(d);
                previousdata = d;
            }
        }

        // missing day(s)?
        {
            // after
            QDateTime today = QDateTime::currentDateTime();
            int missing = maxDays;
            if (previousdata) missing = static_cast<ChartDataVoc *>(m_chartData_env.last())->getDateTime().daysTo(today);
            for (int i = missing - 1; i >= 0; i--)
            {
                QDateTime fakedate(today.addDays(-i));
                m_chartData_env.push_back(new ChartDataVoc(fakedate, -99, -99, -99, -99, -99, -99, -99, -99, -99, this));
            }

            // before
            today = QDateTime::currentDateTime();
            for (int i = m_chartData_env.size(); i < maxDays; i++)
            {
                QDateTime fakedate(today.addDays(-i));
                m_chartData_env.push_front(new ChartDataVoc(fakedate, -99, -99, -99, -99, -99, -99, -99, -99, -99, this));
            }
        }
/*
        // first vs last (for months less than 31 days long)
        if (m_chartData_env.size() > 1)
        {
            while (!m_chartData_env.isEmpty() &&
                   static_cast<ChartDataVoc *>(m_chartData_env.first())->getDay() ==
                   static_cast<ChartDataVoc *>(m_chartData_env.last())->getDay())
            {
                m_chartData_env.pop_front();
            }
        }
*/
        Q_EMIT chartDataEnvUpdated();
    }
}

/* ************************************************************************** */
