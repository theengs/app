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

#include "device_theengs_beacons.h"

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

DeviceTheengsBeacons::DeviceTheengsBeacons(const QString &deviceAddr, const QString &deviceName,
                                           const QString &deviceModel, const QString &devicePropsJson,
                                           QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_BEACON;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsBeacons::DeviceTheengsBeacons(const QBluetoothDeviceInfo &d,
                                           const QString &deviceModel, const QString &devicePropsJson,
                                           QObject *parent):
    DeviceTheengs(d, deviceModel, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_BEACON;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);
}

DeviceTheengsBeacons::~DeviceTheengsBeacons()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBeacons::parseTheengsProps(const QString &json)
{
    //qDebug() << "DeviceTheengsBeacons::parseTheengsProps()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject prop = doc.object()["properties"].toObject();

    // Capabilities
    if (prop.contains("batt")) m_deviceCapabilities += DeviceUtils::DEVICE_BATTERY;
    Q_EMIT capabilitiesUpdated();

    // Sensors
    Q_EMIT sensorsUpdated();
}

/* ************************************************************************** */

void DeviceTheengsBeacons::parseTheengsAdvertisement(const QString &json)
{
    //qDebug() << "DeviceTheengsBeacons::parseTheengsAdvertisement()";
    //qDebug() << "JSON:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setSetting("mac", obj["mac"].toString());

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
/* ************************************************************************** */

// TODO db

/* ************************************************************************** */
