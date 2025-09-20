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

#include "device_bm26.h"

#if defined(ENABLE_MBEDTLS)
#include "mbedtls/aes.h"
#endif

#include <QBluetoothUuid>
#include <QBluetoothServiceInfo>
#include <QLowEnergyService>

#include <QSqlQuery>
#include <QSqlError>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QListIterator>
#include <QDebug>

/* ************************************************************************** */

DeviceTheengsBM26::DeviceTheengsBM26(const QString &deviceAddr,
                                     const QString &deviceName,
                                     const QString &deviceModel,
                                     const QString &devicePropsJson,
                                     QObject *parent):
    DeviceTheengsBatteryMonitors(deviceAddr, deviceName, deviceModel, devicePropsJson, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_BATTERYMONITOR;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (m_deviceModel == "BM2" || m_deviceModel == "BM6")
    {
        //
    }
}

DeviceTheengsBM26::DeviceTheengsBM26(const QBluetoothDeviceInfo &d,
                                     const QString &deviceModel,
                                     const QString &devicePropsJson,
                                     QObject *parent):
    DeviceTheengsBatteryMonitors(d, deviceModel, devicePropsJson, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_BATTERYMONITOR;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    parseTheengsProps(devicePropsJson);

    if (m_deviceModel == "BM2" || m_deviceModel == "BM6")
    {
        //
    }
}

/* ************************************************************************** */

