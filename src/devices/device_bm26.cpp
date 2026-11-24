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

#include "SettingsManager.h"
#include "MqttManager.h"

#include <algorithm>
#include <cstring>
#include <random>

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
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
#if defined(ENABLE_MBEDTLS)
    //m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
#endif

    parseTheengsProps(devicePropsJson);

    //if (!hasBatteryPercent()) m_deviceSensors += DeviceUtilsTheengs::SENSOR_BATTERY_PERCENT;
    //if (!hasBatteryVoltage()) m_deviceSensors += DeviceUtilsTheengs::SENSOR_BATTERY_VOLTAGE;
}

DeviceTheengsBM26::DeviceTheengsBM26(const QBluetoothDeviceInfo &d,
                                     const QString &deviceModel,
                                     const QString &devicePropsJson,
                                     QObject *parent):
    DeviceTheengsBatteryMonitors(d, deviceModel, devicePropsJson, parent)
{
    m_deviceModel = deviceModel;
    m_deviceType = DeviceUtils::DEVICE_THEENGS_BATTERYMONITOR;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
#if defined(ENABLE_MBEDTLS)
    //m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
#endif

    parseTheengsProps(devicePropsJson);

    //if (!hasBatteryPercent()) m_deviceSensors += DeviceUtilsTheengs::SENSOR_BATTERY_PERCENT;
    //if (!hasBatteryVoltage()) m_deviceSensors += DeviceUtilsTheengs::SENSOR_BATTERY_VOLTAGE;
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

    m_serviceVolt_bm6_firstwrite = false;
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

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
            // Windows hack, see: QTBUG-80770 and QTBUG-78488
            QTimer::singleShot(0, this, [=] () { m_serviceVolt->discoverDetails(QLowEnergyService::FullDiscovery); });
#else
            // Windows hack, see: QTBUG-80770 and QTBUG-78488
            QTimer::singleShot(0, this, [=] () { m_serviceVolt->discoverDetails(QLowEnergyService::SkipValueDiscovery); });
#endif
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
        //qDebug() << "DeviceTheengsBM26::serviceDetailsDiscovered_volt(" << m_deviceAddress << ") > ServiceDiscovered";

        if (m_serviceVolt)
        {
            if (m_deviceModel == "BM6")
            {
                // BM6 requires sending an encrypted command before making notify available

                const uint8_t data[16] = { 0xd1, 0x55, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
                uint8_t output[16] = {};
                uint8_t iv[16] = { };

#if defined(ENABLE_MBEDTLS)
                mbedtls_aes_context aes;
                mbedtls_aes_init(&aes);
                mbedtls_aes_setkey_enc(&aes, m_key_bm6, 128);
                mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, 16, iv, data, output);
                mbedtls_aes_free(&aes);
#endif

                // Characteristic "write"
                m_charWrite = m_serviceVolt->characteristic(uuid_volt_char_write);
                m_serviceVolt->writeCharacteristic(m_charWrite, QByteArray(reinterpret_cast<const char*>(output), 16),
                                                   QLowEnergyService::WriteWithResponse);
                m_serviceVolt_bm6_firstwrite = true;
            }
            else if (m_deviceModel == "BM2")
            {
                // Characteristic "read / notify"
                m_charNotif = m_serviceVolt->characteristic(uuid_volt_char_notify);
                m_notificationDesc = m_charNotif.clientCharacteristicConfiguration();
                m_serviceVolt->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

                // Debug
                if (!m_charNotif.isValid()) { qWarning() << "m_charNotif invalid"; }
                if (!m_notificationDesc.isValid()) { qWarning() << "m_notificationDesc invalid"; }
            }
            else
            {
                qWarning() << "DeviceTheengsBM26::serviceDetailsDiscovered_volt() BatteryMonitor device model" << m_deviceModel << "is INVALID";
            }
        }
    }
}

/* ************************************************************************** */

void DeviceTheengsBM26::bleDescriptorRead(const QLowEnergyDescriptor &, const QByteArray &)
{
    //qDebug() << "DeviceTheengsBM26::bleDescriptorRead()";
}

