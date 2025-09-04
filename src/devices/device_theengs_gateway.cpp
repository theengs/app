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

#include "device_theengs_gateway.h"
#include "../device_utils.h"

// Gateway communication
#include "../blufi/BluFiFrame.h"
#include "../blufi/BluFiUtils.h"

#include <QBluetoothUuid>
#include <QLowEnergyService>

#include <QJsonDocument>
#include <QSqlQuery>
#include <QSqlError>

#include <QByteArray>
#include <QString>
#include <QDebug>

/* ************************************************************************** */
/* ************************************************************************** */

DeviceGateway::DeviceGateway(const QString &deviceAddr, const QString &deviceName, QObject *parent):
    Device(deviceAddr, deviceName, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_GATEWAY;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    // Load device infos, bias, limits and initial data
    if (m_dbInternal || m_dbExternal)
    {
        DeviceGateway::getSqlDeviceInfos();
    }

    // Set device name?
    //setName(deviceName);

    // timeout?
    //connect(&m_setupTimer, &QTimer::timeout, this, &DeviceGateway::timeoutSequence_internal, Qt::UniqueConnection);
}

/* ************************************************************************** */

DeviceGateway::DeviceGateway(const QBluetoothDeviceInfo &info, QObject *parent):
    Device(info, parent)
{
    m_deviceType = DeviceUtils::DEVICE_THEENGS_GATEWAY;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_CONNECTION;
    m_deviceBluetoothMode += DeviceUtils::DEVICE_BLE_ADVERTISEMENT;

    // Load device infos, bias, limits and initial data
    if (m_dbInternal || m_dbExternal)
    {
        DeviceGateway::getSqlDeviceInfos();
    }

    // Set device name and RSSI
    setName(info.name());
    setRssi(info.rssi());

    // Set gateway status
    const QList<quint16> &manufacturerIds = info.manufacturerIds();
    for (const auto adv_id: manufacturerIds)
    {
        qDebug() << info.name() << info.address() << Qt::hex
                 << "ID" << adv_id
                 << "manufacturer data" << Qt::dec << info.manufacturerData(adv_id).size() << Qt::hex
                 << "bytes:" << info.manufacturerData(adv_id).toHex();

        parseAdvertisementData(DeviceUtils::BLE_ADV_MANUFACTURERDATA, adv_id, info.manufacturerData(adv_id));
    }

    // DEBUG // fake presets
    //Network *n1 = new Network("test1", -48, true, this); m_networksAvailable.push_back(n1);
    //Network *n2 = new Network("test2", -72, true, this); m_networksAvailable.push_back(n2);

    // timeout?
    //connect(&m_setupTimer, &QTimer::timeout, this, &DeviceGateway::timeoutSequence_internal, Qt::UniqueConnection);
}

/* ************************************************************************** */

