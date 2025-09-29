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

#ifndef DEVICE_THEENGS_SBS1_H
#define DEVICE_THEENGS_SBS1_H
/* ************************************************************************** */

#include "device_theengs_actuators.h"

#include <QObject>
#include <QString>
#include <QStringLiteral>
#include <QList>
#include <QTimer>

#include <QBluetoothUuid>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * Switchbot SmartSwitch alias "SBS1" alias "X1"
 *
 * - https://github.com/OpenWonderLabs/SwitchBotAPI-BLE/blob/latest/devicetypes/bot.md
 */
class DeviceSwitchbotSmartSwitch: public DeviceTheengsActuators
{
    Q_OBJECT

    Q_PROPERTY(int switchMode READ getSwitchMode WRITE setSwitchMode NOTIFY switchmodeUpdated)
    Q_PROPERTY(int switchTime READ getSwitchTime WRITE setSwitchTime NOTIFY switchtimeUpdated)

    /// Device

    int m_switchMode = 0;
    int m_switchHoldTime = 1;

    int getSwitchMode() const { return m_switchMode; }
    void setSwitchMode(const int m);
    int getSwitchTime() const { return m_switchHoldTime; }
    void setSwitchTime(const int t);

    /// BLE

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

signals:
    void switchmodeUpdated();
    void switchtimeUpdated();

protected:
    virtual void deviceErrored(QLowEnergyController::Error error);
    virtual void deviceDisconnected();
    virtual void deviceConnected();

public:
    DeviceSwitchbotSmartSwitch(const QString &deviceAddr, const QString &deviceName,
                               const QString &deviceModel, const QString &devicePropsJson,
                               QObject *parent = nullptr);
    DeviceSwitchbotSmartSwitch(const QBluetoothDeviceInfo &d,
                               const QString &deviceModel, const QString &devicePropsJson,
                               QObject *parent = nullptr);
    ~DeviceSwitchbotSmartSwitch();

    Q_INVOKABLE void actionMode(const int mode = 0, const int inverted = 0);
    Q_INVOKABLE void actionAction(const int action = 0);
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_SBS1_H