void DeviceTheengsBM26::bleDescriptorWritten(const QLowEnergyDescriptor &, const QByteArray &)
{
    //qDebug() << "DeviceTheengsBM26::bleDescriptorWritten()";
}

/* ************************************************************************** */

void DeviceTheengsBM26::bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    //qDebug() << "DeviceTheengsBM26::bleWriteDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();

    if (m_deviceModel == "BM6" && m_serviceVolt_bm6_firstwrite == true)
    {
        // Characteristic "read / notify"
        m_charNotif = m_serviceVolt->characteristic(uuid_volt_char_notify);
        m_notificationDesc = m_charNotif.clientCharacteristicConfiguration();
        m_serviceVolt->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
        m_serviceVolt_bm6_firstwrite = false;

        // Debug
        if (!m_charNotif.isValid()) { qWarning() << "m_charNotif invalid"; }
        if (!m_notificationDesc.isValid()) { qWarning() << "m_notificationDesc invalid"; }
    }
}

void DeviceTheengsBM26::bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    //qDebug() << "DeviceTheengsBM26::bleReadDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceTheengsBM26::bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    //qDebug() << "DeviceTheengsBM26::bleReadNotify(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();

    // Volt UUID // 16 bytes frames
    if (c.uuid() == uuid_volt_char_notify && value.size() == 16)
    {
#if defined(ENABLE_MBEDTLS)
        const uint8_t *data = reinterpret_cast<const uint8_t *>(value.constData());
        const uint8_t *key = (m_deviceModel == "BM6") ? m_key_bm6 : m_key_bm2;
        uint8_t output[16];
        decryptBlock(key, data, output);

        //qWarning() << "DECRYPTED DATA: 0x" << QByteArray(reinterpret_cast<const char*>(output), 16).toHex(' ');
        // BM2 // DATA: 0x "f5 1e 42 00 05 0c 00 00 00 02 03 71 24 00 00 00"
        // BM6 // DATA: 0x "d1 55 07 00 17 01 00 01 df 00 00 00 00 02 00 00"

        float volt = -99.f;
        float temp = -99.f;
        int batt = 0;

        if (m_deviceModel == "BM6")
        {
            temp = output[4];
            if (output[3] == 1) temp = -temp;
            //qDebug() << "(BM6) temp : " << temp;

            batt = output[5];
            //qDebug() << "(BM6) batt : " << batt;

            volt = static_cast<int16_t>(output[8] + (output[7] << 8)) / 100.f;
            //qDebug() << "(BM6) volt : " << volt;
        }
        else if (m_deviceModel == "BM2")
        {
            volt = ((output[2] | (output[1] << 8)) >> 4) / 100.0f;
            //qDebug() << "(BM2) volt : " << volt;

            batt = output[3];
            //qDebug() << "(BM2) batt : " << batt;
        }
        else
        {
            qWarning() << "DeviceTheengsBM26::bleReadNotify() BatteryMonitor device model" << m_deviceModel << "is INVALID";
        }

        if (volt <= 0.f)
        {
            // BM6 first notification ("d1 55 07 ff 00 ...") carries no measurement, wait for the next one
            return;
        }

        if (areValuesValid_voltagepercent(volt, batt))
        {
            handleVoltagePercent(volt, batt, false);

            // We only connect to read the data, so don't stay connected once we have it
            deviceDisconnect();
        }
        else
        {
            qWarning() << "DeviceTheengsBM26::bleReadNotify() !!! INVALID VOLTAGE !!! " << volt << "v";
        }
#else
        qWarning() << "DeviceTheengsBM26::bleReadNotify(" << m_deviceAddress << ") on"
                   << c.name() << " / uuid" << c.uuid() << value.size() << "UNABLE TO DECRYPT !!!";
#endif // ENABLE_MBEDTLS
    }
    else
    {
        qWarning() << "DeviceTheengsBM26::bleReadNotify(" << m_deviceAddress << ")  UNKOWN DATA  on"
                   << c.name() << " / uuid" << c.uuid() << value.size();
    }
}

