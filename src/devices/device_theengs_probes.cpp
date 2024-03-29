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

DeviceTheengsProbes::DeviceTheengsProbes(const QString &deviceAddr, const QString &deviceName,
                                         const QString &deviceModel, const QString &devicePropsJson,
                                         QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBE;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (m_deviceModel == "TPMS") {
        getSqlTpmsData(12*60);
    } else {
        if (hasSetting("preset")) m_preset = getSetting("preset").toString();
        if (hasSetting("interval")) m_realtime_window = getSetting("interval").toString().toInt();
        //getSqlProbeData(12*60); // disabled
    }
}

DeviceTheengsProbes::DeviceTheengsProbes(const QBluetoothDeviceInfo &d,
                                         const QString &deviceModel, const QString &devicePropsJson,
                                         QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBE;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (m_deviceModel == "TPMS") {
        getSqlTpmsData(12*60);
    } else {
        if (hasSetting("preset")) m_preset = getSetting("preset").toString();
        if (hasSetting("interval")) m_realtime_window = getSetting("interval").toString().toInt();
        //getSqlProbeData(12*60); // disabled
    }
}

DeviceTheengsProbes::~DeviceTheengsProbes()
{
    //
}

/* ************************************************************************** */

void DeviceTheengsProbes::setPreset(const QString &p)
{
    if (m_preset != p)
    {
        m_preset = p;
        setSetting("preset", p);
        Q_EMIT presetUpdated();
    }
}

void DeviceTheengsProbes::setRtWindow(const int w)
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

