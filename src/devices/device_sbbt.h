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

#ifndef DEVICE_THEENGS_SBBT_H
#define DEVICE_THEENGS_SBBT_H
/* ************************************************************************** */

#include "device_theengs_windowactuators.h"

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QElapsedTimer>

#include <QBluetoothUuid>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Switchbot Blind Tilt alias "SBBT" alias "W270160X"
 */
class DeviceSwitchbotBlindTilt: public DeviceTheengsWindowActuators
{
    Q_OBJECT

    QBluetoothUuid uuid_data_srv = QBluetoothUuid(QStringLiteral("cba20d00-224d-11e6-9fb8-0002a5d5c51b"));
    QBluetoothUuid uuid_data_char_rx = QBluetoothUuid(QStringLiteral("cba20003-224d-11e6-9fb8-0002a5d5c51b"));
    QBluetoothUuid uuid_data_char_tx = QBluetoothUuid(QStringLiteral("cba20002-224d-11e6-9fb8-0002a5d5c51b"));

    QLowEnergyService *m_serviceData = nullptr;
    QLowEnergyCharacteristic m_charRX;
    QLowEnergyCharacteristic m_charTX;
    QLowEnergyDescriptor m_notificationDesc;

    void serviceScanDone();
    void addLowEnergyService(const QBluetoothUuid &uuid);
    void serviceDetailsDiscovered_data(QLowEnergyService::ServiceState newState);

    void bleServiceError(QLowEnergyService::ServiceError error);
    void bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void bleDescriptorRead(const QLowEnergyDescriptor &d, const QByteArray &value);
    void bleDescriptorWritten(const QLowEnergyDescriptor &d, const QByteArray &value);

protected:
    virtual void deviceErrored(QLowEnergyController::Error error);
    virtual void deviceDisconnected();
    virtual void deviceConnected();

public:
    DeviceSwitchbotBlindTilt(const QString &deviceAddr, const QString &deviceName,
                             const QString &deviceModel, const QString &devicePropsJson,
                             QObject *parent = nullptr);
    DeviceSwitchbotBlindTilt(const QBluetoothDeviceInfo &d,
                             const QString &deviceModel, const QString &devicePropsJson,
                             QObject *parent = nullptr);
    ~DeviceSwitchbotBlindTilt();

    Q_INVOKABLE void actionAction(const int action = 0);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_SBBT_H
