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

#ifndef DEVICE_THEENGS_WINDOW_ACTUATORS_H
#define DEVICE_THEENGS_WINDOW_ACTUATORS_H
/* ************************************************************************** */

#include "device_theengs.h"

#include <QObject>
#include <QList>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Theengs window actuators
 *
 * DeviceSwitchbotBlindTilt
 * Switchbot Blind Tilt alias "SBBT" alias "W270160X"
 *
 * DeviceSwitchbotCurtain
 * Switchbot Curtain alias "SBCU" alias "W070160X"
 */
class DeviceTheengsWindowActuators: public DeviceTheengs
{
    Q_OBJECT

    Q_PROPERTY(bool calibrated READ getCalibrated NOTIFY calibratedUpdated)
    Q_PROPERTY(bool moving READ getMoving NOTIFY movingUpdated)
    Q_PROPERTY(QString direction READ getDirection NOTIFY directionUpdated)
    Q_PROPERTY(int position READ getPosition NOTIFY positionUpdated)
    Q_PROPERTY(int open READ getOpen NOTIFY openUpdated)
    Q_PROPERTY(int lightlevel READ getLightLevel NOTIFY lightlevelUpdated)

    bool m_calibrated = false;
    bool m_moving = false;
    QString m_direction;
    int m_position = -1;
    int m_open = -1;
    int m_lightlevel= -1;

    bool getCalibrated() const { return m_calibrated; }
    bool getMoving() const { return m_moving; }
    QString getDirection() const { return m_direction; }
    int getPosition() const { return m_position; }
    int getOpen() const { return m_open; }
    int getLightLevel() const { return m_lightlevel; }

signals:
    void calibratedUpdated();
    void movingUpdated();
    void directionUpdated();
    void positionUpdated();
    void openUpdated();
    void lightlevelUpdated();

public:
    DeviceTheengsWindowActuators(const QString &deviceAddr, const QString &deviceName,
                                 const QString &deviceModel, const QString &devicePropsJson,
                                 QObject *parent = nullptr);
    DeviceTheengsWindowActuators(const QBluetoothDeviceInfo &d,
                                 const QString &deviceModel, const QString &devicePropsJson,
                                 QObject *parent = nullptr);
    ~DeviceTheengsWindowActuators();

    // theengs decoder
    void parseTheengsProps(const QString &json);
    void parseTheengsAdvertisement(const QString &json);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_WINDOW_ACTUATORS_H
