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
 * InkBird IBT-2X / IBT-4XS / IBT-6XS temperature probes
 * Govee H5055 "Bluetooth BBQ Thermometer"
 * Xiaomi "Temperature and Pression Monitoring System"
 */
class DeviceTheengsProbes: public DeviceTheengs
{
    Q_OBJECT

public:
    DeviceTheengsProbes(const QString &deviceAddr, const QString &deviceName,
                        const QString &deviceModel, const QString &devicePropsJson,
                        QObject *parent = nullptr);
    DeviceTheengsProbes(const QBluetoothDeviceInfo &d,
                        const QString &deviceModel, const QString &devicePropsJson,
                        QObject *parent = nullptr);
    ~DeviceTheengsProbes();

    void parseAdvertisementTheengs(const QString &json);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_PROBES_H
