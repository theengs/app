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

#include "DeviceManager.h"
#include "SettingsManager.h"
#include "MqttManager.h"

#include "utils_bits.h"

#include <decoder.h> // Theengs decoder
#include "device_theengs.h"

#include <QBluetoothDeviceInfo>
#include <QList>
#include <QDebug>

/* ************************************************************************** */

// On Android, ``QBluetoothDeviceInfo::address()`` may surface a privacy-
// rotated random address rather than the BLE peer's actual public MAC.
// Several Theengs decoder rules (TPMS, VCH6003, IBT-2X(S), MB/SW, ...) require
// the BLE source MAC to equal a MAC embedded in the advertisement payload
// (``mac@index`` / ``revmac@index`` conditions). When the OS rewrites the
// source address, those equality checks fail and the decoder rejects an
// otherwise-valid frame, so the App publishes nothing — even though OMG
// (NimBLE on ESP32) decodes the same vector fine.
//
// Rather than touch the vendored decoder (shared with OMG), retry the decode
// with each plausible 6-byte (12-hex-char) window of the manufacturer data
// substituted in as ``id``. The decoder still has to satisfy all of the
// rule's other conditions (manufacturer-data length, fixed tag bytes, ...),
// so the only window that can win is the one the rule's ``mac@index`` /
// ``revmac@index`` already points at — i.e. the actual embedded MAC.
//
// The proper upstream fix lives in the decoder rules themselves (accept
// payload-extracted MAC when source MAC is privacy-rotated). Once that
// lands, this call-site retry can be removed.
static QString hex_window_to_mac_qstr(const std::string &hex, int byte_off)
{
    // hex is lowercase hex chars, 2 per byte. byte_off is the byte offset.
    if (hex.size() < (size_t)(byte_off * 2 + 12)) return QString();
    QString mac;
    mac.reserve(17);
    for (int b = 0; b < 6; ++b) {
        mac.append(QChar(hex[byte_off * 2 + b * 2]));
        mac.append(QChar(hex[byte_off * 2 + b * 2 + 1]));
        if (b < 5) mac.append(':');
    }
    return mac.toUpper();
}

/* ************************************************************************** */

void DeviceManager::bleDevice_discovered(const QBluetoothDeviceInfo &info)
{
    //qDebug() << "bleDevice_discovered() " << info.name() << info.address(); // << info.deviceUuid();
    bleDevice_updated(info, QBluetoothDeviceInfo::Field::None);
}

/* ************************************************************************** */

void DeviceManager::bleDevice_updated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    //qDebug() << "bleDevice_updated() " << info.name() << info.address(); // << info.deviceUuid() // << " updatedFields: " << updatedFields
    Q_UNUSED(updatedFields) // We don't use QBluetoothDeviceInfo::Fields, it's unreliable

    bool status_device = false;
    bool status_gateway = false;

#if !defined(DEBUG_FAKE_DEVICES)
    if (info.rssi() >= 0) return; // we probably just hit the device cache
    //if ((info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) == false) return; // not a BLE device
