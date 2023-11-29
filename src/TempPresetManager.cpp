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

#include <QDir>
#include <QFile>
#include <QStringList>

#include <QDebug>

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
        TempPreset *t1 = new TempPreset(0, 1, true, "Beef", "", this);
        t1->addRange("Rare", 49, 52);
        t1->addRange("Medium Rare", 54, 57);
        t1->addRange("Medium", 60, 63);
        t1->addRange("Medium Well", 65, 68);
        t1->addRange("Well Done", 71, 71);
        m_presets.push_back(t1);

        TempPreset *t2 = new TempPreset(1, 2, true, "Pork", "", this);
        t2->addRange("Safe minimum internal temperature", 63, 63);
        t2->addRange("Ground Pork", 71, 71);
        m_presets.push_back(t2);

        TempPreset *t3 = new TempPreset(2, 3, true, "Chicken", "", this);
        t3->addRange("Safe minimum internal temperature", 74, 74);
        m_presets.push_back(t3);

        TempPreset *t4 = new TempPreset(3, 4, true, "Fish", "", this);
        t4->addRange("Safe minimum internal temperature", 63, 63);
        m_presets.push_back(t4);
    }

    // Load USER presets
    {
        // TODO
    }

    return status;
}

void TempPresetManager::stats()
{
    qDebug() << "TempPresetManager::readDB()" << m_presets.count() << "items in DB";
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

bool TempPresetManager::isNameValid(const QString &name)
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

bool TempPresetManager::addPreset()
{
    //qDebug() << "TempPresetManager::addPreset()";

    TempPreset *tp = new TempPreset(0, 0, false, "New Preset", "", this);
    if (tp)
    {
        m_presets.push_back(tp);
        Q_EMIT presetsChanged();
        return true;
    }

    return false;
}

bool TempPresetManager::addPreset(const int type, const QString &name)
{
    //qDebug() << "TempPresetManager::addPreset(" << type << name << ")";

    TempPreset *tp = new TempPreset(0, type, false, name, "", this);
    if (tp)
    {
        m_presets.push_back(tp);
        Q_EMIT presetsChanged();
        return true;
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
        if (tp && tp->getName() == name)
        {
            m_presets.removeOne(tp);
            delete tp;

            Q_EMIT presetsChanged();
            status = true;
        }
    }

    return status;
}

/* ************************************************************************** */
