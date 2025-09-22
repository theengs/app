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

#ifndef DEVICE_THEENGS_BATTERYMONITORS_H
#define DEVICE_THEENGS_BATTERYMONITORS_H
/* ************************************************************************** */

#include "device_theengs.h"

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QElapsedTimer>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Theengs generic battery monitors
 */
class DeviceTheengsBatteryMonitors: public DeviceTheengs
{
    Q_OBJECT

    Q_PROPERTY(QString preset READ getPreset WRITE setPreset NOTIFY presetUpdated)
    Q_PROPERTY(int realtimeWindow READ getRtWindow WRITE setRtWindow NOTIFY rtWindowUpdated)

    // battery monitor rt data
    QList <std::pair<QDateTime, float>> m_rt_batt;

    QElapsedTimer m_rt_lastupdate;

    // battery monitor settings
    QString m_preset;
    int m_realtime_window = 10;

    QString getPreset() const { return m_preset; }
    void setPreset(const QString &p);

    int getRtWindow() const { return m_realtime_window; }
    void setRtWindow(const int w);

signals:
    void presetUpdated();
    void rtGraphCleaned();
    void rtGraphUpdated();
    void rtWindowUpdated();

public:
    DeviceTheengsBatteryMonitors(const QString &deviceAddr, const QString &deviceName,
                                 const QString &deviceModel, const QString &devicePropsJson,
                                 QObject *parent = nullptr);
    DeviceTheengsBatteryMonitors(const QBluetoothDeviceInfo &d,
                                 const QString &deviceModel, const QString &devicePropsJson,
                                 QObject *parent = nullptr);
    ~DeviceTheengsBatteryMonitors();

    // theengs decoder
    void parseTheengsProps(const QString &json);
    void parseTheengsAdvertisement(const QString &json);

    // Chart battery monitoring
    Q_INVOKABLE void getChartData_batteryRT(QDateTimeAxis *axis, QLineSeries *batt, bool reload = false);
    Q_INVOKABLE void getChartData_batteryHistory(QDateTimeAxis *axis, QLineSeries *batt, bool reload = false);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_BATTERYMONITORS_H
