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

        if (idx == 1) {
            m_pressure1 = pres;
            m_temperature1 = temp;
            m_battery1 = batt;
            m_alarm1 = alarm;
        }
        else if (idx == 2) {
            m_pressure2 = pres;
            m_temperature2 = temp;
            m_battery2 = batt;
            m_alarm2 = alarm;
        }
        else if (idx == 3) {
            m_pressure3 = pres;
            m_temperature3 = temp;
            m_battery3 = batt;
            m_alarm3 = alarm;
        }
        else if (idx == 4) {
            m_pressure4 = pres;
            m_temperature4 = temp;
            m_battery4 = batt;
            m_alarm4 = alarm;
        }
    }
    else //if (obj["model"].toString().contains("BBQ"))
    {
        if (obj.contains("tempc")) m_temperature1 = obj["tempc"].toDouble();
        if (obj.contains("tempc1")) m_temperature1 = obj["tempc1"].toDouble();
        if (obj.contains("tempc2")) m_temperature2 = obj["tempc2"].toDouble();
        if (obj.contains("tempc3")) m_temperature3 = obj["tempc3"].toDouble();
        if (obj.contains("tempc4")) m_temperature4 = obj["tempc4"].toDouble();
        if (obj.contains("tempc5")) m_temperature5 = obj["tempc5"].toDouble();
        if (obj.contains("tempc6")) m_temperature6 = obj["tempc6"].toDouble();
    }

    //if (x > -99)
    {
        m_lastUpdate = QDateTime::currentDateTime();
        refreshDataFinished(true);

        if (needsUpdateDb())
        {
            // TODO // UPDATE DB
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

// TODO db

/* ************************************************************************** */
