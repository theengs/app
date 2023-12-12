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

#ifndef TEMP_PRESET_H
#define TEMP_PRESET_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QQmlApplicationEngine>

/* ************************************************************************** */

class PresetUtils: public QObject
{
    Q_OBJECT

public:
    static void registerQML()
    {
        qRegisterMetaType<PresetUtils::PresetType>("PresetUtils::PresetType");

        qmlRegisterType<PresetUtils>("PresetUtils", 1, 0, "PresetUtils");
    }

    enum PresetType {
        PRESET_UNKNOWN          = 0,

        PRESET_MEAT,
        PRESET_FISH,
        PRESET_EGG,
        PRESET_POULTRY,
        PRESET_SEAFOOD,
        PRESET_VEGETABLE,
        PRESET_FRUIT,

        PRESET_LAST = PRESET_FRUIT,
    };
    Q_ENUM(PresetType)
};

/* ************************************************************************** */

class TempRange: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString color READ getColor WRITE setColor NOTIFY rangeChanged)
    Q_PROPERTY(float tempMin READ getTempMin WRITE setTempMin NOTIFY rangeChanged)
    Q_PROPERTY(float tempMax READ getTempMax WRITE setTempMax NOTIFY rangeChanged)
    Q_PROPERTY(float tempMaxGraph READ getTempMaxGraph NOTIFY rangeChanged)
    Q_PROPERTY(bool tempMaxEnabled READ isTempMaxEnabled WRITE setTempMaxEnabled NOTIFY rangeChanged)
    Q_PROPERTY(bool tempMaxDisabled READ isTempMaxDisabled WRITE setTempMaxDisabled NOTIFY rangeChanged)

    Q_PROPERTY(float tempMin_min READ getTempMinMin NOTIFY rangeLimitsChanged)
    Q_PROPERTY(float tempMin_max READ getTempMinMax NOTIFY rangeLimitsChanged)
    Q_PROPERTY(float tempMax_min READ getTempMaxMin NOTIFY rangeLimitsChanged)
    Q_PROPERTY(float tempMax_max READ getTempMaxMax NOTIFY rangeLimitsChanged)

    QString m_name;
    QString m_color;
    float m_tempMin;
    float m_tempMax;
    bool m_tempMax_enabled = false;

    float m_tempMin_min = 0.f;
    float m_tempMin_max = 100.f;
    float m_tempMax_min = 0.f;
    float m_tempMax_max = 100.f;

Q_SIGNALS:
    void nameChanged();
    void rangeChanged();
    void rangeLimitsChanged();

public:
    TempRange(const QString &name, const float tempMin, const float tempMax,
              const bool tempMaxEnabled, QObject *parent);
    TempRange(const TempRange &r, QObject *parent);
    ~TempRange() = default;

    QString getName() const { return m_name; }
    void setName(const QString &n);

    QString getColor() const { return m_color; }
    void setColor(const QString &c);

    float getTempMin() const { return m_tempMin; }
    void setTempMin(float t);

    float getTempMaxGraph() const { return m_tempMax_enabled ? m_tempMax : 100; }
    float getTempMax() const { return m_tempMax; }
    void setTempMax(float t);

    bool isTempMaxEnabled() const { return m_tempMax_enabled; }
    void setTempMaxEnabled(bool d);
    bool isTempMaxDisabled() const { return !m_tempMax_enabled; }
    void setTempMaxDisabled(bool d);

    float getTempMinMin() const { return m_tempMin_min; }
    float getTempMinMax() const { return m_tempMin_max; }
    float getTempMaxMin() const { return m_tempMax_min; }
    float getTempMaxMax() const { return m_tempMax_max; }
    void setTempMinMin(float f);
    void setTempMinMax(float f);
    void setTempMaxMin(float f);
    void setTempMaxMax(float f);
};

/* ************************************************************************** */

class TempPreset: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ getId CONSTANT)
    Q_PROPERTY(bool readOnly READ getReadOnly CONSTANT)

    Q_PROPERTY(int type READ getType WRITE setType NOTIFY presetChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY presetChanged)

    Q_PROPERTY(QVariant ranges READ getRanges NOTIFY rangesChanged)
    Q_PROPERTY(int rangeCount READ getRangeCount NOTIFY rangesChanged)
    Q_PROPERTY(float rangeMin READ getRangeMin NOTIFY rangesChanged)
    Q_PROPERTY(float rangeMax READ getRangeMax NOTIFY rangesChanged)

    int m_id = 0;
    int m_type = 0;
    bool m_readonly = false;
    QString m_name;

    QList <QObject *> m_ranges;

Q_SIGNALS:
    void presetChanged();
    void rangesChanged();

public:
    TempPreset(const int id, const int type, const bool ro,
               const QString &name, const QString &ranges, QObject *parent);
    TempPreset(const TempPreset &p, const QString &newname, QObject *parent);
    ~TempPreset();

    void save();
    void saveRanges();
    void loadRanges(const QString &json);

    Q_INVOKABLE bool isRangeNameValid(const QString &name);
    Q_INVOKABLE bool addRange(const QString &name, const bool beforAfter,
                              const float min, const float max, const bool maxEnabled);
    Q_INVOKABLE bool removeRange(const QString &name);

    Q_INVOKABLE float getTempMin() const;
    Q_INVOKABLE float getTempMax() const;
    Q_INVOKABLE float getTempMin_default() const;
    Q_INVOKABLE float getTempMax_default() const;
    Q_INVOKABLE float getTempMin_add() const;
    Q_INVOKABLE float getTempMax_add() const;

    int getId() const { return m_id; }
    int getType() const { return m_type; }
    void setType(int t);
    bool getReadOnly() const { return m_readonly; }
    QString getName() const { return m_name; }
    void setName(const QString &n);

    int getPresetRangeFromTemp(float temp) const;

    int getRangeCount() const { return m_ranges.size(); }
    float getRangeMin() const;
    float getRangeMax() const;
    void updateRangesMinMax() const;
    QString getRangesMinMax() const;
    QVariant getRanges() const { return QVariant::fromValue(m_ranges); }

    QList <QObject *> getRangesInternal() const { return m_ranges; }
};

/* ************************************************************************** */
#endif // TEMP_PRESET_H