void DeviceTheengsBM26::decryptBlock(const uint8_t *key, const uint8_t *input, uint8_t *output)
{
#if defined(ENABLE_MBEDTLS)
    // AES-128-CBC, one 16 bytes block, zero IV
    uint8_t iv[16] = { };

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, key, 128);
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, 16, iv, input, output);
    mbedtls_aes_free(&aes);
#else
    Q_UNUSED(key)
    memcpy(output, input, 16);
#endif
}

void DeviceTheengsBM26::handleVoltagePercent(const float volt, const int batt, const bool fromAdvert)
{
    QDateTime cdt = QDateTime::currentDateTime();

    // updates?
    if (volt != m_batteryVoltage)
    {
        m_lastUpdate = cdt;

        m_batteryVoltage = volt;
        Q_EMIT dataUpdated();
    }
    if (batt != m_batteryPercent)
    {
        m_lastUpdate = cdt;

        m_batteryPercent = batt;
        Q_EMIT dataUpdated();
    }

    addRealtimeRecord_voltagepercent(cdt, volt, batt);

    // advertisements arrive every second or so, only save when needed
    if (!fromAdvert || needsUpdateDb())
    {
        addDatabaseRecord_voltagepercent(cdt.toSecsSinceEpoch(), volt, batt);
    }

    refreshDataFinished(true);
}

/* ************************************************************************** */

