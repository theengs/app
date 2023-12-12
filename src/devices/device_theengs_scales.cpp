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

#include "device_theengs_scales.h"

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

DeviceTheengsScales::DeviceTheengsScales(const QString &deviceAddr, const QString &deviceName,
                                         const QString &deviceModel, const QString &devicePropsJson,
                                         QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_SCALE;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
    getSqlScaleData(12*60);
}

DeviceTheengsScales::DeviceTheengsScales(const QBluetoothDeviceInfo &d,
                                         const QString &deviceModel, const QString &devicePropsJson,
                                         QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_SCALE;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
    getSqlScaleData(12*60);
}

DeviceTheengsScales::~DeviceTheengsScales()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsScales::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsScales::parseTheengsProps()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject prop = doc.object()["properties"].toObject();

    // MAC address
    if (prop.contains("mac")) m_deviceAddressMAC = prop["mac"].toString();

    // Capabilities
    if (prop.contains("batt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    if (prop.contains("volt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    Q_EMIT capabilitiesUpdated();

    // Sensors
    if (prop.contains("weighing_mode")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_WEIGHT_MODE;
    if (prop.contains("unit")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_WEIGHT_UNIT;
    if (prop.contains("weight")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_WEIGHT;
    if (prop.contains("impedance")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_IMPEDANCE;
    Q_EMIT sensorsUpdated();
}

void DeviceTheengsScales::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsScales::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setAddressMAC(obj["mac"].toString());

    if (obj["weighing_mode"].isString()) {
        if (m_weightMode != obj["weighing_mode"].toString()) {
            m_weightMode = obj["weighing_mode"].toString();
            Q_EMIT dataUpdated();
        }
    }
    if (obj["weighing_unit"].isString()) {
        if (m_weightUnit != obj["weighing_unit"].toString()) {
            m_weightUnit = obj["weighing_unit"].toString();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("weight")) {
        if (m_weight != obj["weight"].toDouble()) {
            m_weight = obj["weight"].toDouble();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("impedance")) {
        if (m_impedance != obj["impedance"].toInt()) {
            m_impedance = obj["impedance"].toInt();
            Q_EMIT dataUpdated();
        }
    } else {
        if (m_impedance > -99) {
            m_impedance = -99;
            Q_EMIT dataUpdated();
        }
    }

    if (m_weight > -99 || m_impedance > -99)
    {
        m_lastUpdate = QDateTime::currentDateTime();

        if (needsUpdateDb() && m_weightMode == "person")
        {
            if (m_dbInternal || m_dbExternal)
            {
                // SQL date format YYYY-MM-DD HH:MM:SS

                QSqlQuery addData;
                addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, weight, impedance)"
                                " VALUES (:deviceAddr, :ts, :weight, :impedance)");
                addData.bindValue(":deviceAddr", getAddress());
                addData.bindValue(":ts", m_lastUpdate.toString("yyyy-MM-dd hh:mm:ss"));
                addData.bindValue(":weight", m_weight);
                addData.bindValue(":impedance", m_impedance);

                if (addData.exec())
                    m_lastUpdateDatabase = m_lastUpdate;
                else
                    qWarning() << "> DeviceTheengsScales addData.exec() ERROR"
                               << addData.lastError().type() << ":" << addData.lastError().text();
            }
        }

        refreshDataFinished(true);
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsScales::getChartData_scaleAIO(int maxDays, QDateTimeAxis *axis,
                                                QLineSeries *weight, QLineSeries *impedance)
{
    if (m_dbInternal || m_dbExternal)
    {
        QString time = "datetime('now', 'localtime', '-" + QString::number(maxDays) + " days')";
        if (m_dbExternal) time = "DATE_SUB(NOW(), INTERVAL " + QString::number(maxDays) + " DAY)";

        QSqlQuery graphData;
        graphData.prepare("SELECT timestamp, weight, impedance " \
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
            weight->append(timecode, graphData.value(1).toReal());
            impedance->append(timecode, graphData.value(2).toReal());

            // min/max
            if (graphData.value(1).toFloat() > -99 && graphData.value(1).toFloat() < m_weightMin) {
                m_weightMin = graphData.value(1).toFloat();
                minmaxChanged = true;
            }
            if (graphData.value(2).toInt() > -99 && graphData.value(2).toInt() < m_impedanceMin) {
                m_impedanceMin = graphData.value(2).toInt();
                minmaxChanged = true;
            }
            if (graphData.value(1).toFloat() > -99 && graphData.value(1).toFloat() > m_weightMax) {
                m_weightMax = graphData.value(1).toFloat();
                minmaxChanged = true;
            }
            if (graphData.value(2).toInt() > -99 && graphData.value(2).toInt() > m_impedanceMax) {
                m_impedanceMax = graphData.value(2).toInt();
                minmaxChanged = true;
            }
        }

        if (minmaxChanged) { Q_EMIT minmaxUpdated(); }
    }
}

/* ************************************************************************** */