DeviceGateway::~DeviceGateway()
{
    delete m_serviceBluFi;

    qDeleteAll(m_networksAvailable);
    m_networksAvailable.clear();
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DeviceGateway::getSqlDeviceInfos()
{
    qDebug() << "DeviceGateway::getSqlDeviceInfos(" << m_deviceAddress << ")";
    bool status = false;

    if (m_dbInternal || m_dbExternal)
    {
        QSqlQuery getInfos;
        getInfos.prepare("SELECT " \
                         "deviceModel, deviceAddrMAC," \
                         "isOnboarded, password," \
                         "settings " \
                         "FROM gateways WHERE deviceAddr = :deviceAddr");
        getInfos.bindValue(":deviceAddr", getAddress());
        if (getInfos.exec())
        {
            while (getInfos.next())
            {
                m_deviceModel = getInfos.value(0).toString();
                m_deviceAddressMAC = getInfos.value(1).toString();
                //m_onboardingStatus = getInfos.value(2).toInt();
                //m_password = getInfos.value(3).toString();

                QString settings = getInfos.value(4).toString();
                QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
                if (!doc.isNull() && doc.isObject())
                {
                    m_additionalSettings = doc.object();
                }

                status = true;
                Q_EMIT settingsUpdated();
            }
        }
    }

    return status;
}

/* ************************************************************************** */

void DeviceGateway::setName(const QString &name)
{
    Device::setName(name);

    if (name.isEmpty()) return;
/*
    if (m_deviceName.startsWith("OMG_") && m_deviceName.size() == 16)
    {
        // the WiFi MAC address is in the name (no : separator)
        QString mac_from_name = m_deviceName.last(12);

        QString wifi_mac;
        QString ble_mac;

        // set WiFi MAC
        for (int i = 0; i < 12; i+=2)
        {
            wifi_mac += mac_from_name.sliced(i, 2);
            if (i < 10) wifi_mac += ':';
        }

        // set BLE MAC
        ble_mac = DeviceGateway::bleMac_from_wifiMac(wifi_mac);

        // recap
        //qDebug() << "wifi_mac " << wifi_mac;
        //qDebug() << "ble_mac " << ble_mac;

        m_deviceAddressMAC = ble_mac;
        m_mac_ble = ble_mac;
        m_mac_wifi = wifi_mac;
    }
*/
}

/* ************************************************************************** */

QString DeviceGateway::bleMac_from_wifiMac(const QString &wifi_mac_input)
{
    QString wifi_mac;

    // set WiFi MAC
    wifi_mac = wifi_mac_input;
    wifi_mac = wifi_mac.remove(':');
    if (wifi_mac.size() != 12)
    {
        qWarning() << "wifi mac has the wrong size";
        return QString();
    }

    // set BLE MAC
    uint8_t ble_mac_uint8[6];
    int ble_mac_int[6];
    QByteArray ble_mac_qba;
    for (int i = 0; i < 12; i += 2)
    {
        bool error = false;
        ble_mac_int[i/2] = (QString(wifi_mac.at(i)).toUInt(&error, 16) << 4) +
                             QString(wifi_mac.at(i+1)).toUInt(&error, 16);
    }
    for (int i = 5; i >= 0; i--)
    {
        if ((ble_mac_int[i] += 2) < 255)
        {
            break;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        ble_mac_qba.push_back(static_cast<char>(ble_mac_int[i]));
        ble_mac_uint8[i] = ble_mac_int[i];
    }

    return ble_mac_qba.toHex(':').toUpper();
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceGateway::deviceConnected()
{
    qDebug() << "DeviceGateway::deviceConnected(" << m_deviceAddress << ")";
    Device::deviceConnected();
}

/* ************************************************************************** */

void DeviceGateway::deviceDisconnected()
{
    qDebug() << "DeviceGateway::deviceDisconnected(" << m_deviceAddress << ")";
    Device::deviceDisconnected();
}

/* ************************************************************************** */

void DeviceGateway::deviceErrored(QLowEnergyController::Error error)
{
    qDebug() << "DeviceGateway::deviceErrored(" << error << ")";
    Device::deviceErrored(error);
/*
    if (m_ble_action == DeviceUtils::ACTION_THEENGS_ONBOARDING ||
        m_ble_action == DeviceUtils::ACTION_THEENGS_WIFI_UPDATE)
    {
        m_setupSession.ble_errored = true;
        Q_EMIT sequenceUpdated();

        if (m_ble_status >= DeviceUtils::DEVICE_CONNECTED)
        {
            // we are connected, we'll disconnect first
        }
        else
        {
            // we are NOT connected, we'll retry after a second (if we haven't alread)
            if (m_setupSession.sequence_retry < 3)
            {
                QTimer::singleShot(1000, this, SLOT(retrySequence_internal()));
            }
            else
            {
                // we retried enough // we mark the sequence as errored
                m_setupSession.sequence_status = -1;
                Q_EMIT sequenceUpdated();
            }
        }
    }
*/
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceGateway::serviceScanDone()
{
    qDebug() << "DeviceGateway::serviceScanDone(" << m_deviceAddress << ")";

    if (m_serviceBluFi)
    {
        if (m_serviceBluFi->state() == QLowEnergyService::RemoteService)
        {
            connect(m_serviceBluFi, &QLowEnergyService::stateChanged, this, &DeviceGateway::serviceDetailsDiscovered_blufi);

            connect(m_serviceBluFi, &QLowEnergyService::characteristicChanged, this, &DeviceGateway::bleReadNotify);
            connect(m_serviceBluFi, &QLowEnergyService::characteristicRead, this, &DeviceGateway::bleReadDone);
            connect(m_serviceBluFi, &QLowEnergyService::characteristicWritten, this, &DeviceGateway::bleWriteDone);

            //connect(m_serviceBluFi, &QLowEnergyService::descriptorRead, this, &DeviceGateway::descriptorRead);
            //connect(m_serviceBluFi, &QLowEnergyService::descriptorWritten, this, &DeviceGateway::descriptorWritten);

            connect(m_serviceBluFi, &QLowEnergyService::errorOccurred, this, &DeviceGateway::bleError);

            // Windows hack, see: QTBUG-80770 and QTBUG-78488
            QTimer::singleShot(0, this, [=] () { m_serviceBluFi->discoverDetails(QLowEnergyService::FullDiscovery); });
        }
    }
}

/* ************************************************************************** */

void DeviceGateway::addLowEnergyService(const QBluetoothUuid &uuid)
{
    qDebug() << "DeviceGateway::addLowEnergyService(" << uuid.toString() << ")";

    if (uuid == uuid_bluefi_srv) // BluFi service
    {
        delete m_serviceBluFi;
        m_serviceBluFi = nullptr;

        m_serviceBluFi = m_bleController->createServiceObject(uuid);
        if (!m_serviceBluFi)
        {
            qWarning() << "Cannot create service (blufi) for uuid:" << uuid.toString();
        }
    }
}

/* ************************************************************************** */

void DeviceGateway::serviceDetailsDiscovered_blufi(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::RemoteServiceDiscovered)
    {
        qDebug() << "DeviceGateway::serviceDetailsDiscovered_blufi(" << m_deviceAddress << ") > ServiceDiscovered";

        if (m_serviceBluFi)
        {
            // Characteristic "read / notify"
            m_charRead = m_serviceBluFi->characteristic(uuid_bluefi_char_read);
            m_notificationDesc = m_charRead.clientCharacteristicConfiguration();
            m_serviceBluFi->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

            // Characteristic "write"
            m_charWrite = m_serviceBluFi->characteristic(uuid_bluefi_char_write);

            // Debug
            if (!m_charRead.isValid()) { qWarning() << "m_charRead invalid"; }
            if (!m_charWrite.isValid()) { qWarning() << "m_charWrite invalid"; }
            if (!m_notificationDesc.isValid()) { qWarning() << "m_notificationDesc invalid"; }

            // Connected and ready?

            areWeReadyYet();
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DeviceGateway::areWeReadyYet()
{
    /// Check services ///
    if (m_serviceBluFi && m_serviceBluFi->state() == QLowEnergyService::RemoteServiceDiscovered) {
        qDebug() << "m_serviceBluFi READY";
    } else {
        return false;
    }

    /// Check some values ///

    //

    /// Connected and ready ///

    //

    return true;
}

void DeviceGateway::deviceReady()
{
    if (m_ble_action >= DeviceUtils::ACTION_THEENGS)
    {
        // simple sequences
        if (m_ble_action > DeviceUtils::ACTION_BLUFI)
        {
            // start/reset BluFi sequence
            m_setupSession.blufi_seq_nb = 0;

            startHandshake();
            setSeqMode();

            if (m_ble_action == DeviceUtils::ACTION_BLUFI_WIFI_GET_STATUS) getWifiState();
            if (m_ble_action == DeviceUtils::ACTION_BLUFI_WIFI_GET_LIST) getWifiList();
            if (m_ble_action == DeviceUtils::ACTION_BLUFI_WIFI_SET_CREDENTIALS) setWifiCredentials();
            if (m_ble_action == DeviceUtils::ACTION_BLUFI_WIFI_CONNECT) setWifiConnect();
            if (m_ble_action == DeviceUtils::ACTION_BLUFI_WIFI_DISCONNECT) setWifiDisconnect();

            return;
        }

        // complex sequences
        if (m_ble_action > DeviceUtils::ACTION_THEENGS)
        {
            m_setupSession.blufi_seq_nb = 0;
            m_setupSession.ble_connected = true;
            m_setupSession.sequence_status = 1;
            Q_EMIT sequenceUpdated();

            startHandshake();
            setSeqMode();

            if (m_ble_action == DeviceUtils::ACTION_THEENGS_ONBOARDING)
            {
                if (!m_setupSession.mqtt_settings_sent)
                {
                    setCustomData();
                    m_setupSession.mqtt_settings_sent = true;
                    Q_EMIT sequenceUpdated();
                }

                if (!m_setupSession.wifi_settings_sent)
                {
                    setWifiCredentials();
                    m_setupSession.wifi_settings_sent = true;
                    Q_EMIT sequenceUpdated();

                    setWifiConnect();
                    QTimer::singleShot(1000, this, SLOT(getWifiState()));
                }
                else if (m_setupSession.wifi_settings_sent &&
                         !m_setupSession.wifi_connected)
                {
                    getWifiState();
                }

                if (m_setupSession.wifi_connected)
                {
                    createGateway();
                }
            }

            if (m_ble_action == DeviceUtils::ACTION_THEENGS_WIFI_UPDATE)
            {
                if (m_setupSession.mqtt_settings_sent
                    && m_setupSession.wifi_settings_sent
                    && !m_setupSession.wifi_connected)
                {
                    getWifiState();
                }

                if (!m_setupSession.mqtt_settings_sent)
                {
                    setCustomData();
                    m_setupSession.mqtt_settings_sent = true;
                    Q_EMIT sequenceUpdated();
                }

                if (!m_setupSession.wifi_settings_sent)
                {
                    setWifiCredentials();
                    m_setupSession.wifi_settings_sent = true;
                    Q_EMIT sequenceUpdated();

                    setWifiConnect();
                    QTimer::singleShot(1000, this, SLOT(getWifiState()));
                }
            }

            return;
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceGateway::bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceGateway::bleWriteDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceGateway::bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    qDebug() << "DeviceGateway::bleReadDone(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    qDebug() << "DATA: 0x" << value.toHex();
}

void DeviceGateway::bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    //qDebug() << "DeviceGateway::bleReadNotify(" << m_deviceAddress << ") on" << c.name() << " / uuid" << c.uuid() << value.size();
    //qDebug() << "DATA: 0x" << value.toHex();

    // BluFi UUID // BluFi frame size can't be smaller than 4 bytes
    if (c.uuid() == uuid_bluefi_char_read && value.size() >= 4)
    {
        BluFiFrame f(value);
        f.printHeader();
        f.printParsedData();

        uint8_t frametype = 0;
        uint8_t framesubtype = 0;
        f.getFrameType(frametype, framesubtype);

        // handle the response

        if (frametype == BluFiUtils::CTRL_FRAME)
        {
            if (framesubtype == BluFiUtils::SUBTYPE_ACK)
            {
                //
            }
            else
            {
                qWarning() << "UNKNOWN (CTRL) FRAME SUBTYPE:" << framesubtype;
            }
        }
        else if (frametype == BluFiUtils::DATA_FRAME)
        {
            if (framesubtype == BluFiUtils::SUBTYPE_WIFI_CONNECTION_STATE)
            {
                QByteArray data = f.getData();
                if (data.size() >= 2) checkWiFi_state(data.at(1));
            }
            else if (framesubtype == BluFiUtils::SUBTYPE_WIFI_LIST)
            {
                QByteArray data = f.getData();
                if (data.size() >= 2) checkWiFi_list(data);

                m_networksRefreshing = false;
                Q_EMIT networksUpdated();
            }
            else if (framesubtype == BluFiUtils::SUBTYPE_CUSTOM_DATA)
            {
                //
            }
            else if (framesubtype == BluFiUtils::SUBTYPE_ERROR)
            {
                //
            }
            else
            {
                qWarning() << "UNKNOWN (DATA) FRAME SUBTYPE:" << framesubtype;
            }
        }
        else
        {
            qWarning() << "WRONG FRAME TYPE:" << frametype;
        }
    }
}

/* ************************************************************************** */

void DeviceGateway::descriptorRead(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceGateway::descriptorRead()";
}

void DeviceGateway::descriptorWritten(const QLowEnergyDescriptor &, const QByteArray &)
{
    qDebug() << "DeviceGateway::descriptorWritten()";
}

/* ************************************************************************** */

void DeviceGateway::bleError(QLowEnergyService::ServiceError e)
{
    qDebug() << "DeviceGateway::bleError(" << e << ")";
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceGateway::parseAdvertisementData(const uint16_t adv_mode,
                                           const uint16_t adv_id,
                                           const QByteArray &ba)
{
    //qDebug() << "DeviceGateway::parseAdvertisementData(" << m_deviceAddress
    //         << " - " << adv_mode << " - 0x" << adv_id << ")";
    //qDebug() << "DATA (" << ba.size() << "bytes)   >  0x" << ba.toHex();

    if (adv_id == 0xffff && ba.size() == 2)
    {
        const quint8 *data = reinterpret_cast<const quint8 *>(ba.constData());

        int status = data[1];
        if (status >= GatewayState::WAITING_ONBOARDING && status <= GatewayState::ERROR)
        {
            if (m_onboardingStatus != status)
            {
                m_onboardingStatus = status;
                Q_EMIT onboardedUpdated();

                qDebug() << "DeviceGateway::parseAdvertisementData() onboarding status >> " << m_onboardingStatus;
            }
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceGateway::startHandshake()
{
    // TODO // WE DON'T USE ENCRYPTION (YET)
}

void DeviceGateway::setSeqMode()
{
    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::CTRL_FRAME, BluFiUtils::SUBTYPE_SET_SEC_MODE);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, true, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);
        QByteArray d; uint8_t dd=0; d.push_back(dd);
        f.setData(d);

        //f.printDetails();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}
void DeviceGateway::getVersion()
{
    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::CTRL_FRAME, BluFiUtils::SUBTYPE_GET_VERSION);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, true, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        f.printDetails();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}
void DeviceGateway::getWifiState()
{
    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::CTRL_FRAME, BluFiUtils::SUBTYPE_GET_WIFI_STATUS);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        //f.printDetails();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}
void DeviceGateway::getWifiList()
{
    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::CTRL_FRAME, BluFiUtils::SUBTYPE_GET_WIFI_LIST);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, true, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        //f.printDetails();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);

        m_networksRefreshing = true;
        Q_EMIT networksUpdated();
    }
}

void DeviceGateway::setWifiCredentials()
{
    if (m_serviceBluFi)
    {
        BluFiFrame ssid;
        ssid.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_STA_WIFI_SSID);
        ssid.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        ssid.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QByteArray d1(m_setupSession.wifi_ssid.toLatin1());
        ssid.setData(d1);

        //ssid.printDetails();
        //ssid.printParsedData();

        m_serviceBluFi->writeCharacteristic(m_charWrite, ssid.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);

        BluFiFrame pw;
        pw.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_STA_WIFI_PASSWORD);
        pw.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        pw.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QByteArray d2(m_setupSession.wifi_pass.toLatin1());
        pw.setData(d2);

        //pw.printDetails();
        //pw.printParsedData();

        m_serviceBluFi->writeCharacteristic(m_charWrite, pw.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}
void DeviceGateway::setWifiConnect()
{
    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::CTRL_FRAME, BluFiUtils::SUBTYPE_CONNECT_WIFI);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        //f.printDetails();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}
void DeviceGateway::setWifiDisconnect()
{
    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::CTRL_FRAME, BluFiUtils::SUBTYPE_DISCONNECT_WIFI);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        //f.printDetails();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}

void DeviceGateway::setCustomData()
{
    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_CUSTOM_DATA);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QString customdata;
        customdata += "{";
        customdata += "\"customdata\": \"" + m_setupSession.mqtt_topicA + "\",";
        customdata += "\"customdata\": \"" + m_setupSession.mqtt_topicB + "\"";
        customdata += "}";
        qDebug() << customdata << " / size:" << customdata.size();

        QByteArray cd(customdata.toLatin1());
        f.setData(cd);

        //f.printDetails();
        //f.printParsedData();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}

/* ************************************************************************** */

void DeviceGateway::setCustomData_getStatus()
{
    qDebug() << "DeviceGateway::setCustomData_getStatus()" << getAddress() << getName();

    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_CUSTOM_DATA);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QString customdata;
        customdata += "{";
        customdata += "\"target\":\"MQTTtoSYS\",";
        customdata += "\"cmd\":\"status\"";
        //customdata += "\"gw_pass\": \"12345678\",";
        customdata += "}";

        QByteArray cd(customdata.toLatin1());
        f.setData(cd);

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}

/* ************************************************************************** */

void DeviceGateway::setCustomData_setMqtt()
{
    qDebug() << "DeviceGateway::setCustomData_setMqtt()" << getAddress() << getName();

    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_CUSTOM_DATA);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QString customdata;
        customdata += "{";
        customdata += "\"mqtt_server\":\"" + m_setupSession.mqtt_server + "\",";
        customdata += "\"mqtt_port\":\"" + QString::number(m_setupSession.mqtt_port) + "\",";
        customdata += "\"mqtt_user\":\"" + m_setupSession.mqtt_user + "\",";
        customdata += "\"mqtt_pass\":\"" + m_setupSession.mqtt_pass + "\"";
        customdata += "}";
        //qDebug() << customdata << " / size:" << customdata.size();

        QByteArray cd(customdata.toLatin1());
        f.setData(cd);

        f.printDetails();
        f.printParsedData();

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
/*
        {
            "mqtt_topic": "topic/",
            "discovery_prefix": "prefix",
            "gateway_name": "name"
        }
        QString customdata;
        customdata += "{";
        customdata += "\"mqtt_topic\":\"" + m_setupSession.mqtt_topicA + "\",";
        customdata += "\"discovery_prefix\":\"" + m_setupSession.mqtt_topicB + "\",";
        customdata += "\"gateway_name\":\"" + m_setupSession.mqtt_topicB + "\",";
        customdata += "}";
        qDebug() << customdata << " / size:" << customdata.size();
*/
    }
}

/* ************************************************************************** */

void DeviceGateway::setCustomData_setPassword()
{
    qDebug() << "DeviceGateway::setCustomData_setPassword()" << getAddress() << getName();

    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_CUSTOM_DATA);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QString customdata;
        customdata += "{";
        customdata += "\"target\":\"MQTTtoSYS\",";
        customdata += "\"gw_pass\":\"" + m_setupSession.gateway_pass + "\"";
        customdata += "}";

        QByteArray cd(customdata.toLatin1());
        f.setData(cd);

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}

/* ************************************************************************** */

void DeviceGateway::setCustomData_erase()
{
    qDebug() << "DeviceGateway::setCustomData_erase()" << getAddress() << getName();

    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_CUSTOM_DATA);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QString customdata;
        customdata += "{";
        customdata += "\"target\":\"MQTTtoSYS\",";
        customdata += "\"cmd\":\"erase\"";
        //customdata += "\"gw_pass\": \"12345678\",";
        customdata += "}";

        QByteArray cd(customdata.toLatin1());
        f.setData(cd);

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}

/* ************************************************************************** */

void DeviceGateway::setCustomData_restart()
{
    qDebug() << "DeviceGateway::setCustomData_restart()" << getAddress() << getName();

    if (m_serviceBluFi)
    {
        BluFiFrame f;
        f.setFrameType(BluFiUtils::DATA_FRAME, BluFiUtils::SUBTYPE_CUSTOM_DATA);
        f.setFrameCtrl(false, false, BluFiUtils::DIRECTION_OUTPUT, false, false);
        f.setSequenceNumber(m_setupSession.blufi_seq_nb++);

        QString customdata;
        customdata += "{";
        customdata += "\"target\":\"MQTTtoSYS\",";
        customdata += "\"cmd\":\"restart\"";
        //customdata += "\"gw_pass\": \"12345678\",";
        customdata += "}";

        QByteArray cd(customdata.toLatin1());
        f.setData(cd);

        m_serviceBluFi->writeCharacteristic(m_charWrite, f.toByeArray(),
                                            QLowEnergyService::WriteWithResponse);
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceGateway::setGatewayCredentials(const QString &password)
{
    qDebug() << "DeviceGateway::setGatewayCredentials()";

    if (m_setupSession.gateway_pass != password)
    {
        m_setupSession.gateway_pass = password;
    }
}

void DeviceGateway::setWifiCredentials(const QString &ssid, const QString &password)
{
    qDebug() << "DeviceGateway::setCredentials()";

    if (m_setupSession.wifi_ssid != ssid ||
        m_setupSession.wifi_pass != password)
    {
        m_setupSession.wifi_settings_sent = false;
        m_setupSession.wifi_ssid = ssid;
        m_setupSession.wifi_pass = password;
    }
}

void DeviceGateway::setMqttCredentials(const QString &host, const int port,
                                    const QString &user, const QString &password,
                                    const QString &topicA, const QString &topicB)
{
    qDebug() << "DeviceGateway::setCredentials()";

    //if (m_setupSession.mqtt_customeruuid != mqtt_customeruuid ||
    //    m_setupSession.mqtt_iotserver != mqtt_iotserver)
    {
        m_setupSession.mqtt_settings_sent = false;
        m_setupSession.mqtt_server = host;
        m_setupSession.mqtt_port = port;
        m_setupSession.mqtt_user = user;
        m_setupSession.mqtt_pass = password;
        m_setupSession.mqtt_topicA = topicA;
        m_setupSession.mqtt_topicB = topicB;
    }
}

/* ************************************************************************** */

void DeviceGateway::startOnboarding()
{
    if (!isWorking())
    {
        qDebug() << "DeviceBLE::startOnboarding()" << getAddress() << getName();

        m_ble_action = DeviceUtils::ACTION_THEENGS_ONBOARDING;
        m_setupSession.startSequence();
        actionStarted();

        m_setupSession.sequence_status = 1;
        Q_EMIT sequenceUpdated();

        m_setupTimer.start(m_setupSession.s_timout_duration);
        deviceConnect();
    }
}

void DeviceGateway::startWifiUpdate()
{
    if (!isWorking())
    {
        qDebug() << "DeviceBLE::startWifiUpdate()" << getAddress() << getName();

        m_ble_action = DeviceUtils::ACTION_THEENGS_WIFI_UPDATE;
        m_setupSession.startSequence();
        actionStarted();

        m_setupSession.sequence_status = 1;
        Q_EMIT sequenceUpdated();

        m_setupTimer.start(m_setupSession.s_timout_duration);
        deviceConnect();
    }
}

void DeviceGateway::retrySequence()
{
    if (!isWorking())
    {
        qDebug() << "DeviceBLE::retrySequence()" << getAddress() << getName();

        if (m_ble_action == DeviceUtils::ACTION_THEENGS_ONBOARDING)
        {
            qDebug() << "DeviceBLE::retrySequence(onboarding)";
            m_setupSession.retrySequence();
            m_setupTimer.start(m_setupSession.s_timout_duration);
            deviceConnect();
        }
        else if (m_ble_action == DeviceUtils::ACTION_THEENGS_WIFI_UPDATE)
        {
            qDebug() << "DeviceBLE::retrySequence(wifi update)";
            m_setupSession.retrySequence();
            m_setupTimer.start(m_setupSession.s_timout_duration);
            deviceConnect();
        }
        else
        {
            qWarning() << "DeviceBLE::retrySequence(no sequence running)";
        }
    }
}

/* ************************************************************************** */

void DeviceGateway::retrySequence_internal()
{
    qDebug() << "DeviceBLE::retrySequence_internal()";

    if (!isWorking())
    {
        m_setupSession.retrySequence();
        m_setupTimer.start(m_setupSession.s_timout_duration);
        deviceConnect();
    }
}

void DeviceGateway::abortSequence_internal()
{
    qDebug() << "DeviceBLE::abortSequence_internal()";

    if (m_setupSession.sequence_status == 1)
    {
        m_setupSession.sequence_status = -1;
        Q_EMIT sequenceUpdated();
    }

    m_ble_action = DeviceUtils::ACTION_IDLE;

    m_setupTimer.stop();
    deviceDisconnect();
}

void DeviceGateway::timeoutSequence_internal()
{
    if (m_setupSession.sequence_status == 1)
    {
        qDebug() << "DeviceBLE::timeoutSequence_internal()";

        m_setupSession.sequence_status = -2;
        Q_EMIT sequenceUpdated();

        deviceDisconnect();
    }
}

void DeviceGateway::finishSequence_internal()
{
    qDebug() << "DeviceBLE::finishSequence_internal()";

    m_setupSession.sequence_status = 2;
    Q_EMIT sequenceUpdated();
    Q_EMIT sequenceCompleted();

    m_ble_action = DeviceUtils::ACTION_IDLE;

    m_setupTimer.stop();
    deviceDisconnect();
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DeviceGateway::checkWiFi_list(const QByteArray &data)
{
    qDebug() << "DeviceBLE::checkWiFi_list()" << getAddress() << getName();

    // available networks from the device

    if (data.size() >= 2)
    {
        qDeleteAll(m_networksAvailable);
        m_networksAvailable.clear();

        int pos = 0;
        while (pos < data.size())
        {
            int length = data.at(pos);
            pos++;

            int rssi = data.at(pos);
            pos++;

            QString ssid = data.mid(pos, length-1);
            pos += length-1;

            Network *nw = new Network(ssid, rssi, false, this);
            m_networksAvailable.push_back(nw);
        }

        Q_EMIT networksUpdated();
        return true;
    }

    return false;
}

bool DeviceGateway::checkWiFi_state(const uint8_t status)
{
    qDebug() << "DeviceBLE::checkWiFi_state(" << status << ")" << getAddress() << getName();

    // connection state of the STA device

    if (status == BluFiUtils::WIFI_OP_STATE_DISCONNECTED)
    {
        m_setupSession.wifi_status = 0;
        m_setupSession.wifi_connected = false;
        m_setupSession.wifi_errored = true;
        Q_EMIT sequenceUpdated();
    }
    else if (status == BluFiUtils::WIFI_OP_STATE_CONNECTING)
    {
        m_setupSession.wifi_status = 1;
        m_setupSession.wifi_connected = false;
        m_setupSession.wifi_errored = false;
        Q_EMIT sequenceUpdated();
    }
    else if (status == BluFiUtils::WIFI_OP_STATE_CONNECTED)
    {
        m_setupSession.wifi_status = 3;
        m_setupSession.wifi_connected = true;
        m_setupSession.wifi_errored = false;
        Q_EMIT sequenceUpdated();
    }
    else if (status == BluFiUtils::WIFI_OP_STATE_CONNECTED_NO_IP)
    {
        m_setupSession.wifi_status = 2;
        m_setupSession.wifi_connected = true;
        m_setupSession.wifi_errored = true;
        Q_EMIT sequenceUpdated();
    }

    if (m_setupSession.wifi_connected)
    {
        if (m_ble_action == DeviceUtils::ACTION_THEENGS_ONBOARDING)
        {
            createGateway();
        }
        else if (m_ble_action == DeviceUtils::ACTION_THEENGS_WIFI_UPDATE)
        {
            finishSequence_internal();
        }
    }
    else // if (!m_setupSession.wifi_connected)
    {
        // retry later
        QTimer::singleShot(1000, this, SLOT(getWifiState()));
    }

    return m_setupSession.wifi_connected;
}

void DeviceGateway::createGateway()
{
    qDebug() << "DeviceBLE::createGateway()" << getAddress() << getName();

    {
        QString addrclean = m_mac_wifi;
        addrclean = addrclean.remove(QChar(':'), Qt::CaseInsensitive);

        Q_EMIT sequenceUpdated();
    }
}

void DeviceGateway::gatewayCreationResponse(const QString &context, bool created)
{
    qDebug() << "DeviceGateway::gatewayCreationResponse(" << created << ")" << context;

    if (created)
    {
        Q_EMIT sequenceUpdated();

        finishSequence_internal();
    }
    else
    {
        abortSequence_internal();
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceGateway::actionPasswordSet()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionPasswordSet()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_SET_PASSWORD);
        setCustomData_setPassword();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionPasswordSet()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_SET_PASSWORD);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionPasswordSet() BLE status: " << m_ble_status;
    }
}

/* ************************************************************************** */

void DeviceGateway::actionWifiScan()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionWifiScan()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_GET_LIST);
        getWifiList();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionWifiScan()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_GET_LIST);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionWifiScan() BLE status: " << m_ble_status;
    }
}

