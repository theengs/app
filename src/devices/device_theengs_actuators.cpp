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

#include "device_theengs_actuators.h"

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

DeviceTheengsActuators::DeviceTheengsActuators(const QString &deviceAddr, const QString &deviceName,
                                               const QString &deviceModel, const QString &devicePropsJson,
                                               QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsActuators::DeviceTheengsActuators(const QBluetoothDeviceInfo &d,
                                               const QString &deviceModel, const QString &devicePropsJson,
                                               QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_ACTUATOR;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsActuators::~DeviceTheengsActuators()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsActuators::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsActuators::parseTheengsProps()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject prop = doc.object()["properties"].toObject();

    // JSON: "{\"id\":\"76:57:43:01:5C:01\",\"name\":\"Switchbot_S1\",\"rssi\":0,\"brand\":\"SwitchBot\",\"model\":\"Bot\",\"model_id\":\"X1\",\"type\":\"ACTR\",\"acts\":true,\"ctrl\":true,\"mode\":\"on/off\",\"state\":\"off\",\"batt\":91}"

    // MAC address
    if (prop.contains("mac")) m_deviceAddressMAC = prop["mac"].toString();

    // Capabilities
    if (prop.contains("batt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    if (prop.contains("volt")) m_deviceCapabilities |= DeviceUtils::DEVICE_BATTERY;
    Q_EMIT capabilitiesUpdated();

    // Sensors
    if (prop.contains("mode")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_MODE;
    if (prop.contains("state")) m_deviceSensorsTheengs |= DeviceUtilsTheengs::SENSOR_STATE;
    Q_EMIT sensorsUpdated();
}

/* ************************************************************************** */

void DeviceTheengsActuators::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsActuators::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setAddressMAC(obj["mac"].toString());

    // Switch Button Pusher // mode, state, battery
    // JSON: "{\"id\":\"76:57:43:01:5C:01\",\"name\":\"Switchbot_S1\",\"rssi\":0,\"brand\":\"SwitchBot\",\"model\":\"Bot\",\"model_id\":\"X1\",\"type\":\"ACTR\",\"acts\":true,\"ctrl\":true,\"mode\":\"on/off\",\"state\":\"off\",\"batt\":91}"

    if (obj.contains("mode"))
    {
        if (m_mode != obj["mode"].toString())
        {
            m_mode = obj["mode"].toString();
            Q_EMIT modeUpdated();
        }
    }

    if (obj.contains("state"))
    {
        QString state_str;
        bool state;
        if (state_str != obj["state"].toString())
        {
            state_str = obj["state"].toString();
        }

        if (m_mode == "on/off")
        {
            if (state_str == "on") {
                state = true;
            } else {
                state = false;
            }

            if (m_state != state)
            {
                m_state = state;
                Q_EMIT stateUpdated();
            }
        } else
        {
            qWarning() << "DeviceTheengsActuators unsupported mode:" << m_mode;
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
