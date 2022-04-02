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

DeviceTheengsProbes::DeviceTheengsProbes(const QString &deviceAddr, const QString &deviceName, QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;

    if (deviceName.contains("iBBQ"))
    {
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;

        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;

        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_5;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_6;
    }
    else if (deviceName.contains("TPMS"))
    {
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_1;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_2;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_3;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_4;
    }
}

DeviceTheengsProbes::DeviceTheengsProbes(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceTheengs(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_PROBES;
    m_deviceCapabilities = DeviceUtils::DEVICE_BATTERY;

    if (d.name().contains("iBBQ"))
    {
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;

        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;

        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_5;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_6;
    }
    else if (d.name().contains("TPMS"))
    {
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_1;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_1;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_2;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_2;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_3;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_3;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_TEMPERATURE_4;
        m_deviceSensors += DeviceUtilsTheengs::SENSOR_PRESSURE_4;
    }
}

DeviceTheengsProbes::~DeviceTheengsProbes()
{
    //
}

/* ************************************************************************** */

void DeviceTheengsProbes::parseAdvertisementTheengs(const QString &json)
{
    qDebug() << "DeviceTheengsProbes::parseAdvertisementTheengs()";
    qDebug() << "DATA:" << json;

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
            m_alarm1 = alarm;
        }
        else if (idx == 2) {
            m_pressure2 = pres;
            m_temperature2 = temp;
            m_alarm2 = alarm;
        }
        else if (idx == 3) {
            m_pressure3 = pres;
            m_temperature3 = temp;
            m_alarm3 = alarm;
        }
        else if (idx == 4) {
            m_pressure4 = pres;
            m_temperature4 = temp;
            m_alarm4 = alarm;
        }
    }
    else if (obj["model"].toString().contains("BBQ"))
    {
        if (obj.contains("tempc")) m_temperature1 = obj["tempc"].toDouble();
        if (obj.contains("temp1c")) m_temperature1 = obj["temp1c"].toDouble();
        if (obj.contains("temp2c")) m_temperature2 = obj["temp2c"].toDouble();
        if (obj.contains("temp3c")) m_temperature3 = obj["temp3c"].toDouble();
        if (obj.contains("temp4c")) m_temperature4 = obj["temp4c"].toDouble();
        if (obj.contains("temp5c")) m_temperature5 = obj["temp5c"].toDouble();
        if (obj.contains("temp6c")) m_temperature6 = obj["temp6c"].toDouble();
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
