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

#include "TempPreset.h"

#include <QDebug>

/* ************************************************************************** */
/* ************************************************************************** */

TempRange::TempRange(const QString &name, const float tempMin, const float tempMax,
                     QObject *parent) : QObject(parent)
{
    m_name = name;
    //m_color = color;
    m_tempMin = tempMin;
    m_tempMax = tempMax;
}

/* ************************************************************************** */

void TempRange::setName(const QString &n)
{
    if (m_name != n)
    {
        m_name = n;
        Q_EMIT nameChanged();
    }
}

void TempRange::setColor(const QString &c)
{
    if (m_color != c)
    {
        m_color = c;
        Q_EMIT rangeChanged();
    }
}

void TempRange::setTempMin(float t)
{
    if (m_tempMin != t)
    {
        m_tempMin = t;
        Q_EMIT rangeChanged();
    }
}

void TempRange::setTempMax(float t)
{
    if (m_tempMax != t)
    {
        m_tempMax = t;
        Q_EMIT rangeChanged();
    }
}

void TempRange::setTempMaxDisabled(bool d)
{
    if (m_tempMax_disabled != d)
    {
        m_tempMax_disabled = d;
        Q_EMIT rangeChanged();
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

TempPreset::TempPreset(const int id, const int type, const bool ro,
                       const QString &name, const QString &data,
                       QObject *parent) : QObject(parent)
{
    m_id = id;
    m_readonly = ro;
    m_type = type;
    m_name = name;
    m_data = data;
}

TempPreset::~TempPreset()
{
    qDeleteAll(m_ranges);
    m_ranges.clear();
}

/* ************************************************************************** */

bool TempPreset::isRangeNameValid(const QString &name)
{
    bool status = false;

    if (!name.isEmpty())
    {
        status = true;

        for (auto rr: std::as_const(m_ranges))
        {
            TempRange *tr = qobject_cast<TempRange*>(rr);
            if (tr && tr->getName() == name)
            {
                status = false;
            }
        }
    }

    return status;
}

bool TempPreset::addRange(const QString &name, const bool before,
                          const float min, const float max)
{
    TempRange *r = new TempRange(name, min, max, this);
    if (r)
    {
        if (before)
        {
            m_ranges.push_front(r);
        }
        else // after
        {
            m_ranges.push_back(r);
        }

        Q_EMIT rangesChanged();
        return true;
    }

    return false;
}

bool TempPreset::removeRange(const QString &name)
{
    for (auto rr: std::as_const(m_ranges))
    {
        TempRange *tr = qobject_cast<TempRange*>(rr);
        if (tr && tr->getName() == name)
        {
            m_ranges.removeOne(tr);
            delete tr;

            Q_EMIT rangesChanged();
            return true;
        }
    }

    return false;
}

/* ************************************************************************** */

float TempPreset::getTempMin_add() const
{
    float min = 140;

    if (!m_ranges.isEmpty())
    {
        TempRange *tr = qobject_cast<TempRange*>(m_ranges.first());
        min = tr->getTempMin();
    }

    return min;
}

float TempPreset::getTempMax_add() const
{
    float max = 160;

    if (!m_ranges.isEmpty())
    {
        TempRange *tr = qobject_cast<TempRange*>(m_ranges.last());
        if (tr->getTempMax() <= 0 || tr->isTempMaxDisabled()) {
            max = tr->getTempMin();
        } else {
            max = tr->getTempMax();
        }
    }

    return max;
}

/* ************************************************************************** */

float TempPreset::getRangeMin() const
{
    float min = +999.f;

    for (auto rr: std::as_const(m_ranges))
    {
        TempRange *tr = qobject_cast<TempRange*>(rr);
        if (tr)
        {
            if (tr->getTempMin() < min) min = tr->getTempMin();
        }
    }

    return min;
}

float TempPreset::getRangeMax() const
{
    float max = -999.f;

    for (auto rr: std::as_const(m_ranges))
    {
        TempRange *tr = qobject_cast<TempRange*>(rr);
        if (tr)
        {
            if (tr->getTempMax() > max) max = tr->getTempMax();
        }
    }

    return max;
}

QString TempPreset::getRangeMinMax() const
{
    float min = +999.f;
    float max = -999.f;

    for (auto rr: std::as_const(m_ranges))
    {
        TempRange *tr = qobject_cast<TempRange*>(rr);
        if (tr)
        {
            if (tr->getTempMin() < min) min = tr->getTempMin();
            if (tr->getTempMax() > max) max = tr->getTempMax();
        }
    }

    QString mm = "(min: " + QString::number(min) + "°C" + "  /  " + "max: " + QString::number(max) + "°C)";

    return mm;
}

/* ************************************************************************** */

void TempPreset::setType(int t)
{
    if (m_type != t)
    {
        m_type = t;
        Q_EMIT presetChanged();
    }
}

void TempPreset::setName(const QString &n)
{
    if (m_name != n)
    {
        m_name = n;
        Q_EMIT presetChanged();
    }
}

/* ************************************************************************** */
/* ************************************************************************** */
