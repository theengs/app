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

#include "device_theengs_motionsensors.h"

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

DeviceTheengsMotionSensors::DeviceTheengsMotionSensors(const QString &deviceAddr, const QString &deviceName,
                                                       const QString &deviceModel, const QString &devicePropsJson,
                                                       QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_MOTIONSENSOR;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsMotionSensors::DeviceTheengsMotionSensors(const QBluetoothDeviceInfo &d,
                                                       const QString &deviceModel, const QString &devicePropsJson,
                                                       QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_MOTIONSENSOR;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsMotionSensors::~DeviceTheengsMotionSensors()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsMotionSensors::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsMotions::parseTheengsProps()";
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
    if (prop.contains("open")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_OPEN;
    if (prop.contains("motion")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_MOTION;
    if (prop.contains("sensingdistance")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_DISTANCE;
    if (prop.contains("lightlevel")) m_deviceSensors |= DeviceUtils::SENSOR_LUMINOSITY;
    if (prop.contains("darkness")) m_deviceSensors |= DeviceUtils::SENSOR_LUMINOSITY;
    if (prop.contains("lux")) m_deviceSensors |= DeviceUtils::SENSOR_LUMINOSITY;
    Q_EMIT sensorsUpdated();
}

/* ************************************************************************** */

void DeviceTheengsMotionSensors::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsMotions::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setAddressMAC(obj["mac"].toString());

    if (obj.contains("open")) {
        if (m_open != obj["open"].toBool()) {
            m_open = obj["open"].toBool();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("motion")) {
        if (m_motion != obj["motion"].toBool()) {
            m_motion = obj["motion"].toBool();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("sensingdistance")) {
        if (m_sensing_distance != obj["sensingdistance"].toDouble()) {
            m_sensing_distance = obj["sensingdistance"].toDouble();
            Q_EMIT dataUpdated();
        }
    }

    if (obj.contains("lightlevel")) {
        if (m_luminosityLux != obj["lightlevel"].toInt()) {
            m_luminosityLux = obj["lightlevel"].toInt();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("darkness")) {
        if (m_luminosityLux != obj["darkness"].toInt()) {
            m_luminosityLux = obj["darkness"].toInt();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("lux")) {
        if (m_luminosityLux != obj["lux"].toInt()) {
            m_luminosityLux = obj["lux"].toInt();
            Q_EMIT dataUpdated();
        }
    }

    {
        m_lastUpdate = QDateTime::currentDateTime();

        //if (needsUpdateDb()) // always on for theengs advertising
        {
            // TODO
        }

        refreshDataFinished(true);
    }
}

/* ************************************************************************** */
