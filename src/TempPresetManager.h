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

#ifndef TEMP_PRESET_MANAGER_H
#define TEMP_PRESET_MANAGER_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QVariant>
#include <QList>

class TempPreset;

/* ************************************************************************** */

class TempPresetManager: public QObject
{
    Q_OBJECT

    QList <QObject *> m_presets;
    Q_PROPERTY(QVariant presets READ getPresets NOTIFY presetsChanged)
    Q_PROPERTY(int presetCount READ getPresetCount NOTIFY presetsChanged)

    QList <QObject *> m_presetsFiltered;
    Q_PROPERTY(QVariant presetsFiltered READ getPresetsFiltered NOTIFY presetsFilteredChanged)
    Q_PROPERTY(int presetCountFiltered READ getPresetCountFiltered NOTIFY presetsFilteredChanged)

    int getPresetCount() { return m_presets.size(); }
    QVariant getPresets() { return QVariant::fromValue(m_presets); }

    int getPresetCountFiltered() { return m_presetsFiltered.size(); }
    QVariant getPresetsFiltered() { return QVariant::fromValue(m_presetsFiltered); }

    bool m_isLoaded = false;
    bool readDB_csv(const QString &path);
    void stats();

    // Singleton
    static TempPresetManager *instance;
    TempPresetManager();
    ~TempPresetManager();

Q_SIGNALS:
    void presetsChanged();
    void presetsFilteredChanged();

public:
    static TempPresetManager *getInstance();

    Q_INVOKABLE bool load();
    Q_INVOKABLE void filter(const QString &filter);

    Q_INVOKABLE bool isNameValid(const QString &name);

    Q_INVOKABLE bool addPreset();
    Q_INVOKABLE bool addPreset(const int type, const QString &name);
    Q_INVOKABLE bool removePreset(const QString &name);
};

/* ************************************************************************** */
#endif // TEMP_PRESET_MANAGER_H
