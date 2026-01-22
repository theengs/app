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

#include <QDateTime>
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

/* ************************************************************************** */

bool DeviceTheengsBatteryMonitors::areValuesValid_voltagepercent(const float v, const int p) const
{
    return areValuesValid_voltage(v) && areValuesValid_percent(p);
}

bool DeviceTheengsBatteryMonitors::areValuesValid_voltage(const float v) const
{
    return (v > -20.f && v < 20.f);
}

bool DeviceTheengsBatteryMonitors::areValuesValid_percent(const int p) const
{
    return (p >= 0 && p <= 100);
}

/* ************************************************************************** */

bool DeviceTheengsBatteryMonitors::addDatabaseRecord_voltagepercent(const int64_t timestamp, const float v, const int p)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addDatabaseRecord_voltagepercent()" << v << p;
    bool status = false;

    if (m_dbInternal || m_dbExternal)
    {
        if (areValuesValid_voltagepercent(v, p))
        {
            // SQL date format YYYY-MM-DD HH:MM:SS

            // We only save one record every 20 minutes
            int round_seconds = 20*60;
            QDateTime tmcd_rounded = QDateTime::fromSecsSinceEpoch(timestamp + (round_seconds - timestamp % round_seconds) - round_seconds);
            QDateTime tmcd = QDateTime::fromSecsSinceEpoch(timestamp);

            // hijack battery2 // stored as int, so voltage*100
            int vvv = v*100.f;

            QSqlQuery addData;
            addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp_rounded, timestamp, battery1, battery2)"
                            " VALUES (:deviceAddr, :timestamp_rounded, :timestamp, :battery1, :battery2)");
            addData.bindValue(":deviceAddr", getAddress());
            addData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
            addData.bindValue(":timestamp", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
            addData.bindValue(":battery1", p); // hijack battery1
            addData.bindValue(":battery2", vvv); // hijack battery2

            if (addData.exec())
            {
                status = true;
                m_lastUpdateDatabase = tmcd;
            }
            else
            {
                qWarning() << "> DeviceTheengsBatteryMonitors addData.exec(vp) ERROR"
                           << addData.lastError().type() << ":" << addData.lastError().text();
            }
        }
        else
        {
            qWarning() << "areValuesValid_voltagepercent(" << m_deviceName << v << p << ") values are INVALID";
        }
    }

    return status;
}

bool DeviceTheengsBatteryMonitors::addDatabaseRecord_voltage(const int64_t timestamp, const float v)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addDatabaseRecord_voltage()" << v;
    bool status = false;

    if (m_dbInternal || m_dbExternal)
    {
        if (areValuesValid_voltage(v))
        {
            // SQL date format YYYY-MM-DD HH:MM:SS

            // We only save one record every 20 minutes
            int round_seconds = 20*60;
            QDateTime tmcd_rounded = QDateTime::fromSecsSinceEpoch(timestamp + (round_seconds - timestamp % round_seconds) - round_seconds);
            QDateTime tmcd = QDateTime::fromSecsSinceEpoch(timestamp);

            // hijack battery2 // stored as int, so voltage*100
            int vvv = v*100.f;

            QSqlQuery ifData;
            ifData.prepare("SELECT 1 FROM sensorTheengs"
                           " WHERE deviceAddr = :deviceAddr AND timestamp_rounded = :timestamp_rounded"
                           " LIMIT 1;");
            ifData.bindValue(":deviceAddr", getAddress());
            ifData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
            if (ifData.exec())
            {
                if (ifData.first())
                {
                    // We have a record, update data
                    QSqlQuery updateData;
                    updateData.prepare("UPDATE sensorTheengs SET battery2 = :battery2"
                                       " WHERE deviceAddr = :deviceAddr AND timestamp_rounded = :timestamp_rounded");
                    updateData.bindValue(":battery2", vvv); // hijack battery2
                    updateData.bindValue(":deviceAddr", getAddress());
                    updateData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));

                    if (updateData.exec())
                    {
                        status = true;
                        m_lastUpdateDatabase = tmcd;
                    }
                    else
                    {
                        qWarning() << "> DeviceTheengsBatteryMonitors updateData.exec(v) ERROR"
                                   << updateData.lastError().type() << ":" << updateData.lastError().text();
                    }
                }
                else
                {
                    // We DON'T have a record, add data
                    QSqlQuery addData;
                    addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp_rounded, timestamp, battery2)"
                                    " VALUES (:deviceAddr, :timestamp_rounded, :timestamp, :battery2)");
                    addData.bindValue(":deviceAddr", getAddress());
                    addData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
                    addData.bindValue(":timestamp", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
                    addData.bindValue(":battery2", vvv); // hijack battery2

                    if (addData.exec())
                    {
                        status = true;
                        m_lastUpdateDatabase = tmcd;
                    }
                    else
                    {
                        qWarning() << "> DeviceTheengsBatteryMonitors addData.exec(v) ERROR"
                                   << addData.lastError().type() << ":" << addData.lastError().text();
                    }
                }
            }
            else
            {
                qWarning() << "> DeviceTheengsBatteryMonitors ifData.exec(v) ERROR"
                           << ifData.lastError().type() << ":" << ifData.lastError().text();
            }
        }
        else
        {
            qWarning() << "areValuesValid_voltage(" << m_deviceName << v << ") values are INVALID";
        }
    }

    return status;
}

