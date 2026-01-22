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

#ifndef DEVICE_THEENGS_BM26_H
#define DEVICE_THEENGS_BM26_H
/* ************************************************************************** */

#include "device_theengs_batterymonitors.h"

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QElapsedTimer>

#include <QBluetoothUuid>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

/* ************************************************************************** */

/*!
 * BM2 / BM6 battery monitors
 *
 * - https://github.com/1technophile/OpenMQTTGateway/blob/226297ba5b445461c0d2e228c6d4925000c1d354/main/gatewayBLEConnect.cpp#L259
 */
class DeviceTheengsBM26: public DeviceTheengsBatteryMonitors
{
    Q_OBJECT

    QBluetoothUuid uuid_volt_srv = QBluetoothUuid(QStringLiteral("0000fff0-0000-1000-8000-00805f9b34fb"));
    QBluetoothUuid uuid_volt_char_write = QBluetoothUuid(QStringLiteral("0000fff3-0000-1000-8000-00805f9b34fb"));
    QBluetoothUuid uuid_volt_char_notify = QBluetoothUuid(QStringLiteral("0000fff4-0000-1000-8000-00805f9b34fb"));

    QLowEnergyService *m_serviceVolt = nullptr;
    QLowEnergyCharacteristic m_charWrite;
    QLowEnergyCharacteristic m_charNotif;
    QLowEnergyDescriptor m_notificationDesc;

    const uint8_t m_key_bm2[16] = { 108, 101, 97, 103, 101, 110, 100, 255, 254, 49, 56, 56, 50, 52, 54, 54, };
    const uint8_t m_key_bm6[16] = { 108, 101, 97, 103, 101, 110, 100, 255, 254, 48, 49, 48, 48, 48, 48, 57, };

    void serviceScanDone();
    void addLowEnergyService(const QBluetoothUuid &uuid);
    void serviceDetailsDiscovered_volt(QLowEnergyService::ServiceState newState);

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
    DeviceTheengsBM26(const QString &deviceAddr, const QString &deviceName,
                      const QString &deviceModel, const QString &devicePropsJson,
                      QObject *parent = nullptr);
    DeviceTheengsBM26(const QBluetoothDeviceInfo &d,
                      const QString &deviceModel, const QString &devicePropsJson,
                      QObject *parent = nullptr);
    ~DeviceTheengsBM26();

    Q_INVOKABLE void actionFakeVoltage();
    Q_INVOKABLE void actionReadVoltage();
};

/* ************************************************************************** */
#endif // DEVICE_THEENGS_BM26_H
