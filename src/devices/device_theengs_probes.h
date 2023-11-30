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

#ifndef DEVICE_THEENGS_PROBES_H
#define DEVICE_THEENGS_PROBES_H
/* ************************************************************************** */

#include "device_theengs.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Theengs probes:
 * - InkBird IBT-2X / IBT-4XS / IBT-6XS temperature probes
 * - Govee H5055 "Bluetooth BBQ Thermometer"
 * - Xiaomi "Temperature and Pressure Monitoring System"
 */
class DeviceTheengsProbes: public DeviceTheengs
{
    Q_OBJECT

    // rt data
    QVector <std::pair<QDateTime, float>> m_rt_probe1;
    QVector <std::pair<QDateTime, float>> m_rt_probe2;
    QVector <std::pair<QDateTime, float>> m_rt_probe3;
    QVector <std::pair<QDateTime, float>> m_rt_probe4;
    QVector <std::pair<QDateTime, float>> m_rt_probe5;
    QVector <std::pair<QDateTime, float>> m_rt_probe6;

signals:
    void rtGraphUpdated();
    void rtGraphCleaned();

public:
    DeviceTheengsProbes(const QString &deviceAddr, const QString &deviceName,
                        const QString &deviceModel, const QString &devicePropsJson,
                        QObject *parent = nullptr);
    DeviceTheengsProbes(const QBluetoothDeviceInfo &d,
                        const QString &deviceModel, const QString &devicePropsJson,
                        QObject *parent = nullptr);
    ~DeviceTheengsProbes();

    // theengs decoder
    void parseTheengsProps(const QString &json);
    void parseTheengsAdvertisement(const QString &json);

    // Chart probe realtime
    Q_INVOKABLE void updateRtGraph(QDateTimeAxis *axis, int minutes,
                                   QLineSeries *temp1, QLineSeries *temp2,
                                   QLineSeries *temp3, QLineSeries *temp4,
                                   QLineSeries *temp5, QLineSeries *temp6);

    // Chart probe AIO
    Q_INVOKABLE void getChartData_probeAIO(int maxDays, QDateTimeAxis *axis,
                                           QLineSeries *temp1, QLineSeries *temp2,
                                           QLineSeries *temp3, QLineSeries *temp4,
                                           QLineSeries *temp5, QLineSeries *temp6);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_PROBES_H