bool DeviceTheengsBatteryMonitors::addDatabaseRecord_percent(const int64_t timestamp, const int p)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addDatabaseRecord_percent()" << p;
    bool status = false;

    if (m_dbInternal || m_dbExternal)
    {
        if (areValuesValid_percent(p))
        {
            // SQL date format YYYY-MM-DD HH:MM:SS

            // We only save one record every 20 minutes
            int round_seconds = 20*60;
            QDateTime tmcd_rounded = QDateTime::fromSecsSinceEpoch(timestamp + (round_seconds - timestamp % round_seconds) - round_seconds);
            QDateTime tmcd = QDateTime::fromSecsSinceEpoch(timestamp);

            QSqlQuery ifData;
            ifData.prepare("SELECT 1 FROM sensorTheengs"
                           " WHERE deviceAddr = :deviceAddr AND timestamp_rounded = :timestamp_rounded"
                           " LIMIT 1;");
            ifData.bindValue(":deviceAddr", getAddress());
            ifData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
            if (ifData.exec())
            {
                if (ifData.first())
                {
                    // We have a record, update data
                    QSqlQuery updateData;
                    updateData.prepare("UPDATE sensorTheengs SET battery1 = :battery1"
                                       " WHERE deviceAddr = :deviceAddr AND timestamp_rounded = :timestamp_rounded");
                    updateData.bindValue(":battery1", p); // hijack battery2
                    updateData.bindValue(":deviceAddr", getAddress());
                    updateData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));

                    if (updateData.exec())
                    {
                        status = true;
                        m_lastUpdateDatabase = tmcd;
                    }
                    else
                    {
                        qWarning() << "> DeviceTheengsBatteryMonitors updateData.exec(v) ERROR"
                                   << updateData.lastError().type() << ":" << updateData.lastError().text();
                    }
                }
                else
                {
                    // We DON'T have a record, add data
                    QSqlQuery addData;
                    addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp_rounded, timestamp, battery1)"
                                    " VALUES (:deviceAddr, :timestamp_rounded, :timestamp, :battery1)");
                    addData.bindValue(":deviceAddr", getAddress());
                    addData.bindValue(":timestamp_rounded", tmcd_rounded.toString("yyyy-MM-dd hh:mm:00"));
                    addData.bindValue(":timestamp", tmcd.toString("yyyy-MM-dd hh:mm:ss"));
                    addData.bindValue(":battery1", m_batteryPercent); // hijack battery1

                    if (addData.exec())
                    {
                        status = true;
                        m_lastUpdateDatabase = tmcd;
                    }
                    else
                    {
                        qWarning() << "> DeviceTheengsBatteryMonitors addData.exec(p) ERROR"
                                   << addData.lastError().type() << ":" << addData.lastError().text();
                    }
                }
            }
            else
            {
                qWarning() << "> DeviceTheengsBatteryMonitors ifData.exec(p) ERROR"
                           << ifData.lastError().type() << ":" << ifData.lastError().text();
            }
        }
        else
        {
            qWarning() << "areValuesValid_percent(" << m_deviceName << p << ") values are INVALID";
        }
    }

    return status;
}

/* ************************************************************************** */

bool DeviceTheengsBatteryMonitors::addRealtimeRecord_voltagepercent(const QDateTime &timestamp, const float v, const int p)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addRealtimeRecord_voltagepercent()" << v << p;
    return addRealtimeRecord_voltage(timestamp, v) && addRealtimeRecord_percent(timestamp, p);
}

bool DeviceTheengsBatteryMonitors::addRealtimeRecord_voltage(const QDateTime &timestamp, const float v)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addRealtimeRecord_voltage()" << v;
    bool status = true;

    if (areValuesValid_voltage(v))
    {
        // rt data
        while (m_rt_batt_voltage.size() > 600) { m_rt_batt_voltage.pop_front(); } // sanetize
        m_rt_batt_voltage.push_back(std::make_pair(timestamp, v));
        Q_EMIT rtGraphUpdated();
    }
    else
    {
        qWarning() << "areValuesValid_voltage(" << m_deviceName << v << ") values are INVALID";
        status = false;
    }

    return status;
}

