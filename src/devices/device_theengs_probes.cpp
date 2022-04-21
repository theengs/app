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

#include "device_theengs_probes.h"

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

DeviceTheengsProbes::DeviceTheengsProbes(const QString &deviceAddr, const QString &deviceName,
                                         const QString &deviceModel, const QString &devicePropsJson,
                                         QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsProbes::DeviceTheengsProbes(const QBluetoothDeviceInfo &d,
                                         const QString &deviceModel, const QString &devicePropsJson,
                                         QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsProbes::~DeviceTheengsProbes()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsProbes::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsProbes::parseTheengsProps()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject prop = doc.object()["properties"].toObject();

    // Capabilities
    if (prop.contains("batt")) m_deviceCapabilities += DeviceUtils::DEVICE_BATTERY;
    Q_EMIT capabilitiesUpdated();

    // Sensors
    if (prop.contains("tempc")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
    if (prop.contains("tempc1")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
    if (prop.contains("tempc2")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;
    if (prop.contains("tempc3")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
    if (prop.contains("tempc4")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;
    if (prop.contains("tempc5")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_TEMPERATURE_5;
    if (prop.contains("tempc6")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_TEMPERATURE_6;
    if (prop.contains("count") && prop.contains("alarm")) m_deviceSensorsTheengs += DeviceUtilsTheengs::SENSOR_PROBES_TPMS;
    Q_EMIT sensorsUpdated();
}

/* ************************************************************************** */

void DeviceTheengsProbes::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsProbes::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setSetting("mac", obj["mac"].toString());

    if (obj["model"].toString() == "TPMS")
    {
        int idx = obj["count"].toInt();
        float pres = obj["pres"].toDouble() * 10.0;
        float temp = obj["tempc"].toDouble();
        int batt = obj["batt"].toInt();
        bool alarm = obj["alarm"].toBool();

        if (idx == 1)
        {
            m_pressure1 = pres;
            m_temperature1 = temp;
            m_battery1 = batt;
            m_alarm1 = alarm;
        }
        else if (idx == 2)
        {
            m_pressure2 = pres;
            m_temperature2 = temp;
            m_battery2 = batt;
            m_alarm2 = alarm;
        }
        else if (idx == 3)
        {
            m_pressure3 = pres;
            m_temperature3 = temp;
            m_battery3 = batt;
            m_alarm3 = alarm;
        }
        else if (idx == 4)
        {
            m_pressure4 = pres;
            m_temperature4 = temp;
            m_battery4 = batt;
            m_alarm4 = alarm;
        }
    }
    else // temperature probes
    {
        if (obj.contains("tempc")) m_temperature1 = obj["tempc"].toDouble();
        if (obj.contains("tempc1")) m_temperature1 = obj["tempc1"].toDouble();
        if (obj.contains("tempc2")) m_temperature2 = obj["tempc2"].toDouble();
        if (obj.contains("tempc3")) m_temperature3 = obj["tempc3"].toDouble();
        if (obj.contains("tempc4")) m_temperature4 = obj["tempc4"].toDouble();
        if (obj.contains("tempc5")) m_temperature5 = obj["tempc5"].toDouble();
        if (obj.contains("tempc6")) m_temperature6 = obj["tempc6"].toDouble();
    }

    {
        m_lastUpdate = QDateTime::currentDateTime();

        if (needsUpdateDb())
        {
            if (m_dbInternal || m_dbExternal)
            {
                if (obj["model"].toString() == "TPMS")
                {
                    // TODO?
                }
                else
                {
                    QSqlQuery addData;
                    addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, temperature1, temperature2, temperature3, temperature4, temperature5, temperature6)"
                                    " VALUES (:deviceAddr, :ts, :t1, :t2, :t3, :t4, :t5, :t6)");
                    addData.bindValue(":deviceAddr", getAddress());
                    addData.bindValue(":ts", m_lastUpdate.toString("yyyy-MM-dd hh:mm:ss"));
                    addData.bindValue(":t1", m_temperature1);
                    addData.bindValue(":t2", m_temperature2);
                    addData.bindValue(":t3", m_temperature3);
                    addData.bindValue(":t4", m_temperature4);
                    addData.bindValue(":t5", m_temperature5);
                    addData.bindValue(":t6", m_temperature6);

                    if (addData.exec())
                        m_lastUpdateDatabase = m_lastUpdate;
                    else
                        qWarning() << "> DeviceTheengsProbes TEMP addData.exec() ERROR"
                                   << addData.lastError().type() << ":" << addData.lastError().text();
                }
            }
        }

        refreshDataFinished(true);
    }
}

/* ************************************************************************** */

void DeviceTheengsProbes::getChartData_probeAIO(int maxDays, QDateTimeAxis *axis,
                                                QLineSeries *temp1, QLineSeries *temp2,
                                                QLineSeries *temp3, QLineSeries *temp4,
                                                QLineSeries *temp5, QLineSeries *temp6)
{
    if (m_dbInternal || m_dbExternal)
    {
        QString time = "datetime('now', 'localtime', '-" + QString::number(maxDays) + " days')";
        if (m_dbExternal) time = "DATE_SUB(NOW(), INTERVAL " + QString::number(maxDays) + " DAY)";

        QSqlQuery graphData;
        graphData.prepare("SELECT timestamp, temperature1, temperature2, temperature3, temperature4, temperature5, temperature6 " \
                          "FROM sensorTheengs " \
                          "WHERE deviceAddr = :deviceAddr AND timestamp >= " + time + ";");
        graphData.bindValue(":deviceAddr", getAddress());

        if (graphData.exec() == false)
        {
            qWarning() << "> graphData.exec() ERROR" << graphData.lastError().type() << ":" << graphData.lastError().text();
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
            temp1->append(timecode, graphData.value(1).toReal());
            temp2->append(timecode, graphData.value(2).toReal());
            temp3->append(timecode, graphData.value(3).toReal());
            temp4->append(timecode, graphData.value(4).toReal());
            temp5->append(timecode, graphData.value(5).toReal());
            temp6->append(timecode, graphData.value(6).toReal());

            // min/max
            for (int i = 1; i <= 6; i++)
            {
                if (graphData.value(i).toFloat() > -99 && graphData.value(i).toFloat() < m_tempMin) {
                    m_tempMin = graphData.value(i).toFloat();
                    minmaxChanged = true;
                }
                if (graphData.value(i).toFloat() > -99 && graphData.value(i).toFloat() > m_tempMax) {
                    m_tempMax = graphData.value(i).toFloat();
                    minmaxChanged = true;
                }
            }
        }

        if (minmaxChanged) { Q_EMIT minmaxUpdated(); }
    }
}

/* ************************************************************************** */
