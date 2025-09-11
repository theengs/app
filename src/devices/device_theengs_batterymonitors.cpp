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
#include "NotificationManager.h"
#include "SettingsManager.h"
#include "TempPresetManager.h"
#include "TempPreset.h"

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
        //
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
        //
    }
}

DeviceTheengsBatteryMonitors::~DeviceTheengsBatteryMonitors()
{
    //
}

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
/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::parseTheengsProps(const QString &json)
{
    qDebug() << "DeviceTheengsBatteryMonitors::parseTheengsProps()";
    qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject prop = doc.object()["properties"].toObject();

    // MAC address
    if (prop.contains("mac")) m_deviceAddressMAC = prop["mac"].toString();

    // Capabilities
    //if (prop.contains("batt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    //if (prop.contains("volt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    //Q_EMIT capabilitiesUpdated();

    // Sensors
    if (prop.contains("batt")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_BATTERY;
    Q_EMIT sensorsUpdated();
}

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
        m_battery1 = obj["batt"].toInt();
        //bool track = obj["track"].toBool();
        //QString device = obj["device"].toString();
    }

    {
        m_lastUpdate = QDateTime::currentDateTime();

        if (needsUpdateDb())
        {
            if (m_dbInternal || m_dbExternal)
            {
/*
                addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, battery1)"
                                " VALUES (:deviceAddr, :ts, :batt)");
                addData.bindValue(":batt", m_battery1);

                addData.bindValue(":deviceAddr", getAddress());
                addData.bindValue(":ts", m_lastUpdate.toString("yyyy-MM-dd hh:mm:ss"));

                if (addData.exec())
                    m_lastUpdateDatabase = m_lastUpdate;
                else
                    qWarning() << "> DeviceTheengsBatteryMonitors addData.exec() ERROR"
                               << addData.lastError().type() << ":" << addData.lastError().text();
*/
            }
        }

        refreshDataFinished(true);
    }
}

/* ************************************************************************** */

void DeviceTheengsBatteryMonitors::startRtCapture(bool start)
{
    if (m_capture_started != start)
    {
        m_capture_started = start;

        if (m_capture_started) qDebug() << "DeviceTheengsBatteryMonitors::startRtCapture()" << getAddress() << getName();
        else qDebug() << "DeviceTheengsBatteryMonitors::stopRtCapture()" << getAddress() << getName();

        // init ranges
        for (int i = 0; i < 6; i++)
        {
            m_capture_range_was.push_back(-3);
        }
    }
}

void DeviceTheengsBatteryMonitors::sanetizeRtCapture(int index)
{/*
    if (index < 0 || index > 5) return;
    if (m_rt_probe[index].size() < 600 &&
        m_rt_probe[index].first().first.secsTo(QDateTime::currentDateTime()) < 660) return;

    std::pair <QDateTime, float> cur = m_rt_probe[index].first();
    float curval = cur.second;
    int curcnt = 1;

    for (const auto &d: m_rt_probe[index])
    {
        // don't sanetize inside the last 10m window
        if (d.first.secsTo(QDateTime::currentDateTime()) < 600) break;

        if (cur.first.secsTo(d.first) < 60)
        {
            curval += d.second;
            curcnt++;
            m_rt_probe[index].removeFirst();
        }
        else
        {
            cur.second = curval / static_cast<float>(curcnt);
            m_rt_san_probe[index].push_back(cur);
            cur = d;
            curval = cur.second;
            curcnt = 1;
        }
    }

    if (curcnt > 1)
    {
        cur.second = curval / static_cast<float>(curcnt);
        m_rt_san_probe[index].push_back(cur);
    }*/
}

void DeviceTheengsBatteryMonitors::getChartData_probeRT(QDateTimeAxis *axis,
                                                        QLineSeries *batt,
                                                        bool reload)
{
    //qDebug() << "DeviceTheengsBatteryMonitors::getChartData_probeRT()" << getAddress() << getName();
    //qDebug() << "min " << QDateTime::currentDateTime().addSecs(-300).toString("hh:mm:ss");
    //qDebug() << "max " << QDateTime::currentDateTime().toString("hh:mm:ss");

    //if (!m_capture_started) startRtCapture(true);
/*
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
    int maxprobes = 0;
    QLineSeries *temp[6] = { nullptr };
    if (temp1 && hasTemp1()) { maxprobes++; temp[0] = temp1; temp1->clear(); }
    if (temp2 && hasTemp2()) { maxprobes++; temp[1] = temp2; temp2->clear(); }
    if (temp3 && hasTemp3()) { maxprobes++; temp[2] = temp3; temp3->clear(); }
    if (temp4 && hasTemp4()) { maxprobes++; temp[3] = temp4; temp4->clear(); }
    if (temp5 && hasTemp5()) { maxprobes++; temp[4] = temp5; temp5->clear(); }
    if (temp6 && hasTemp6()) { maxprobes++; temp[5] = temp6; temp6->clear(); }

    for (int i = 0; i < maxprobes; i++)
    {
        for (const auto &p: m_rt_san_probe[i]) {
            if (p.first.secsTo(QDateTime::currentDateTime()) > -seconds) continue;
            temp[i]->append(p.first.toMSecsSinceEpoch(), p.second);
        }
        for (const auto &p: m_rt_probe[i]) {
            if (p.first.secsTo(QDateTime::currentDateTime()) > -seconds) continue;
            temp[i]->append(p.first.toMSecsSinceEpoch(), p.second);
        }
    }
*/
}

/* ************************************************************************** */
