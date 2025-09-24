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

#include "BatteryPreset.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/* ************************************************************************** */
/* ************************************************************************** */

BatteryPreset::BatteryPreset(const int id, const int type, const bool ro,
                             const QString &name, const float vMin, const float vMax,
                             QObject *parent) : QObject(parent)
{
    m_id = id;
    m_readonly = ro;
    m_type = type;
    m_name = name;
    m_voltageMin = vMin;
    m_voltageMax = vMax;
}

BatteryPreset::~BatteryPreset()
{
    //
}

/* ************************************************************************** */

void BatteryPreset::setType(int t)
{
    if (m_type != t)
    {
        m_type = t;
        Q_EMIT presetChanged();
    }
}

void BatteryPreset::setName(const QString &n)
{
    if (m_name != n)
    {
        m_name = n;
        Q_EMIT presetChanged();
    }
}

/* ************************************************************************** */
