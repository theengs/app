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

#ifndef DEVICE_THEENGS_ACTUATORS_H
#define DEVICE_THEENGS_ACTUATORS_H
/* ************************************************************************** */

#include "device_theengs.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Theengs actuators
 *
 * DeviceSwitchbotSmartSwitch
 * Switchbot SmartSwitch alias "SBS1" alias "X1"
 */
class DeviceTheengsActuators: public DeviceTheengs
{
    Q_OBJECT

    Q_PROPERTY(QString mode READ getMode NOTIFY modeUpdated)
    Q_PROPERTY(bool state READ getState NOTIFY stateUpdated)

    QString m_mode;
    bool m_state = false;

    QString getMode() const { return m_mode; }
    bool getState() const { return m_state; }

signals:
    void modeUpdated();
    void stateUpdated();

public:
    DeviceTheengsActuators(const QString &deviceAddr, const QString &deviceName,
                           const QString &deviceModel, const QString &devicePropsJson,
                           QObject *parent = nullptr);
    DeviceTheengsActuators(const QBluetoothDeviceInfo &d,
                           const QString &deviceModel, const QString &devicePropsJson,
                           QObject *parent = nullptr);
    ~DeviceTheengsActuators();

    // theengs decoder
    void parseTheengsProps(const QString &json);
    void parseTheengsAdvertisement(const QString &json);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_ACTUATORS_H