#endif

    if (m_devices_blacklist.contains(info.address().toString())) return; // device MAC is blacklisted
    if (m_devices_blacklist.contains(info.deviceUuid().toString())) return; // device UUID is blacklisted

    /// KNOWN GATEWAYS /////////////////////////////////////////////////////////

    for (auto d: std::as_const(m_gateways_model->m_devices))
    {
        DeviceGateway *dd = qobject_cast<DeviceGateway *>(d);

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
        if (dd && dd->getAddress() == info.deviceUuid().toString())
#else
        if (dd && dd->getAddress() == info.address().toString())
#endif
        {
            dd->setName(info.name());
            dd->setRssi(info.rssi());

            const QList<quint16> &manufacturerIds = info.manufacturerIds();
            for (const auto id: manufacturerIds)
            {
                //qDebug() << info.name() << info.address() << ">  Manufacturer Data >"
                //         << "/ ID" << Qt::hex << Qt::showbase << id
                //         << "/" << Qt::dec << info.manufacturerData(id).size() << "bytes"
                //         << "/" << info.manufacturerData(id).toHex();

                dd->parseAdvertisementData(DeviceUtils::BLE_ADV_MANUFACTURERDATA,
                                           id, info.manufacturerData(id));

                status_gateway = true;
            }

            break;
        }
    }

    /// KNOWN DEVICES //////////////////////////////////////////////////////////

    for (auto d: std::as_const(m_devices_model->m_devices))
    {
        Device *dd = qobject_cast<Device*>(d);

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
        if (dd && dd->getAddress() == info.deviceUuid().toString())
#else
        if (dd && dd->getAddress() == info.address().toString())
#endif
        {
            dd->setName(info.name());
            dd->setRssi(info.rssi());

            QString mac_qstr = dd->getAddressMAC();
            QString mac_qstr_clean = dd->getAddressMAC().remove(':');

            if (mac_qstr.isEmpty() || mac_qstr_clean.isEmpty())
            {
                mac_qstr = "00:00:00:00:00:00";
                mac_qstr_clean = "000000000000";
            }

            // Handle advertisement //

            const QList<quint16> &manufacturerIds = info.manufacturerIds();
            const QList<QBluetoothUuid> &serviceIds = info.serviceIds();

            int maxLoop = std::max(manufacturerIds.size(), serviceIds.size());

            for (int i = 0; i < maxLoop; i++)
            {
                ArduinoJson::DynamicJsonDocument doc(4096);
                doc["id"] = mac_qstr.toStdString();
                doc["name"] = info.name().toStdString();
                doc["rssi"] = info.rssi();

                // manufacturer data
                if (manufacturerIds.size() > i)
                {
                    const auto id = manufacturerIds.at(i);
                    doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).rightJustified(4, '0').toStdString() + info.manufacturerData(id).toHex().toStdString();

                    //qDebug() << info.name() << info.address() << ">  Manufacturer Data >"
                    //         << "/ ID" << Qt::hex << Qt::showbase << id
                    //         << "/" << Qt::dec << info.manufacturerData(id).size() << "bytes"
                    //         << "/" << info.manufacturerData(id).toHex();

                    dd->parseAdvertisementData(DeviceUtils::BLE_ADV_MANUFACTURERDATA,
                                               id, info.manufacturerData(id));
                }

                // service data
                if (serviceIds.size() > i)
                {
                    const auto id = serviceIds.at(i);
                    doc["servicedata"] = info.serviceData(id).toHex().toStdString();
                    doc["servicedatauuid"] = QByteArray::number(id.toUInt16(), 16).rightJustified(4, '0').toStdString();

                    //qDebug() << info.name() << info.address() << ">  Service Data >"
                    //         << "/ ID" << Qt::hex << Qt::showbase << id.toUInt16()
                    //         << "/" << Qt::dec << info.serviceData(id).size() << "bytes"
                    //         << "/" << info.serviceData(id).toHex();

                    dd->parseAdvertisementData(DeviceUtils::BLE_ADV_SERVICEDATA,
                                               id.toUInt16(), info.serviceData(id));
                }

                // theengs decoding
                TheengsDecoder decoder;
                ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();
                if (decoder.decodeBLEJson(obj) >= 0)
                {
                    dd->setTheengsModelId(QString::fromStdString(doc["model"]), QString::fromStdString(doc["model_id"]));

                    // Do not process devices with random macs or IBEACONS packets
                    if (doc["type"] == "RMAC" || doc["prmac"] || doc["model_id"] == "IBEACON") break;

                    obj.remove("manufacturerdata");
                    obj.remove("servicedata");
                    obj.remove("servicedatauuid");

                    std::string output;
                    serializeJson(obj, output);
                    //qDebug() << "decodeBLEJson(known) output:" << output.c_str();

                    DeviceTheengs *ddd = dynamic_cast<DeviceTheengs*>(dd);
                    if (ddd) ddd->setTheengsModelId(QString::fromStdString(doc["model"]), QString::fromStdString(doc["model_id"]));
                    if (ddd) ddd->parseTheengsAdvertisement(QString::fromStdString(output));

                    // We need a valid MAC address, so if needed use the one from the decoder
                    if (mac_qstr.isEmpty() && obj.containsKey("mac"))
                    {
                        mac_qstr = QString::fromStdString(obj["mac"]);
                        mac_qstr_clean = mac_qstr.remove(':');
                    }

                    // MQTT send
                    SettingsManager *sm = SettingsManager::getInstance();
                    MqttManager *mq = MqttManager::getInstance();
                    if (sm && mq && !mac_qstr_clean.isEmpty())
                    {
                        QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT/" + mac_qstr_clean;
                        bool status_mqtt = mq->publishData(topic, QString::fromStdString(output));
                        if (!status_mqtt)
                        {
                            //qWarning() << "MQTT publishData(" << topic << ")  FAILED  >> " << output;
                        }
                    }

                    status_device = true;
                }
                else
                {
                    std::string output;
                    serializeJson(doc, output);
                    qWarning() << "decodeBLEJson(known) error:" << output.c_str();
                }
            }

            // Dynamic updates
            if (m_listening)
            {
                if (dd->isEnabled() && dd->hasBluetoothConnection())
                {
                    //qDebug() << "adding from bleDevice_updated()";
                    //qDebug() << "last upd" << dd->getLastUpdateInt() << dd->needsUpdateRt();
                    //qDebug() << "last err" << dd->getLastErrorInt() << dd->isErrored();

                    // old or no data: go for refresh
                    // also, check if we didn't already fail to update in the last couple minutes
                    if (dd->needsUpdateRt() && !dd->isErrored())
                    {
                        if (!m_devices_updating_queue.contains(dd) && !m_devices_updating.contains(dd))
                        {
                            m_devices_updating_queue.push_back(dd);
                            dd->refreshQueued();
                            refreshDevices_continue();
                        }
                    }
                }
            }

            break;
        }
    }

    /// UN-KNOWN DEVICES ///////////////////////////////////////////////////////

    if (!status_device)
    {
        QString mac_qstr = info.address().toString();
        QString mac_qstr_clean = info.address().toString().remove(':');

        if (mac_qstr.isEmpty() || mac_qstr_clean.isEmpty())
        {
            mac_qstr = "00:00:00:00:00:00";
            mac_qstr_clean = "000000000000";
        }

        const QList<quint16> &manufacturerIds = info.manufacturerIds();
        const QList<QBluetoothUuid> &serviceIds = info.serviceIds();

        int maxLoop = std::max(manufacturerIds.size(), serviceIds.size());

        for (int i = 0; i < maxLoop; i++)
        {
            std::string mfg_hex;
            std::string svc_hex;
            std::string svc_uuid;

            if (manufacturerIds.size() > i)
            {
                const auto id = manufacturerIds.at(i);
                mfg_hex = QByteArray::number(endian_flip_16(id), 16).rightJustified(4, '0').toStdString() + info.manufacturerData(id).toHex().toStdString();
            }
            if (serviceIds.size() > i)
            {
                const auto id = serviceIds.at(i);
                svc_hex = info.serviceData(id).toHex().toStdString();
                svc_uuid = QByteArray::number(id.toUInt16(), 16).rightJustified(4, '0').toStdString();
            }

            // Build the list of candidate ``id`` values to try against the
            // decoder. We always start with the OS-reported BLE address; if
            // that fails (likely because Android privacy-rotated the address),
            // we retry with each 6-byte window of the manufacturer data so
            // that ``mac@index`` / ``revmac@index`` rules can still match
            // against the payload-embedded MAC.
            QList<QString> id_candidates;
            id_candidates.append(mac_qstr);
            for (size_t off = 0; off + 6 <= mfg_hex.size() / 2; ++off) {
                QString cand = hex_window_to_mac_qstr(mfg_hex, (int)off);
                if (!cand.isEmpty() && cand != mac_qstr) id_candidates.append(cand);
            }

            bool decoded = false;
            for (int attempt = 0; attempt < id_candidates.size() && !decoded; ++attempt)
            {
                ArduinoJson::DynamicJsonDocument doc(4096);
                doc["id"] = id_candidates.at(attempt).toStdString();
                doc["name"] = info.name().toStdString();
                doc["rssi"] = info.rssi();
                if (!mfg_hex.empty()) doc["manufacturerdata"] = mfg_hex;
                if (!svc_hex.empty()) {
                    doc["servicedata"] = svc_hex;
                    doc["servicedatauuid"] = svc_uuid;
                }

                TheengsDecoder decoder;
                ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();
                if (decoder.decodeBLEJson(obj) < 0) continue;

                decoded = true;

                // Do not process devices with random macs
                if (doc["type"] == "RMAC" || doc["prmac"]) break;

                obj.remove("manufacturerdata");
                obj.remove("servicedata");
                obj.remove("servicedatauuid");

                std::string output;
                serializeJson(obj, output);
                //qDebug() << "decodeBLEJson(unknown) output:" << output.c_str();

                // Publish under the OS-reported MAC by default, matching
                // pre-existing behavior so Home Assistant topic subscriptions
                // stay stable across upgrades. HIL_BENCH_MODE prefers the
                // decoder-extracted embedded MAC instead — needed on the bench
                // because the brute-force candidate above may have decoded
                // under a synthetic id (the OS MAC didn't match the rule), and
                // because it stabilizes topics across Android privacy-address
                // rotations on the bench phone.
                QString publish_mac_clean = mac_qstr_clean;
#ifdef HIL_BENCH_MODE
                if (obj.containsKey("mac")) {
                    QString embedded = QString::fromStdString(obj["mac"].as<std::string>());
                    QString embedded_clean = embedded;
                    embedded_clean.remove(':');
                    if (!embedded_clean.isEmpty()) publish_mac_clean = embedded_clean;
                }
#endif

                // MQTT send
                SettingsManager *sm = SettingsManager::getInstance();
                MqttManager *mq = MqttManager::getInstance();
                if (sm && mq && !publish_mac_clean.isEmpty())
                {
                    QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT/" + publish_mac_clean;
                    bool status_mqtt = mq->publishData(topic, QString::fromStdString(output));
                    if (!status_mqtt)
                    {
                        //qWarning() << "MQTT publishData(" << topic << ")  FAILED  >> " << output;
                    }
                }
            }

            if (!decoded)
            {
                //std::string output;
                //serializeJson(doc, output);
                //qWarning() << "decodeBLEJson(unknown) error:" << output.c_str();
            }
        }
    }

    /// Dynamic scanning ///////////////////////////////////////////////////////

    if (m_scanning)
    {
        if (!status_gateway && info.name().startsWith("OMG_"))
        {
            //qDebug() << "addBleGateway(" << info.name() << ") FROM DYNAMIC SCANNING";
            addBleGateway(info);
        }
        else if (!status_device)
        {
            //qDebug() << "addBleDevice(" << info.name() << ") FROM DYNAMIC SCANNING";
            addBleDevice(info);
        }
    }
}

/* ************************************************************************** */
