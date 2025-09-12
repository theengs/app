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

#include "device_theengs_windowactuators.h"

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

DeviceTheengsWindowActuators::DeviceTheengsWindowActuators(const QString &deviceAddr, const QString &deviceName,
                                                           const QString &deviceModel, const QString &devicePropsJson,
                                                           QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR_WINDOW;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsWindowActuators::DeviceTheengsWindowActuators(const QBluetoothDeviceInfo &d,
                                                           const QString &deviceModel, const QString &devicePropsJson,
                                                           QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR_WINDOW;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsWindowActuators::~DeviceTheengsWindowActuators()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsWindowActuators::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsWindowActuators::parseTheengsProps()";
    //qDebug() << "JSON:" << json;

    // JSON: "{\"properties\":{\"open\":{\"unit\":\"%\",\"name\":\"open\"},\"direction\":{\"unit\":\"string\",\"name\":\"direction\"},\"motion\":{\"unit\":\"status\",\"name\":\"motion\"},\"calibrated\":{\"unit\":\"status\",\"name\":\"calibrated\"},\"lightlevel\":{\"unit\":\"int\",\"name\":\"light level\"},\"batt\":{\"unit\":\"%\",\"name\":\"battery\"},\"mac\":{\"unit\":\"string\",\"name\":\"MAC address\"}}}"

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
    if (prop.contains("direction")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_DIRECTION;
    if (prop.contains("mode")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_MODE;
    if (prop.contains("state")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_STATE;
    //if (prop.contains("calibrated")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_;
    //if (prop.contains("lightlevel")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_;
    Q_EMIT sensorsUpdated();
}

/* ************************************************************************** */

void DeviceTheengsWindowActuators::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsWindowActuators::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setAddressMAC(obj["mac"].toString());

    // Curtain motor // moving, position, light level, battery, calibration state
    // JSON: "{\"id\":\"76:57:43:01:5C:02\",\"name\":\"Switchbot_Curtain 2\",\"rssi\":0,\"brand\":\"SwitchBot\",\"model\":\"Curtain (2/3)\",\"model_id\":\"W070160X\",\"type\":\"WCVR\",\"acts\":true,\"ctrl\":true,\"moving\":false,\"position\":100,\"calibrated\":true,\"lightlevel\":1,\"batt\":17}"
    // JSON: "{\"id\":\"76:57:43:01:5C:03\",\"name\":\"Switchbot_Curtain 3\",\"rssi\":0,\"brand\":\"SwitchBot\",\"model\":\"Curtain (2/3)\",\"model_id\":\"W070160X\",\"type\":\"WCVR\",\"acts\":true,\"ctrl\":true,\"moving\":false,\"position\":100,\"calibrated\":true,\"lightlevel\":1,\"batt\":79}"

    // Venetian blind tilting actor // open, direction, motion, calibrated, light level, battery
    // JSON: ?

    if (obj.contains("calibrated"))
    {
        if (m_calibrated != obj["calibrated"].toBool())
        {
            m_calibrated = obj["calibrated"].toBool();
            Q_EMIT calibratedUpdated();
        }
    }

    if (obj.contains("moving"))
    {
        if (m_moving != obj["moving"].toBool())
        {
            m_moving = obj["moving"].toBool();
            Q_EMIT movingUpdated();
        }
    }

    if (obj.contains("direction"))
    {
        if (m_direction != obj["direction"].toString())
        {
            m_direction = obj["direction"].toString();
            Q_EMIT directionUpdated();
        }
    }

    if (obj.contains("position"))
    {
        if (m_position != obj["position"].toInt())
        {
            m_position = obj["position"].toInt();
            Q_EMIT positionUpdated();
        }
    }

    if (obj.contains("open"))
    {
        if (m_open != obj["open"].toInt())
        {
            m_open = obj["open"].toInt();
            Q_EMIT openUpdated();
        }
    }

    if (obj.contains("lightlevel"))
    {
        if (m_lightlevel != obj["lightlevel"].toInt())
        {
            m_lightlevel = obj["lightlevel"].toInt();
            Q_EMIT lightlevelUpdated();
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
