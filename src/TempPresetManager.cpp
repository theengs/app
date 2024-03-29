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

#include "TempPresetManager.h"
#include "TempPreset.h"
#include "DatabaseManager.h"

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QDebug>

#include <QSqlQuery>
#include <QSqlError>

/* ************************************************************************** */

TempPresetManager *TempPresetManager::instance = nullptr;

TempPresetManager *TempPresetManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new TempPresetManager();
    }

    return instance;
}

TempPresetManager::TempPresetManager()
{
    load();
    filter("");

    qmlRegisterType<TempPreset>("TempPreset", 1, 0, "TempPreset");
}

TempPresetManager::~TempPresetManager()
{
    m_presetsFiltered.clear();

    qDeleteAll(m_presets);
    m_presets.clear();
}

/* ************************************************************************** */

bool TempPresetManager::load()
{
    bool status = true;

    // Load APP presets
    {
        TempPreset *t1 = new TempPreset(-1, PresetUtils::PRESET_BEEF, true, "Beef", "", this);
        t1->addRange("Rare", false, 49, 54, true);
        t1->addRange("Medium Rare", false, 54, 60, true);
        t1->addRange("Safe internal temperature", false, 63, 63, true);
        t1->addRange("Medium", false, 60, 66, true);
        t1->addRange("Medium Well", false, 66, 71, true);
        t1->addRange("Well Done", false, 71, -1, false);
        m_presets.push_back(t1);

        TempPreset *t2 = new TempPreset(-2, PresetUtils::PRESET_PORK, true, "Pork", "", this);
        t2->addRange("Safe internal temperature", false, 63, 70, true);
        t2->addRange("Ground Pork", false, 71, -1, false);
        m_presets.push_back(t2);

        TempPreset *t3 = new TempPreset(-3, PresetUtils::PRESET_CHICKEN, true, "Chicken", "", this);
        t3->addRange("Safe internal temperature", false, 74, -1, false);
        m_presets.push_back(t3);

        TempPreset *t4 = new TempPreset(-4, PresetUtils::PRESET_FISH, true, "Fish", "", this);
        t4->addRange("Safe internal temperature", false, 63, -1, false);
        m_presets.push_back(t4);
    }

    // Load USER presets
    DatabaseManager *db = DatabaseManager::getInstance();
    if (db)
    {
        m_dbInternal = db->hasDatabaseInternal();
        m_dbExternal = db->hasDatabaseExternal();
    }

    if (m_dbInternal || m_dbExternal)
    {
        QSqlQuery queryPresets;
        queryPresets.exec("SELECT id, type, name, ranges FROM tempPresets");
        while (queryPresets.next())
        {
            int id = queryPresets.value(0).toInt();
            int type = queryPresets.value(1).toInt();
            QString name = queryPresets.value(2).toString();
            QString ranges = queryPresets.value(3).toString();

            TempPreset *d = new TempPreset(id, type, false, name, ranges, this);
            if (d) m_presets.push_back(d);
        }
    }

    return status;
}

void TempPresetManager::filter(const QString &filter)
{
    //qDebug() << "TempPresetManager::filter()" << filter;

    m_presetsFiltered.clear();

    for (auto pp: std::as_const(m_presets))
    {
        TempPreset *tp = qobject_cast<TempPreset*>(pp);
        if (tp->getName().toLower().contains(filter.toLower()))
        {
            m_presetsFiltered.push_back(tp);
        }
    }

    Q_EMIT presetsFilteredChanged();
}

/* ************************************************************************** */

bool TempPresetManager::isPresetNameValid(const QString &name)
{
    bool status = false;

    if (!name.isEmpty())
    {
        status = true;

        for (auto pp: std::as_const(m_presets))
        {
            TempPreset *tp = qobject_cast<TempPreset*>(pp);
            if (tp && tp->getName() == name)
            {
                status = false;
            }
        }
    }

    return status;
}

bool TempPresetManager::addPreset(const int type, const QString &name)
{
    //qDebug() << "TempPresetManager::addPreset(" << type << name << ")";

    TempPreset *newpreset = new TempPreset(0, type, false, name, "", this);
    if (newpreset)
    {
        newpreset->save();

        m_presets.push_back(newpreset);

        Q_EMIT presetsChanged();
        return true;
    }

    return false;
}

bool TempPresetManager::copyPreset(const QString &name, const QString &newName)
{
    qDebug() << "TempPresetManager::copyPreset(" << name << newName << ")";

    for (auto pp: std::as_const(m_presets))
    {
        TempPreset *tp = qobject_cast<TempPreset*>(pp);
        if (tp && tp->getName() == name)
        {
            TempPreset *newpreset = new TempPreset(*tp, newName, this);
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

    return false;
}

bool TempPresetManager::removePreset(const QString &name)
{
    bool status = false;

    //qDebug() << "TempPresetManager::removePreset()" << name;

    for (auto pp: std::as_const(m_presets))
    {
        TempPreset *tp = qobject_cast<TempPreset*>(pp);
        if (tp && !tp->getReadOnly() && tp->getName() == name)
        {
            // Remove from database
            if (m_dbInternal || m_dbExternal)
            {
                QSqlQuery removePreset;
                removePreset.prepare("DELETE FROM tempPresets WHERE id = :id");
                removePreset.bindValue(":id", tp->getId());

                if (removePreset.exec() == false)
                {
                    qWarning() << "> removePreset.exec() ERROR"
                               << removePreset.lastError().type() << ":" << removePreset.lastError().text();
                }
            }

            // Remove preset
            m_presets.removeOne(tp);
            delete tp;

            Q_EMIT presetsChanged();
            status = true;

            break;
        }
    }

    return status;
}

/* ************************************************************************** */

TempPreset *TempPresetManager::getPreset(const QString &name)
{
    TempPreset *p = nullptr;

    for (auto pp: std::as_const(m_presets))
    {
        TempPreset *tp = qobject_cast<TempPreset*>(pp);
        if (tp && tp->getName() == name)
        {
            return tp;
        }
    }

    return p;
}

/* ************************************************************************** */
