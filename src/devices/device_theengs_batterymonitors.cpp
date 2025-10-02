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

#include "device_theengs_batterymonitors.h"

#include <QBluetoothUuid>
#include <QBluetoothServiceInfo>
#include <QLowEnergyService>

#include <QSqlQuery>
#include <QSqlError>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QListIterator>
#include <QDebug>

/* ************************************************************************** */

DeviceTheengsBatteryMonitors::DeviceTheengsBatteryMonitors(const QString &deviceAddr,
                                                           const QString &deviceName,
                                                           const QString &deviceModel,
                                                           const QString &devicePropsJson,
                                                           QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_BATTERYMONITOR;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (m_deviceModel == "BM2" || m_deviceModel == "BM6")
    {
        if (hasSetting("preset")) m_preset = getSetting("preset").toString();
    }
}

DeviceTheengsBatteryMonitors::DeviceTheengsBatteryMonitors(const QBluetoothDeviceInfo &d,
                                                           const QString &deviceModel,
                                                           const QString &devicePropsJson,
                                                           QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_BATTERYMONITOR;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (m_deviceModel == "BM2" || m_deviceModel == "BM6")
    {
        if (hasSetting("preset")) m_preset = getSetting("preset").toString();
    }
}

DeviceTheengsBatteryMonitors::~DeviceTheengsBatteryMonitors()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::setPreset(const QString &p)
{
    if (m_preset != p)
    {
        m_preset = p;
        setSetting("preset", p);
        Q_EMIT presetUpdated();
    }
}

void DeviceTheengsBatteryMonitors::setRtWindow(const int w)
{
    if (m_realtime_window != w)
    {
        m_realtime_window = w;
        setSetting("interval", w);
        Q_EMIT rtWindowUpdated();
    }
}

bool DeviceTheengsBatteryMonitors::areValuesValid_percent(const int p) const
{
    return (p >= 0 && p <= 100);
}

bool DeviceTheengsBatteryMonitors::areValuesValid_voltage(const float v) const
{
    return (v > -20.f && v < 20.f);
}

bool DeviceTheengsBatteryMonitors::addDatabaseRecord_percent(const QDateTime &timestamp, const int p)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addDatabaseRecord_percent()" << p;
    bool status = false;

    if (m_dbInternal || m_dbExternal)
    {
        if (areValuesValid_percent(p))
        {
            // SQL date format YYYY-MM-DD HH:MM:SS
            // We only save one record every 20m

            // hijack battery1
            QSqlQuery addData;
            addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, battery1)"
                            " VALUES (:deviceAddr, :timestamp, :battery1)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
            addData.bindValue(":battery1", m_batteryPercent);
            status = addData.exec();

            if (status)
            {
                m_lastUpdateDatabase = timestamp;
            }
            else
            {
                qWarning() << "> DeviceTheengsBatteryMonitors addData.exec(v) ERROR"
                           << addData.lastError().type() << ":" << addData.lastError().text();
            }
        }
        else
        {
            qWarning() << "areValuesValid_percent(" << m_deviceName << p << ") values are INVALID";
        }
    }

    return status;
}