void DeviceGateway::actionWifiSet()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionWifiSet()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_SET_CREDENTIALS);
        setWifiCredentials();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionWifiSet()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_SET_CREDENTIALS);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionWifiSet() BLE status: " << m_ble_status;
    }
}

void DeviceGateway::actionWifiConnect()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionWifiConnect()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_CONNECT);
        setWifiConnect();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionWifiConnect()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_CONNECT);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionWifiConnect() BLE status: " << m_ble_status;
    }
}

void DeviceGateway::actionWifiDisconnect()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionWifiDisconnect()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_DISCONNECT);
        setWifiDisconnect();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionWifiDisconnect()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_WIFI_DISCONNECT);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionWifiDisconnect() BLE status: " << m_ble_status;
    }
}

/* ************************************************************************** */

void DeviceGateway::actionMqttSet()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionMqttSet()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_SET_MQTT);
        setCustomData_setMqtt();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionMqttSet()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_SET_MQTT);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionMqttSet() BLE status: " << m_ble_status;
    }
}

/* ************************************************************************** */

void DeviceGateway::actionStatus()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionStatus()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_GET_STATUS);
        setCustomData_getStatus();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionStatus()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_GET_STATUS);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionStatus() BLE status: " << m_ble_status;
    }
}

/* ************************************************************************** */

void DeviceGateway::actionErase()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionErase()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_ERASE);
        setCustomData_erase();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionErase()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_ERASE);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionErase() BLE status: " << m_ble_status;
    }
}

/* ************************************************************************** */

void DeviceGateway::actionRestart()
{
    if (m_ble_status == DeviceUtils::DEVICE_CONNECTED)
    {
        qDebug() << "DeviceStodeus::actionRestart()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_RESTART);
        setCustomData_restart();
    }
    else if ((m_ble_status <= DeviceUtils::DEVICE_AVAILABLE))
    {
        qDebug() << "DeviceStodeus::actionRestart()" << getAddress() << getName();
        actionStarted(DeviceUtils::ACTION_BLUFI_CUSTOM_RESTART);
        deviceConnect();
    }
    else
    {
        qWarning() << "DeviceStodeus::actionRestart() BLE status: " << m_ble_status;
    }
}

/* ************************************************************************** */
/* ************************************************************************** */