void DeviceTheengsProbes::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsProbes::parseTheengsProps()";
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
    if (prop.contains("count") && prop.contains("alarm"))
    {
        m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_PROBES_TPMS;
    }
    else
    {
        if (prop.contains("tempc")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
        if (prop.contains("tempc1")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
        if (prop.contains("tempc2")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;
        if (prop.contains("tempc3")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
        if (prop.contains("tempc4")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;
        if (prop.contains("tempc5")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_TEMPERATURE_5;
        if (prop.contains("tempc6")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_TEMPERATURE_6;
    }
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
    if (obj.contains("mac")) setAddressMAC(obj["mac"].toString());

    int tmps_idx = -1;

    if (obj["model"].toString() == "TPMS")
    {
        tmps_idx = obj["count"].toInt();
        float pres = obj["pres"].toDouble() * 1000.f;
        float temp = obj["tempc"].toDouble();
        int batt = obj["batt"].toInt();
        bool alarm = obj["alarm"].toBool();

        if (tmps_idx == 1)
        {
            m_pressure1 = pres;
            m_temperature1 = temp;
            m_battery1 = batt;
            m_alarm1 = alarm;
        }
        else if (tmps_idx == 2)
        {
            m_pressure2 = pres;
            m_temperature2 = temp;
            m_battery2 = batt;
            m_alarm2 = alarm;
        }
        else if (tmps_idx == 3)
        {
            m_pressure3 = pres;
            m_temperature3 = temp;
            m_battery3 = batt;
            m_alarm3 = alarm;
        }
        else if (tmps_idx == 4)
        {
            m_pressure4 = pres;
            m_temperature4 = temp;
            m_battery4 = batt;
            m_alarm4 = alarm;
        }
    }
    else // temperature probes
    {
        m_temperature1 = -99.f;
        m_temperature2 = -99.f;
        m_temperature3 = -99.f;
        m_temperature4 = -99.f;
        m_temperature5 = -99.f;
        m_temperature6 = -99.f;

        QDateTime ts = QDateTime::currentDateTime();

        if (obj.contains("tempc") || obj.contains("tempc1")) {
            if (obj.contains("tempc")) m_temperature1 = obj["tempc"].toDouble();
            if (obj.contains("tempc1")) m_temperature1 = obj["tempc1"].toDouble();

            if (m_capture_started) {
                m_rt_probe[0].push_back(std::make_pair(ts, m_temperature1));
                sanetizeRtCapture(0);
                Q_EMIT rtProbe1Updated();
            }
        }
        if (obj.contains("tempc2")) {
            m_temperature2 = obj["tempc2"].toDouble();
            if (m_capture_started) {
                m_rt_probe[1].push_back(std::make_pair(ts, m_temperature2));
                sanetizeRtCapture(1);
                Q_EMIT rtProbe2Updated();
            }
        }
        if (obj.contains("tempc3")) {
            m_temperature3 = obj["tempc3"].toDouble();
            if (m_capture_started) {
                m_rt_probe[2].push_back(std::make_pair(ts, m_temperature3));
                sanetizeRtCapture(2);
                Q_EMIT rtProbe3Updated();
            }
        }
        if (obj.contains("tempc4")) {
            m_temperature4 = obj["tempc4"].toDouble();
            if (m_capture_started) {
                m_rt_probe[3].push_back(std::make_pair(ts, m_temperature4));
                sanetizeRtCapture(3);
                Q_EMIT rtProbe4Updated();
            }
        }
        if (obj.contains("tempc5")) {
            m_temperature5 = obj["tempc5"].toDouble();
            if (m_capture_started) {
                m_rt_probe[4].push_back(std::make_pair(ts, m_temperature5));
                sanetizeRtCapture(4);
                Q_EMIT rtProbe5Updated();
            }
        }
        if (obj.contains("tempc6")) {
            m_temperature6 = obj["tempc6"].toDouble();
            if (m_capture_started) {
                m_rt_probe[5].push_back(std::make_pair(ts, m_temperature6));
                sanetizeRtCapture(5);
                Q_EMIT rtProbe6Updated();
            }
        }

        // Update RT graph?
        if (m_capture_started)
        {
            Q_EMIT rtGraphUpdated();
        }

        // Notification?
        if (m_capture_started)
        {
            NotificationManager *nm = NotificationManager::getInstance();
            TempPresetManager *tpm = TempPresetManager::getInstance();

            TempPreset *tp = nullptr;
            if (nm && tpm)
            {
                tp = tpm->getPreset(m_preset);
            }

            if (tp)
            {
                for (int i = 0; i < 6; i++)
                {
                    float temp = -99.f;
                    if (i == 0) temp = m_temperature1;
                    else if (i == 1) temp = m_temperature2;
                    else if (i == 2) temp = m_temperature3;
                    else if (i == 3) temp = m_temperature4;
                    else if (i == 4) temp = m_temperature5;
                    else if (i == 5) temp = m_temperature6;

                    if (temp > -80)
                    {
                        int capture_range_is = tp->getPresetRangeFromTemp(temp);

                        if (capture_range_is != m_capture_range_was[i])
                        {
                            QString title;
                            QString notif;

                            if (m_capture_range_was[i] == -3)
                            {
                                // init
                            }
                            else
                            {
                                QString namestr = m_deviceName;
                                if (!m_locationName.isEmpty()) namestr = m_locationName;

                                float min = tp->getPresetRangeTempMin_fromRangeIndex(capture_range_is);
                                float max = tp->getPresetRangeTempMax_fromRangeIndex(capture_range_is);
                                QString unitstr = "°C";
                                if (SettingsManager::getInstance()->getAppUnits() != 0)
                                {
                                    min = (1.8f * min) + 32.f;
                                    max = (1.8f * max) + 32.f;
                                    unitstr = "°F";
                                }

                                QString rangestr;
                                if (min > -80 && max > -80) rangestr =  QString::number(min) + unitstr + " - " + QString::number(max) + unitstr;
                                else if (min > -80 && max < -80) rangestr =  QString::number(min) + unitstr + " and up";

                                title = namestr + " probe #" + QString::number(i+1) + " alert";

                                if (capture_range_is == -2) // above last range
                                {
                                    notif = "Above " + tp->getName() + " last range " + rangestr +
                                            " (" + tp->getPresetRangeName_fromRangeIndex(capture_range_is) + ")";
                                }
                                else if (capture_range_is == -1) // below first range
                                {
                                    notif = "Below " + tp->getName() + " first range " + rangestr +
                                            " (" + tp->getPresetRangeName_fromRangeIndex(capture_range_is) + ")";
                                }
                                else // inside range #i
                                {
                                    notif = "Inside " + tp->getName() + " range " + rangestr +
                                            " (" + tp->getPresetRangeName_fromRangeIndex(capture_range_is) + ")";
                                }
                            }

                            if (!title.isEmpty() && !notif.isEmpty())
                            {
                                nm->setNotification(title, notif, 16);
                            }

                            m_capture_range_was[i] = capture_range_is;
                        }
                    }
                }
            }
        }
    }

    {
        m_lastUpdate = QDateTime::currentDateTime();

        if (needsUpdateDb())
        {
            if (m_dbInternal || m_dbExternal)
            {
                if (obj["model"].toString() == "TPMS")
                {
                    QSqlQuery addData;

                    if (tmps_idx == 1)
                    {
                        addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, temperature1, pressure1, battery1, alarm1)"
                                        " VALUES (:deviceAddr, :ts, :temp, :pres, :batt, :alrm)");
                        addData.bindValue(":temp", m_temperature1);
                        addData.bindValue(":pres", m_pressure1);
                        addData.bindValue(":batt", m_battery1);
                        addData.bindValue(":alrm", m_alarm1);
                    }
                    else if (tmps_idx == 2)
                    {
                        addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, temperature2, pressure2, battery2, alarm2)"
                                        " VALUES (:deviceAddr, :ts, :temp, :pres, :batt, :alrm)");
                        addData.bindValue(":temp", m_temperature2);
                        addData.bindValue(":pres", m_pressure2);
                        addData.bindValue(":batt", m_battery2);
                        addData.bindValue(":alrm", m_alarm2);
                    }
                    else if (tmps_idx == 3)
                    {
                        addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, temperature3, pressure3, battery3, alarm3)"
                                        " VALUES (:deviceAddr, :ts, :temp, :pres, :batt, :alrm)");
                        addData.bindValue(":temp", m_temperature3);
                        addData.bindValue(":pres", m_pressure3);
                        addData.bindValue(":batt", m_battery3);
                        addData.bindValue(":alrm", m_alarm3);
                    }
                    else if (tmps_idx == 4)
                    {
                        addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, temperature4, pressure4, battery4, alarm4)"
                                        " VALUES (:deviceAddr, :ts, :temp, :pres, :batt, :alrm)");
                        addData.bindValue(":temp", m_temperature4);
                        addData.bindValue(":pres", m_pressure4);
                        addData.bindValue(":batt", m_battery4);
                        addData.bindValue(":alrm", m_alarm4);
                    }

                    addData.bindValue(":deviceAddr", getAddress());
                    addData.bindValue(":ts", m_lastUpdate.toString("yyyy-MM-dd hh:mm:ss"));

                    if (addData.exec())
                        m_lastUpdateDatabase = m_lastUpdate;
                    else
                        qWarning() << "> DeviceTheengsProbes TPMS addData.exec() ERROR"
                                   << addData.lastError().type() << ":" << addData.lastError().text();
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

void DeviceTheengsProbes::startRtCapture(bool start)
{
    if (m_capture_started != start)
    {
        m_capture_started = start;

        if (m_capture_started) qDebug() << "DeviceTheengsProbes::startRtCapture()" << getAddress() << getName();
        else qDebug() << "DeviceTheengsProbes::stopRtCapture()" << getAddress() << getName();

        // init ranges
        for (int i = 0; i < 6; i++)
        {
            m_capture_range_was.push_back(-3);
        }
    }
}

void DeviceTheengsProbes::sanetizeRtCapture(int index)
{
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
    }
}

void DeviceTheengsProbes::getChartData_probeRT(QDateTimeAxis *axis,
                                               QLineSeries *temp1, QLineSeries *temp2,
                                               QLineSeries *temp3, QLineSeries *temp4,
                                               QLineSeries *temp5, QLineSeries *temp6,
                                               bool reload)
{
    //qDebug() << "DeviceTheengsProbes::getChartData_probeRT()" << getAddress() << getName();
    //qDebug() << "min " << QDateTime::currentDateTime().addSecs(-300).toString("hh:mm:ss");
    //qDebug() << "max " << QDateTime::currentDateTime().toString("hh:mm:ss");

    //if (!m_capture_started) startRtCapture(true);

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
