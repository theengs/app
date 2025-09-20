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

#ifndef DEVICE_THEENGS_SBCU_H
#define DEVICE_THEENGS_SBCU_H
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
 * Switchbot Curtain alias "SBCU" alias "W070160X"
 */
class DeviceSwitchbotCurtain: public DeviceTheengsWindowActuators
{
    Q_OBJECT

    QBluetoothUuid uuid_data_srv = QBluetoothUuid(QStringLiteral("0000fff0-0000-1000-8000-00805f9b34fb"));
    QBluetoothUuid uuid_data_char_notify = QBluetoothUuid(QStringLiteral("0000ff04-0000-1000-8000-00805f9b34fb"));

    QLowEnergyService *m_serviceData = nullptr;
    QLowEnergyCharacteristic m_charNotif;
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
    DeviceSwitchbotCurtain(const QString &deviceAddr, const QString &deviceName,
                             const QString &deviceModel, const QString &devicePropsJson,
                             QObject *parent = nullptr);
    DeviceSwitchbotCurtain(const QBluetoothDeviceInfo &d,
                             const QString &deviceModel, const QString &devicePropsJson,
                             QObject *parent = nullptr);
    ~DeviceSwitchbotCurtain();
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_SBCU_H
