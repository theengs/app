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
    else if (hasWeight()) m_deviceType = DeviceUtils::DEVICE_THEENGS_SCALE;
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
            addData.prepare("REPLACE INTO thermoData (deviceAddr, timestamp_rounded, timestamp, temperature)"
                            " VALUES (:deviceAddr, :timestamp_rounded, :timestamp, :temp)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
            addData.bindValue(":timestamp", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
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
    if (h < 0.f || h > 100.f) return false;

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
            addData.prepare("REPLACE INTO thermoData (deviceAddr, timestamp_rounded, timestamp, temperature, humidity)"
                            " VALUES (:deviceAddr, :timestamp_rounded, :timestamp, :temp, :hygro)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
            addData.bindValue(":timestamp", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
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
/* ************************************************************************** */

void DeviceTheengsGeneric::getChartData_thermometerAIO(int maxDays, QDateTimeAxis *axis,
                                                       QLineSeries *temp, QLineSeries *humi)
{
    if (!axis || !temp || !humi) return;

    temp->clear();
    humi->clear();

    if (m_dbInternal || m_dbExternal)
    {
        QString datetime_days = "datetime('now', 'localtime', '-" + QString::number(maxDays) + " days')";
        if (m_dbExternal) datetime_days = "DATE_SUB(NOW(), INTERVAL " + QString::number(maxDays) + " DAY)";

        QSqlQuery graphData;
        graphData.prepare("SELECT timestamp, temperature, humidity " \
                          "FROM thermoData " \
                          "WHERE deviceAddr = :deviceAddr AND timestamp >= " + datetime_days + ";");
        graphData.bindValue(":deviceAddr", getAddress());

        if (graphData.exec() == false)
        {
            qWarning() << "> graphData.exec(thermo aio) ERROR"
                       << graphData.lastError().type() << ":" << graphData.lastError().text();
            return;
        }

        axis->setFormat("dd MMM");
        axis->setMax(QDateTime::currentDateTime());
        bool minSet = false;
        bool minmaxChanged = false;

        while (graphData.next())
        {
            QDateTime date = QDateTime::fromString(graphData.value(0).toString(), "yyyy-MM-dd hh:mm:ss");
            if (!minSet)
            {
                axis->setMin(date);
                minSet = true;
            }
            qint64 timecode = date.toMSecsSinceEpoch();

            // data
            temp->append(timecode, graphData.value(1).toReal());
            humi->append(timecode, graphData.value(2).toReal());

            // min/max
            if (graphData.value(1).toFloat() < m_tempMin) { m_tempMin = graphData.value(1).toFloat(); minmaxChanged = true; }
            if (graphData.value(2).toFloat() < m_humiMin) { m_humiMin = graphData.value(2).toFloat(); minmaxChanged = true; }

            if (graphData.value(1).toFloat() > m_tempMax) { m_tempMax = graphData.value(1).toFloat(); minmaxChanged = true; }
            if (graphData.value(2).toFloat() > m_humiMax) { m_humiMax = graphData.value(2).toFloat(); minmaxChanged = true; }
        }

        if (minmaxChanged) { Q_EMIT minmaxUpdated(); }
    }
    else
    {
        // No database, use fake values
        m_tempMin = 0.f;
        m_tempMax = 36.f;
        m_humiMin = 0;
        m_humiMax = 100;

        Q_EMIT minmaxUpdated();
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsGeneric::updateChartData_thermometerMinMax(int maxDays)
{
    if (maxDays <= 0) return;
    int maxMonths = 2;

    qDeleteAll(m_chartData_minmax);
    m_chartData_minmax.clear();
    m_tempMin = 999.f;
    m_tempMax = -99.f;
    ChartDataMinMax *previousdata = nullptr;

    if (m_dbInternal || m_dbExternal)
    {
        QString strftime_d = "strftime('%Y-%m-%d', timestamp)"; // sqlite
        if (m_dbExternal) strftime_d = "DATE_FORMAT(timestamp, '%Y-%m-%d')"; // mysql

        QString datetime_months = "datetime('now','-" + QString::number(maxMonths) + " month')"; // sqlite
        if (m_dbExternal) datetime_months = "DATE_SUB(NOW(), INTERVAL -" + QString::number(maxMonths) + " MONTH)"; // mysql

        QSqlQuery graphData;
        graphData.prepare("SELECT " + strftime_d + ", min(temperature), avg(temperature), max(temperature), min(humidity), max(humidity) " \
                          "FROM thermoData " \
                          "WHERE deviceAddr = :deviceAddr AND timestamp >= " + datetime_months + " " \
                          "GROUP BY " + strftime_d + " " \
                          "ORDER BY " + strftime_d + " DESC;");
        graphData.bindValue(":deviceAddr", getAddress());
        graphData.bindValue(":maxDays", maxDays);

        if (graphData.exec() == false)
        {
            qWarning() << "> graphData.exec(thermo m/m) ERROR"
                       << graphData.lastError().type() << ":" << graphData.lastError().text();
            return;
        }

        bool minmaxChanged = false;

        while (graphData.next())
        {
            if (m_chartData_minmax.size() < maxDays)
            {
                // missing day(s)?
                if (previousdata)
                {
                    QDateTime datefromsql = graphData.value(0).toDateTime();
                    int diff = datefromsql.daysTo(previousdata->getDateTime());
                    for (int i = diff; i > 1; i--)
                    {
                        if (m_chartData_minmax.size() < (maxDays-1))
                        {
                            QDateTime fakedate(datefromsql.addDays(i-1));
                            m_chartData_minmax.push_front(new ChartDataMinMax(fakedate, -99, -99, -99, -99, -99, this));
                        }
                    }
                }

                // min/max
                if (graphData.value(1).toFloat() < m_tempMin) { m_tempMin = graphData.value(1).toFloat(); minmaxChanged = true; }
                if (graphData.value(3).toFloat() > m_tempMax) { m_tempMax = graphData.value(3).toFloat(); minmaxChanged = true; }
                if (graphData.value(4).toInt() < m_soilMoistureMin) { m_soilMoistureMin = graphData.value(4).toInt(); minmaxChanged = true; }
                if (graphData.value(5).toInt() > m_soilMoistureMax) { m_soilMoistureMax = graphData.value(5).toInt(); minmaxChanged = true; }

                // data
                ChartDataMinMax *d = new ChartDataMinMax(graphData.value(0).toDateTime(),
                                                         graphData.value(1).toFloat(), graphData.value(2).toFloat(), graphData.value(3).toFloat(),
                                                         graphData.value(4).toInt(), graphData.value(5).toInt(), this);
                m_chartData_minmax.push_front(d);
                previousdata = d;
            }
        }

        if (minmaxChanged) { Q_EMIT minmaxUpdated(); }

        // missing day(s)?
        {
            // after
            QDateTime today = QDateTime::currentDateTime();
            int missing = maxDays;
            if (previousdata) missing = static_cast<ChartDataMinMax *>(m_chartData_minmax.last())->getDateTime().daysTo(today);
            for (int i = missing - 1; i >= 0; i--)
            {
                QDateTime fakedate(today.addDays(-i));
                m_chartData_minmax.push_back(new ChartDataMinMax(fakedate, -99, -99, -99, -99, -99, this));
            }

            // before
            today = QDateTime::currentDateTime();
            for (int i = m_chartData_minmax.size(); i < maxDays; i++)
            {
                QDateTime fakedate(today.addDays(-i));
                m_chartData_minmax.push_front(new ChartDataMinMax(fakedate, -99, -99, -99, -99, -99, this));
            }
        }
/*
        // first vs last (for months less than 31 days long)
        if (m_chartData_minmax.size() > 1)
        {
            while (!m_chartData_minmax.isEmpty() &&
                   static_cast<ChartDataMinMax *>(m_chartData_minmax.first())->getDay() ==
                   static_cast<ChartDataMinMax *>(m_chartData_minmax.last())->getDay())
            {
                m_chartData_minmax.pop_front();
            }
        }
*/
        Q_EMIT chartDataMinMaxUpdated();
    }
    else
    {
        // No database, use fake values
        m_soilMoistureMin = 0;
        m_soilMoistureMax = 50;
        m_soilConduMin = 0;
        m_soilConduMax = 2000;
        m_soilTempMin = 0.f;
        m_soilTempMax = 36.f;
        m_soilPhMin = 0.f;
        m_soilPhMax = 15.f;
        m_tempMin = 0.f;
        m_tempMax = 36.f;
        m_humiMin = 0;
        m_humiMax = 100;
        m_luxMin = 0;
        m_luxMax = 10000;
        m_mmolMin = 0;
        m_mmolMax = 10000;

        Q_EMIT minmaxUpdated();
    }
}

/* ************************************************************************** */
