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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/* ************************************************************************** */
/* ************************************************************************** */

TempRange::TempRange(const QString &name, const float tempMin, const float tempMax,
                     const bool tempMaxEnabled, QObject *parent) : QObject(parent)
{
    m_name = name;
    //m_color = color;
    m_tempMin = tempMin;
    m_tempMax = tempMax;
    m_tempMax_enabled = tempMaxEnabled;
}

TempRange::TempRange(const TempRange &p, QObject *parent) : QObject(parent)
{
    m_name = p.getName();
    //m_color = p.getColor();
    m_tempMin = p.getTempMin();
    m_tempMax = p.getTempMax();
    m_tempMax_enabled = p.isTempMaxEnabled();
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

void TempRange::setTempMaxEnabled(bool d)
{
    if (m_tempMax_enabled != d)
    {
        m_tempMax_enabled = d;
        Q_EMIT rangeChanged();
    }
}

void TempRange::setTempMaxDisabled(bool d)
{
    if (m_tempMax_enabled != !d)
    {
        m_tempMax_enabled = !d;
        Q_EMIT rangeChanged();
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

TempPreset::TempPreset(const int id, const int type, const bool ro,
                       const QString &name, const QString &ranges,
                       QObject *parent) : QObject(parent)
{
    m_id = id;
    m_readonly = ro;
    m_type = type;
    m_name = name;

    loadRanges(ranges);

    if (!m_readonly)
    {
        connect(this, &TempPreset::presetChanged, this, &TempPreset::save);
        connect(this, &TempPreset::rangesChanged, this, &TempPreset::saveRanges);
    }
}

TempPreset::TempPreset(const TempPreset &p, const QString &name,
                       QObject *parent) : QObject(parent)
{
    m_id = 0; // need a new ID
    m_readonly = false; // copied preset are not read only
    m_type = p.getType();
    m_name = name; // need a new name

    for (auto r: p.getRangesInternal())
    {
        TempRange *newrange = new TempRange(*qobject_cast<TempRange*>(r), this);
        if (newrange)
        {
            connect(newrange, &TempRange::nameChanged, this, &TempPreset::saveRanges);
            connect(newrange, &TempRange::rangeChanged, this, &TempPreset::saveRanges);
            m_ranges.push_back(newrange);
        }
    }

    //
    connect(this, &TempPreset::presetChanged, this, &TempPreset::save);
    connect(this, &TempPreset::rangesChanged, this, &TempPreset::saveRanges);
}

TempPreset::~TempPreset()
{
    qDeleteAll(m_ranges);
    m_ranges.clear();
}

/* ************************************************************************** */

void TempPreset::loadRanges(const QString &json)
{
    //qDebug() << "TempPreset::loadRanges(" << json << ")";

    QJsonDocument presetDoc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject presetObj = presetDoc.object();

    QJsonArray rangeArray = presetObj["ranges"].toArray();
    for (const auto &value: rangeArray)
    {
        QJsonObject obj = value.toObject();

        QString name = obj["name"].toString();
        float min = obj["min"].toDouble();
        float max = obj["max"].toDouble();
        bool maxEnabled = obj["maxEnabled"].toBool();

        TempRange *r = new TempRange(name, min, max, maxEnabled, this);
        if (r)
        {
            connect(r, &TempRange::nameChanged, this, &TempPreset::saveRanges);
            connect(r, &TempRange::rangeChanged, this, &TempPreset::saveRanges);
            m_ranges.push_back(r);
        }
    }

    if (m_ranges.size()) Q_EMIT rangesChanged();
}

void TempPreset::save()
{
    //qDebug() << "TempPreset::save()";

    QSqlQuery savePreset;
    if (m_id == 0)
    {
        savePreset.prepare("INSERT INTO tempPresets (type, name) VALUES(:type, :name)");
        savePreset.bindValue(":type", m_type);
        savePreset.bindValue(":name", m_name);
    }
    else
    {
        savePreset.prepare("REPLACE INTO tempPresets (id, type, name) VALUES(:id, :type, :name)");
        savePreset.bindValue(":id", m_id);
        savePreset.bindValue(":type", m_type);
        savePreset.bindValue(":name", m_name);
    }

    if (savePreset.exec() == false)
    {
        qWarning() << "> savePreset.exec() ERROR"
                   << savePreset.lastError().type() << ":" << savePreset.lastError().text();
    }
    else
    {
        if (m_id == 0)
        {
            m_id = savePreset.lastInsertId().toInt();
        }
    }
}

void TempPreset::saveRanges()
{
    QJsonArray jsonArray;
    for (auto rr: std::as_const(m_ranges))
    {
        TempRange *tr = qobject_cast<TempRange*>(rr);
        if (tr)
        {
            QJsonObject jsonrange;
            jsonrange["name"] = tr->getName();
            jsonrange["min"] = tr->getTempMin();
            jsonrange["max"] = tr->getTempMax();
            jsonrange["maxEnabled"] = tr->isTempMaxEnabled();
            jsonArray.append(jsonrange);
        }
    }

    QJsonObject jsonObj;
    jsonObj["ranges"] = jsonArray;

    QSqlQuery saveRanges;
    saveRanges.prepare("UPDATE tempPresets SET ranges = :ranges WHERE name = :name");
    saveRanges.bindValue(":ranges", QString(QJsonDocument(jsonObj).toJson(QJsonDocument::Compact)));
    saveRanges.bindValue(":name", m_name);

    if (saveRanges.exec() == false)
    {
        qWarning() << "> saveRanges.exec() ERROR"
                   << saveRanges.lastError().type() << ":" << saveRanges.lastError().text();
    }
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
                          const float min, const float max, const bool maxEnabled)
{
    TempRange *r = new TempRange(name, min, max, maxEnabled, this);
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
    float min = 40;

    if (!m_ranges.isEmpty())
    {
        TempRange *tr = qobject_cast<TempRange*>(m_ranges.first());
        min = tr->getTempMin();
    }

    return min;
}

float TempPreset::getTempMax_add() const
{
    float max = 60;

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