void DeviceTheengsBM26::parseAdvertisementData(const uint16_t adv_mode, const uint16_t adv_id, const QByteArray &value)
{
    //qDebug() << "DeviceTheengsBM26::parseAdvertisementData(" << m_deviceAddress << adv_mode << adv_id << value.size() << ")";

#if defined(ENABLE_MBEDTLS)
    // Beside their iBeacon advertisement (battery percent only, decoded by Theengs Decoder), the BM2
    // and BM6 broadcast 16 bytes of manufacturer data encrypted with the same AES key as their GATT
    // notifications. Plaintext layout, identical on both:
    // - [0:6] device MAC address, in transmission order on some units and reversed on others
    // - [6:8] voltage, big endian, in 1/100 V
    // - [8]   state of charge in %
    // - [9:]  unknown
    // - https://github.com/1technophile/OpenMQTTGateway/pull/2373

    if (m_deviceModel != "BM2" && m_deviceModel != "BM6") return;
    if (adv_mode != DeviceUtils::BLE_ADV_MANUFACTURERDATA) return;
    if (value.size() != 14) return; // Qt strips the first two bytes into the manufacturer ID

    uint8_t ciphertext[16];
    ciphertext[0] = adv_id & 0xFF;
    ciphertext[1] = (adv_id >> 8) & 0xFF;
    memcpy(ciphertext + 2, value.constData(), 14);

    uint8_t plaintext[16];
    decryptBlock((m_deviceModel == "BM6") ? m_key_bm6 : m_key_bm2, ciphertext, plaintext);

    // The advertisement carries no plaintext marker: the embedded MAC is what identifies it
    const QByteArray embeddedMac(reinterpret_cast<const char *>(plaintext), 6);

    QByteArray advertiserMac = QByteArray::fromHex(getAddressMAC().remove(':').toLatin1());
    if (advertiserMac.size() == 6)
    {
        QByteArray reversedMac = advertiserMac;
        std::reverse(reversedMac.begin(), reversedMac.end());

        if (embeddedMac != advertiserMac && embeddedMac != reversedMac) return;
    }
    else if (!m_advMac.isEmpty() && embeddedMac != m_advMac)
    {
        // No advertiser MAC available (iOS / macOS), use the first valid embedded MAC as a reference
        return;
    }

    int centivolt = (plaintext[6] << 8) | plaintext[7];
    int batt = plaintext[8];
    // The embedded MAC is the real check, this only rejects nonsense. No lower bound beyond 0:
    // a BM6 keeps advertising valid data on a 3 V supply
    if (centivolt <= 0 || centivolt > 2000 || batt > 100)
    {
        qDebug() << "DeviceTheengsBM26::parseAdvertisementData(" << m_deviceAddress << ") implausible data:"
                 << centivolt << "cV" << batt << "%";
        return;
    }

    if (advertiserMac.size() != 6) m_advMac = embeddedMac;
    m_advData_lastUpdate = QDateTime::currentDateTime();

    // The advertisements now carry the data, stop retrying a pending connection
    if (m_stayConnected && m_ble_status < DeviceUtils::DEVICE_CONNECTED) deviceDisconnect();

    float volt = centivolt / 100.f;
    handleVoltagePercent(volt, batt, true);

    // MQTT send, using the same fields as the Theengs Decoder output for the iBeacon
    SettingsManager *sm = SettingsManager::getInstance();
    MqttManager *mq = MqttManager::getInstance();
    QString mac_clean = getAddressMAC().remove(':');
    if (sm && mq && !mac_clean.isEmpty())
    {
        QJsonObject obj;
        obj.insert("id", getAddressMAC());
        obj.insert("name", getName());
        obj.insert("rssi", getRssi());
        obj.insert("brand", "GENERIC");
        obj.insert("model", m_deviceModel + " Battery Monitor");
        obj.insert("model_id", m_deviceModel);
        obj.insert("type", "BATT");
        obj.insert("track", true);
        obj.insert("bvpp", true);
        obj.insert("volt", centivolt / 100.0);
        obj.insert("batt", batt);
        obj.insert("device", m_deviceModel + " Tracker");

        QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT/" + mac_clean;
        mq->publishData(topic, QString(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    }
#else
    Q_UNUSED(adv_mode)
    Q_UNUSED(adv_id)
    Q_UNUSED(value)
#endif // ENABLE_MBEDTLS
}

/* ************************************************************************** */

void DeviceTheengsBM26::bleServiceError(QLowEnergyService::ServiceError e)
{
    qWarning() << "DeviceTheengsBM26::bleServiceError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

float fakeFloat()
{
    // Use a random device to seed the generator
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Define the distribution between 10.0 and 14.0
    static std::uniform_real_distribution<float> dist(10.0f, 14.0f);

    return dist(gen);
}

void DeviceTheengsBM26::actionFakeVoltage()
{
    qDebug() << "DeviceGateway::actionFakeVoltage()" << getAddress() << getName();

    float volt = fakeFloat();

    // valid data?
    if (areValuesValid_voltage(volt))
    {
        m_lastUpdate = QDateTime::currentDateTime();

        // changes?
        if (volt != m_batteryVoltage)
        {
            m_batteryVoltage = volt;
            Q_EMIT dataUpdated();
        }

        // rt graph
        addRealtimeRecord_voltage(QDateTime::currentDateTime(), m_batteryVoltage);

        // database
        //if (needsUpdateDb())
        {
            addDatabaseRecord_voltage(m_lastUpdate.toSecsSinceEpoch(), m_batteryVoltage);
        }

        refreshDataFinished(true);
    }
}

bool DeviceTheengsBM26::hasRecentAdvertisementData() const
{
    return m_advData_lastUpdate.isValid() &&
           m_advData_lastUpdate.secsTo(QDateTime::currentDateTime()) < s_advData_timeout_s;
}

void DeviceTheengsBM26::actionConnect(const bool stayConnected)
{
    Q_UNUSED(stayConnected) // the data comes as notifications right after connecting, no need to stay

    // The advertisements carry the data, no need to connect
    if (hasRecentAdvertisementData())
    {
        qDebug() << "DeviceTheengsBM26::actionConnect()" << getAddress() << getName() << "> data from advertisements";
        return;
    }

    // Otherwise connect, read the data once, and disconnect (see bleReadNotify())
    actionReadVoltage();
}

void DeviceTheengsBM26::actionReadVoltage()
{
    qDebug() << "DeviceGateway::actionReadVoltage()" << getAddress() << getName();

    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        actionStarted(DeviceUtils::ACTION_UPDATE);
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        actionStarted(DeviceUtils::ACTION_UPDATE);
        deviceConnect(true); // retry until we get the data, bleReadNotify() then disconnects
    }
    else
    {
        qWarning() << "DeviceGateway::actionReadVoltage() BLE status: " << m_ble_status;
    }
}

/* ************************************************************************** */
/* ************************************************************************** */
