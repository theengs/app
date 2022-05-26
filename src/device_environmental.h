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

#ifndef DEVICE_ENVIRONMENTAL_H
#define DEVICE_ENVIRONMENTAL_H
/* ************************************************************************** */

#include "device_sensor.h"

#include <QObject>
#include <QString>

/* ************************************************************************** */

/*!
 * \brief The DeviceEnvironmental class
 */
class DeviceEnvironmental: public DeviceSensor
{
    Q_OBJECT

public:
    DeviceEnvironmental(const QString &deviceAddr, const QString &deviceName, QObject *parent = nullptr);
    DeviceEnvironmental(const QBluetoothDeviceInfo &d, QObject *parent = nullptr);
    virtual ~DeviceEnvironmental();

    // Chart environmental histogram
    Q_INVOKABLE void updateChartData_environmentalVoc(int maxDays);
};

/* ************************************************************************** */
#endif // DEVICE_ENVIRONMENTAL_H
