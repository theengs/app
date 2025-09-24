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

#include "BatteryPresetManager.h"
#include "BatteryPreset.h"
#include "DatabaseManager.h"

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QDebug>

#include <QSqlQuery>
#include <QSqlError>

/* ************************************************************************** */

BatteryPresetManager *BatteryPresetManager::instance = nullptr;

BatteryPresetManager *BatteryPresetManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new BatteryPresetManager();
    }

    return instance;
}

BatteryPresetManager::BatteryPresetManager()
{
    load();
    filter("");

    qmlRegisterType<BatteryPreset>("BatteryPreset", 1, 0, "BatteryPreset");
}

BatteryPresetManager::~BatteryPresetManager()
{
    m_presetsFiltered.clear();

    qDeleteAll(m_presets);
    m_presets.clear();
}

/* ************************************************************************** */

bool BatteryPresetManager::load()
{
    bool status = true;

    // Load APP presets
    {
        BatteryPreset *b1 = new BatteryPreset(-1, BatteryPresetUtils::PRESET_LEADACID, true,
                                              "Lead Acid", 10, 12, this);
        m_presets.push_back(b1);

        BatteryPreset *b2 = new BatteryPreset(-2, BatteryPresetUtils::PRESET_AGM, true,
                                              "Absorbent Glass Mat", 10, 12, this);
        m_presets.push_back(b2);

        BatteryPreset *b3 = new BatteryPreset(-3, BatteryPresetUtils::PRESET_EFB, true,
                                              "Enhanced Flooded Battery", 10, 12, this);
        m_presets.push_back(b3);

        BatteryPreset *b4 = new BatteryPreset(-4, BatteryPresetUtils::PRESET_GEL, true,
                                              "Gel", 10, 12, this);
        m_presets.push_back(b4);

        BatteryPreset *b5 = new BatteryPreset(-5, BatteryPresetUtils::PRESET_LION, true,
                                              "Lithium Ion", 10, 12, this);
        m_presets.push_back(b5);
    }

    // USER presets?
    DatabaseManager *db = DatabaseManager::getInstance();
    if (db)
    {
        //m_dbInternal = db->hasDatabaseInternal();
        //m_dbExternal = db->hasDatabaseExternal();
    }

    // Load USER presets
    if (m_dbInternal || m_dbExternal)
    {
        QSqlQuery queryPresets;
        queryPresets.exec("SELECT id, type, name, vMin, vMax FROM batteryPresets");
        while (queryPresets.next())
        {
            int id = queryPresets.value(0).toInt();
            int type = queryPresets.value(1).toInt();
            QString name = queryPresets.value(2).toString();
            float vMin = queryPresets.value(3).toFloat();
            float vMax = queryPresets.value(4).toFloat();

            BatteryPreset *d = new BatteryPreset(id, type, false, name, vMin, vMax, this);
            if (d) m_presets.push_back(d);
        }
    }

    return status;
}

void BatteryPresetManager::filter(const QString &filter)
{
    //qDebug() << "BatteryPresetManager::filter()" << filter;

    m_presetsFiltered.clear();

    for (auto pp: std::as_const(m_presets))
    {
        BatteryPreset *bp = qobject_cast<BatteryPreset*>(pp);
        if (bp->getName().toLower().contains(filter.toLower()))
        {
            m_presetsFiltered.push_back(bp);
        }
    }

    Q_EMIT presetsFilteredChanged();
}

/* ************************************************************************** */

bool BatteryPresetManager::isPresetNameValid(const QString &name)
{
    bool status = false;

    if (!name.isEmpty())
    {
        status = true;

        for (auto pp: std::as_const(m_presets))
        {
            BatteryPreset *bp = qobject_cast<BatteryPreset*>(pp);
            if (bp && bp->getName() == name)
            {
                status = false;
            }
        }
    }

    return status;
}

bool BatteryPresetManager::addPreset(const int type, const QString &name)
{
    //qDebug() << "BatteryPresetManager::addPreset(" << type << name << ")";
/*
    BatteryPreset *newpreset = new BatteryPreset(0, type, false, name, "", this);
    if (newpreset)
    {
        newpreset->save();

        m_presets.push_back(newpreset);

        Q_EMIT presetsChanged();
        return true;
    }
*/
    return false;
}

bool BatteryPresetManager::copyPreset(const QString &name, const QString &newName)
{
    qDebug() << "BatteryPresetManager::copyPreset(" << name << newName << ")";
/*
    for (auto pp: std::as_const(m_presets))
    {
        BatteryPreset *bp = qobject_cast<BatteryPreset*>(pp);
        if (bp && bp->getName() == name)
        {
            BatteryPreset *newpreset = new BatteryPreset(*bp, newName, this);
            if (newpreset)
            {
                newpreset->save();
                newpreset->saveRanges();

                m_presets.push_back(newpreset);

                Q_EMIT presetsChanged();
                return true;
            }
        }
    }
*/
    return false;
}

bool BatteryPresetManager::removePreset(const QString &name)
{
    bool status = false;

    //qDebug() << "BatteryPresetManager::removePreset()" << name;
/*
    for (auto pp: std::as_const(m_presets))
    {
        BatteryPreset *bp = qobject_cast<BatteryPreset*>(pp);
        if (bp && !bp->getReadOnly() && bp->getName() == name)
        {
            // Remove from database
            if (m_dbInternal || m_dbExternal)
            {
                QSqlQuery removePreset;
                removePreset.prepare("DELETE FROM batteryPresets WHERE id = :id");
                removePreset.bindValue(":id", bp->getId());

                if (removePreset.exec() == false)
                {
                    qWarning() << "> removePreset.exec() ERROR"
                               << removePreset.lastError().type() << ":" << removePreset.lastError().text();
                }
            }

            // Remove preset
            m_presets.removeOne(bp);
            delete bp;

            Q_EMIT presetsChanged();
            status = true;

            break;
        }
    }
*/
    return status;
}

/* ************************************************************************** */

BatteryPreset *BatteryPresetManager::getPreset(const QString &name)
{
    BatteryPreset *p = nullptr;

    for (auto pp: std::as_const(m_presets))
    {
        BatteryPreset *bp = qobject_cast<BatteryPreset*>(pp);
        if (bp && bp->getName() == name)
        {
            return bp;
        }
    }

    return p;
}

/* ************************************************************************** */
