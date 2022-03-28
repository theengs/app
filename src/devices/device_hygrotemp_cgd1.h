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

#ifndef DEVICE_HYGROTEMP_CGD1_H
#define DEVICE_HYGROTEMP_CGD1_H
/* ************************************************************************** */

#include "device_sensor.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Qingping (formerly ClearGrass) "Bluetooth Alarm Clock"
 * CGD1 device / squared body / LCD
 */
class DeviceHygrotempCGD1: public DeviceSensor
{
    Q_OBJECT

    void parseAdvertisementData(const QByteArray &value);

public:
    DeviceHygrotempCGD1(QString &deviceAddr, QString &deviceName, QObject *parent = nullptr);
    DeviceHygrotempCGD1(const QBluetoothDeviceInfo &d, QObject *parent = nullptr);
    ~DeviceHygrotempCGD1();

private:
    // QLowEnergyController related
    void serviceScanDone();
    void addLowEnergyService(const QBluetoothUuid &uuid);
};

/* ************************************************************************** */
#endif // DEVICE_HYGROTEMP_CGD1_H