DeviceTheengsBM26::~DeviceTheengsBM26()
{
    delete m_serviceVolt;
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBM26::deviceConnected()
{
    qDebug() << "DeviceTheengsBM26::deviceConnected(" << m_deviceAddress << ")";
    Device::deviceConnected();
}

/* ************************************************************************** */

void DeviceTheengsBM26::deviceDisconnected()
{
    qDebug() << "DeviceTheengsBM26::deviceDisconnected(" << m_deviceAddress << ")";
    Device::deviceDisconnected();
}

/* ************************************************************************** */

void DeviceTheengsBM26::deviceErrored(QLowEnergyController::Error error)
{
    qDebug() << "DeviceTheengsBM26::deviceErrored(" << error << ")";
    Device::deviceErrored(error);
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBM26::serviceScanDone()
{
    qDebug() << "DeviceTheengsBM26::serviceScanDone(" << m_deviceAddress << ")";

    if (m_serviceVolt)
    {
        if (m_serviceVolt->state() == QLowEnergyService::RemoteService)
        {
            connect(m_serviceVolt, &QLowEnergyService::stateChanged, this, &DeviceTheengsBM26::serviceDetailsDiscovered_volt);

            connect(m_serviceVolt, &QLowEnergyService::characteristicChanged, this, &DeviceTheengsBM26::bleReadNotify);
            connect(m_serviceVolt, &QLowEnergyService::characteristicRead, this, &DeviceTheengsBM26::bleReadDone);
            connect(m_serviceVolt, &QLowEnergyService::characteristicWritten, this, &DeviceTheengsBM26::bleWriteDone);

            connect(m_serviceVolt, &QLowEnergyService::descriptorRead, this, &DeviceTheengsBM26::bleDescriptorRead);
            connect(m_serviceVolt, &QLowEnergyService::descriptorWritten, this, &DeviceTheengsBM26::bleDescriptorWritten);

            connect(m_serviceVolt, &QLowEnergyService::errorOccurred, this, &DeviceTheengsBM26::bleServiceError);

            // Windows hack, see: QTBUG-80770 and QTBUG-78488
            QTimer::singleShot(0, this, [=] () { m_serviceVolt->discoverDetails(QLowEnergyService::SkipValueDiscovery); });
        }
    }
}

/* ************************************************************************** */

void DeviceTheengsBM26::addLowEnergyService(const QBluetoothUuid &uuid)
{
    qDebug() << "DeviceTheengsBM26::addLowEnergyService(" << uuid.toString() << ")";

    if (uuid == uuid_volt_srv)
    {
        delete m_serviceVolt;
        m_serviceVolt = nullptr;

        m_serviceVolt = m_bleController->createServiceObject(uuid);
        if (!m_serviceVolt)
        {
            qWarning() << "Cannot create service (volt) for uuid:" << uuid.toString();
        }
    }
}

/* ************************************************************************** */

void DeviceTheengsBM26::serviceDetailsDiscovered_volt(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::RemoteServiceDiscovered)
    {
        qDebug() << "DeviceTheengsBM26::serviceDetailsDiscovered_volt(" << m_deviceAddress << ") > ServiceDiscovered";

        if (m_serviceVolt)
        {
            // Characteristic "read / notify"
            m_charNotif = m_serviceVolt->characteristic(uuid_volt_char_notify);
            m_notificationDesc = m_charNotif.clientCharacteristicConfiguration();
            m_serviceVolt->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

            // Debug
            if (!m_charNotif.isValid()) { qWarning() << "m_charNotif invalid"; }
            if (!m_notificationDesc.isValid()) { qWarning() << "m_notificationDesc invalid"; }
        }
    }
}

/* ************************************************************************** */

void DeviceTheengsBM26::bleDescriptorRead(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceTheengsBM26::bleDescriptorRead()";
}

void DeviceTheengsBM26::bleDescriptorWritten(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceTheengsBM26::bleDescriptorWritten()";
}

/* ************************************************************************** */

void DeviceTheengsBM26::bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceTheengsBM26::bleWriteDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceTheengsBM26::bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceTheengsBM26::bleReadDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceTheengsBM26::bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceTheengsBM26::bleReadNotify(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();

    const uint8_t *data = reinterpret_cast<const quint8 *>(value.constData());

    // Volt UUID // 16 bytes frames
    if (c.uuid() == uuid_volt_char_notify && value.size() == 16)
    {
#if defined(ENABLE_MBEDTLS)
        unsigned char output[16];
        unsigned char iv[16] = { };
        unsigned char key[16] = { 108, 101, 97, 103, 101, 110, 100, 255, 254, 49, 56, 56, 50, 52, 54, 54, };

        mbedtls_aes_context aes;
        mbedtls_aes_init(&aes);
        mbedtls_aes_setkey_dec(&aes, key, 128);
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, 16, iv, data, output);
        mbedtls_aes_free(&aes);

        float volt = ((output[2] | (output[1] << 8)) >> 4) / 100.0f;
        if (volt > -20.f && volt < 20.f)
        {
            if (volt != m_batteryVoltage)
            {
                m_batteryVoltage = volt;
                Q_EMIT dataUpdated();
            }
/*
            // save
            if (needsUpdateDb()) // TODO
            {
                if (m_dbInternal || m_dbExternal)
                {
                    // hijack battery2 // stored as int, so m_batteryVoltage*100
                    int vvv = m_batteryVoltage*100.f;

                    QSqlQuery addData;
                    addData.prepare("REPLACE INTO sensorTheengs (deviceAddr, timestamp, battery2)"
                                    " VALUES (:deviceAddr, :ts, :voltage)");
                    addData.bindValue(":voltage", vvv);

                    addData.bindValue(":deviceAddr", getAddress());
                    addData.bindValue(":ts", m_lastUpdate.toString("yyyy-MM-dd hh:mm:ss"));

                    if (addData.exec())
                        m_lastUpdateDatabase = m_lastUpdate;
                    else
                        qWarning() << "> DeviceTheengsBatteryMonitors addData.exec(v) ERROR"
                                   << addData.lastError().type() << ":" << addData.lastError().text();
                }
            }
*/
        }
        else
        {
            qWarning() << "DeviceTheengsBM26::bleReadNotify() !!! INVALID VOLTAGE !!! " << volt << "v";
        }
#endif // ENABLE_MBEDTLS
    }
}

void DeviceTheengsBM26::bleServiceError(QLowEnergyService::ServiceError e)
{
    qDebug() << "DeviceTheengsBM26::bleServiceError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceTheengsBM26::actionReadVoltage()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */
