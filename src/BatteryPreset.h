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

#ifndef BATTERY_PRESET_H
#define BATTERY_PRESET_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QQmlApplicationEngine>

/* ************************************************************************** */

class BatteryPresetUtils: public QObject
{
    Q_OBJECT

public:
    static void registerQML()
    {
        qRegisterMetaType<BatteryPresetUtils::PresetType>("BatteryPresetUtils::PresetType");

        qmlRegisterType<BatteryPresetUtils>("BatteryPresetUtils", 1, 0, "BatteryPresetUtils");
    }

    enum PresetType {
        PRESET_UNKNOWN          = 0,

        PRESET_LEADACID,
        PRESET_AGM, // Absorbent Glass Mat
        PRESET_EFB, // Enhanced Flooded Battery
        PRESET_GEL,
        PRESET_LION, // lithium-ion

        PRESET_LAST = PRESET_LION,
    };
    Q_ENUM(PresetType)
};

/* ************************************************************************** */

class BatteryPreset: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ getId CONSTANT)
    Q_PROPERTY(bool readOnly READ getReadOnly CONSTANT)
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY presetChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY presetChanged)
    Q_PROPERTY(float voltageMin READ getVoltageMin NOTIFY rangesChanged)
    Q_PROPERTY(float voltageMax READ getVoltageMax NOTIFY rangesChanged)

    int m_id = 0;
    int m_type = 0;
    bool m_readonly = true;
    QString m_name;

    float m_voltageMin;
    float m_voltageMax;

Q_SIGNALS:
    void presetChanged();
    void rangesChanged();

public:
    BatteryPreset(const int id, const int type, const bool ro,
                  const QString &name, const float vMin, const float vMax, QObject *parent);
    ~BatteryPreset();

    int getId() const { return m_id; }
    int getType() const { return m_type; }
    void setType(int t);
    bool getReadOnly() const { return m_readonly; }

    QString getName() const { return m_name; }
    void setName(const QString &n);

    Q_INVOKABLE float getVoltageMin() const { return m_voltageMin; }
    Q_INVOKABLE float getVoltageMax() const { return m_voltageMax; }
};

/* ************************************************************************** */
#endif // BATTERY_PRESET_H