bool DeviceTheengsBatteryMonitors::addDatabaseRecord_voltage(const QDateTime &timestamp, const float v)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addDatabaseRecord_voltage()" << v;
    bool status = false;

    if (m_dbInternal || m_dbExternal)
    {
        if (areValuesValid_voltage(v))
        {
            // SQL date format YYYY-MM-DD HH:MM:SS
            // We only save one record every 20m

            // hijack battery2 // stored as int, so voltage*100
            int vvv = v*100.f;

            QSqlQuery addData;
            addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, battery2)"
                            " VALUES (:deviceAddr, :timestamp, :battery2)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
            addData.bindValue(":battery2", vvv);
            status = addData.exec();

            if (status)
            {
                m_lastUpdateDatabase = timestamp;
            }
            else
            {
                qWarning() << "> DeviceTheengsBatteryMonitors addData.exec(v) ERROR"
                           << addData.lastError().type() << ":" << addData.lastError().text();
            }
        }
        else
        {
            qWarning() << "areValuesValid_voltage(" << m_deviceName << v << ") values are INVALID";
        }
    }

    return status;
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::parseTheengsProps()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject prop = doc.object()["properties"].toObject();

    // JSON: "{\"properties\":{\"batt\":{\"unit\":\"%\",\"name\":\"battery\"},\"device\":{\"unit\":\"string\",\"name\":\"tracker device\"}}}"

    // MAC address
    if (prop.contains("mac")) m_deviceAddressMAC = prop["mac"].toString();

    // Capabilities
    //if (prop.contains("batt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    //if (prop.contains("volt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    //Q_EMIT capabilitiesUpdated();

    // Sensors
    //if (prop.contains("batt")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_BATTERY_PERCENT;
    Q_EMIT sensorsUpdated();
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    //if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setAddressMAC(obj["mac"].toString());

    // JSON: "{\"id\":\"61:57:43:01:5C:3D\",\"name\":\"BM2\",\"rssi\":0,\"brand\":\"GENERIC\",\"model\":\"BM2 Battery Monitor\",\"model_id\":\"BM2\",\"type\":\"BATT\",\"track\":true,\"batt\":68,\"device\":\"BM2 Tracker\"}"

    if (obj["model_id"].toString() == "BM2" || obj["model_id"].toString() == "BM6")
    {
        int battery = obj["batt"].toInt();
        //bool track = obj["track"].toBool(); // unused
        //QString device = obj["device"].toString(); // unused

        // valid data?
        if (areValuesValid_percent(battery))
        {
            m_lastUpdate = QDateTime::currentDateTime();

            // changes?
            if (m_batteryPercent != battery)
            {
                m_batteryPercent = battery;
                Q_EMIT dataUpdated();
            }
            refreshDataFinished(true);

            // rt data
            while (m_rt_batt.size() > 600) { m_rt_batt.pop_front(); } // sanetize
            m_rt_batt.push_back(std::make_pair(m_lastUpdate, m_batteryPercent));
            Q_EMIT rtGraphUpdated();

            // save?
            if (needsUpdateDb())
            {
                //addDatabaseRecord_percent(m_lastUpdate, m_batteryPercent);
            }
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::getChartData_batteryRT(QDateTimeAxis *axis, QLineSeries *batt, bool reload)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::getChartData_batteryRT()" << getAddress() << getName();
    //qDebug() << "min " << QDateTime::currentDateTime().addSecs(m_realtime_window * -60).toString("hh:mm:ss");
    //qDebug() << "max " << QDateTime::currentDateTime().toString("hh:mm:ss");

    //if (!m_capture_started) startRtCapture(true);
    if (!axis) return;
    if (!batt) return;

    int seconds = m_realtime_window * -60;
    axis->setFormat("hh:mm");
    axis->setMin(QDateTime::currentDateTime().addSecs(seconds));
    axis->setMax(QDateTime::currentDateTime());

    //
    if (!reload && m_rt_lastupdate.isValid() && m_rt_lastupdate.elapsed() < 500)
    {
        return;
    }
    else
    {
        m_rt_lastupdate.start();
    }

    //
    batt->clear();
    for (const auto &p: std::as_const(m_rt_batt))
    {
        if (p.first.secsTo(QDateTime::currentDateTime()) > -seconds) continue;
        batt->append(p.first.toMSecsSinceEpoch(), p.second);
    }
}

/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::getChartData_batteryHistory(QDateTimeAxis *axis, QLineSeries *batt, bool reload, int maxDays)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::getChartData_batteryHistory()" << getAddress() << getName();

    if (!axis) return;
    if (!batt) return;
    Q_UNUSED(reload)

    if (m_dbInternal || m_dbExternal)
    {
        QString time = "datetime('now', 'localtime', '-" + QString::number(maxDays) + " days')";
        if (m_dbExternal) time = "DATE_SUB(NOW(), INTERVAL " + QString::number(maxDays) + " DAY)";

        QSqlQuery graphData;
        graphData.prepare("SELECT timestamp, battery2 " \
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

            batt->append(timecode, graphData.value(1).toReal() / 100.f);

            // min/max // TODO?
        }

        if (minmaxChanged) { Q_EMIT minmaxUpdated(); }
    }
}

/* ************************************************************************** */
