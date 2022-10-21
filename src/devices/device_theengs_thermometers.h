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

#ifndef DEVICE_THEENGS_THERMOMETERS_H
#define DEVICE_THEENGS_THERMOMETERS_H
/* ************************************************************************** */

#include "device_theengs.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Theengs thermometer/hygrometer devices
 */
class DeviceTheengsThermometers: public DeviceTheengs
{
    Q_OBJECT

    bool areValuesValid_thermometer(const float t) const;
    bool addDatabaseRecord_thermometer(const int64_t timestamp, const float t);

    bool areValuesValid_hygrometer(const float t, const float h) const;
    bool addDatabaseRecord_hygrometer(const int64_t timestamp, const float t, const float h);

public:
    DeviceTheengsThermometers(const QString &deviceAddr, const QString &deviceName,
                             const QString &deviceModel, const QString &devicePropsJson,
                             QObject *parent = nullptr);
    DeviceTheengsThermometers(const QBluetoothDeviceInfo &d,
                             const QString &deviceModel, const QString &devicePropsJson,
                             QObject *parent = nullptr);
    ~DeviceTheengsThermometers();

    // theengs decoder
    void parseTheengsProps(const QString &json);
    void parseTheengsAdvertisement(const QString &json);

    // Chart thermometer "min/max"
    Q_INVOKABLE void updateChartData_thermometerMinMax(int maxDays);

    // Chart thermometer "AIO"
    Q_INVOKABLE void getChartData_thermometerAIO(int maxDays, QDateTimeAxis *axis,
                                                 QLineSeries *temp, QLineSeries *hygro);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_THERMOMETERS_H
