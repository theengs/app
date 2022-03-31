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

#ifndef DEVICE_TPMS_H
#define DEVICE_TPMS_H
/* ************************************************************************** */

#include "device_theengs.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Xiaomi "Temperature and Pression Monitoring System"
 */
class DeviceTPMS: public DeviceTheengs
{
    Q_OBJECT

    void parseAdvertisementData(const QByteArray &value);

public:
    DeviceTPMS(QString &deviceAddr, QString &deviceName, QObject *parent = nullptr);
    DeviceTPMS(const QBluetoothDeviceInfo &d, QObject *parent = nullptr);
    ~DeviceTPMS();

private:
    // QLowEnergyController related
    void serviceScanDone();
    void addLowEnergyService(const QBluetoothUuid &uuid);
};

/* ************************************************************************** */
#endif // DEVICE_TPMS_H