bool DeviceTheengsBatteryMonitors::addRealtimeRecord_percent(const QDateTime &timestamp, const int p)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::addRealtimeRecord_percent()" << p;
    bool status = true;

    if (areValuesValid_percent(p))
    {
        // rt data
        while (m_rt_batt_percent.size() > 600) { m_rt_batt_percent.pop_front(); } // sanetize
        m_rt_batt_percent.push_back(std::make_pair(timestamp, p));
        Q_EMIT rtGraphUpdated();
    }
    else
    {
        qWarning() << "areValuesValid_percent(" << m_deviceName << p << ") values are INVALID";
        status = false;
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

            // rt data
            addRealtimeRecord_percent(m_lastUpdate, battery);

            // save in db?
            if (needsUpdateDb())
            {
                addDatabaseRecord_percent(m_lastUpdate.toSecsSinceEpoch(), battery);
            }

            refreshDataFinished(true);
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::getChartData_batteryRT(QDateTimeAxis *axis,
                                                          QLineSeries *batt_p, QLineSeries *batt_v,
                                                          bool reload)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::getChartData_batteryRT()" << getAddress() << getName();

    if (!axis) return;

    QDateTime min = QDateTime::currentDateTime().addSecs(m_realtime_window * -60);
    QDateTime min_actual = min;
    QDateTime max = QDateTime::currentDateTime();
    int realtime_window_actual = m_realtime_window;

    if (m_rt_batt_voltage.size())
    {
        min_actual = m_rt_batt_voltage.first().first;

        realtime_window_actual = (max.toSecsSinceEpoch() - min_actual.toSecsSinceEpoch()) /60;
        if (realtime_window_actual < 5) realtime_window_actual = 5;
        if (realtime_window_actual > m_realtime_window) realtime_window_actual = m_realtime_window;
        min_actual = QDateTime::currentDateTime().addSecs(realtime_window_actual * -60);;
    }

    int seconds = realtime_window_actual * -60;

    //qDebug() << "min " << min.toString("hh:mm:ss");
    //qDebug() << "min_actual " << min_actual.toString("hh:mm:ss");
    //qDebug() << "max " << max.toString("hh:mm:ss");

    //qDebug() << "realtime_window " << m_realtime_window;
    //qDebug() << "realtime_window_actual " << realtime_window_actual;
    //qDebug() << "seconds " << seconds;

    axis->setFormat("hh:mm");
    axis->setMin(min_actual);
    axis->setMax(max);

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
    if (batt_p)
    {
        batt_p->clear();
        for (const auto &p: std::as_const(m_rt_batt_percent))
        {
            if (p.first.secsTo(QDateTime::currentDateTime()) > -seconds) continue;
            batt_p->append(p.first.toMSecsSinceEpoch(), p.second);
        }
    }

    //
    if (batt_v)
    {
        batt_v->clear();
        for (const auto &v: std::as_const(m_rt_batt_voltage))
        {
            if (v.first.secsTo(QDateTime::currentDateTime()) > -seconds) continue;
            batt_v->append(v.first.toMSecsSinceEpoch(), v.second);
        }
    }
}

/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::getChartData_batteryHistory(QDateTimeAxis *axis,
                                                               QLineSeries *batt_p, QLineSeries *batt_v,
                                                               bool reload, int maxDays)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::getChartData_batteryHistory()" << getAddress() << getName() << maxDays;

    if (!axis) return;
    if (batt_p) batt_p->clear();
    if (batt_v) batt_v->clear();
    Q_UNUSED(reload)

    if (m_dbInternal || m_dbExternal)
    {
        QString time = "datetime('now', 'localtime', '-" + QString::number(maxDays) + " days')";
        if (m_dbExternal) time = "DATE_SUB(NOW(), INTERVAL " + QString::number(maxDays) + " DAY)";

        QSqlQuery graphData;
        graphData.prepare("SELECT timestamp, battery1, battery2 " \
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
            qint64 timecode = date.toMSecsSinceEpoch();

            if (!minSet)
            {
                axis->setMin(date);
                minSet = true;
            }

            if (batt_p)
            {
                float p = graphData.value(1).toReal();
                if (p > 0.f)
                {
                    batt_p->append(timecode, p);
                }
            }
            if (batt_v)
            {
                float v = graphData.value(2).toReal() / 100.f;
                if (v > 0.f)
                {
                    batt_v->append(timecode, v);
                }
            }

            // min/max // TODO?
        }

        if (minmaxChanged) { Q_EMIT minmaxUpdated(); }
    }
}

/* ************************************************************************** */
