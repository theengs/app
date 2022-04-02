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

#include "device_theengs_scales.h"

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

DeviceTheengsScales::DeviceTheengsScales(const QString &deviceAddr, const QString &deviceName, QObject *parent):
    DeviceTheengs(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_SCALE;
}

DeviceTheengsScales::DeviceTheengsScales(const QBluetoothDeviceInfo &d, QObject *parent):
    DeviceTheengs(d, parent)
{
    m_deviceType = DeviceUtils::DEVICE_SCALE;
}

DeviceTheengsScales::~DeviceTheengsScales()
{
    if (m_bleController) m_bleController->disconnectFromDevice();
}

/* ************************************************************************** */

void DeviceTheengsScales::parseAdvertisementTheengs(const QString &json)
{
    qDebug() << "DeviceTheengsScales::parseAdvertisementTheengs()";
    qDebug() << "DATA:" << json;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("batt")) setBattery(obj["batt"].toInt());
    if (obj.contains("mac")) setSetting("mac", obj["mac"].toString());

    if (obj["weighing_mode"].isString()) {
        if (m_weightMode != obj["weighing_mode"].toString()) {
            m_weightMode = obj["weighing_mode"].toString();
            Q_EMIT dataUpdated();
        }
    }
    if (obj["weighing_unit"].isString()) {
        if (m_weightUnit != obj["weighing_unit"].toString()) {
            m_weightUnit = obj["weighing_unit"].toString();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("weight")) {
        if (m_weight != obj["weight"].toDouble()) {
            m_weight = obj["weight"].toDouble();
            Q_EMIT dataUpdated();
        }
    }
    if (obj.contains("impedance")) {
        if (m_impedance != obj["impedance"].toInt()) {
            m_impedance = obj["impedance"].toInt();
            Q_EMIT dataUpdated();
        }
    }

    if (m_weight > -99)
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
