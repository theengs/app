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

                //
                if (manufacturerIds.size() > i)
                {
                    const auto id = manufacturerIds.at(i);

                    //qDebug() << info.name() << info.address() << ">  Manufacturer Data >"
                    //         << "/ ID" << Qt::hex << Qt::showbase << id
                    //         << "/" << Qt::dec << info.manufacturerData(id).size() << "bytes"
                    //         << "/" << info.manufacturerData(id).toHex();

                    dd->parseAdvertisementData(DeviceUtils::BLE_ADV_MANUFACTURERDATA,
                                               id, info.manufacturerData(id));

                    doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).rightJustified(4, '0').toStdString() + info.manufacturerData(id).toHex().toStdString();
                }

                //
                if (serviceIds.size() > i)
                {
                    const auto id = serviceIds.at(i);

                    //qDebug() << info.name() << info.address() << ">  Service Data >"
                    //         << "/ ID" << Qt::hex << Qt::showbase << id.toUInt16()
                    //         << "/" << Qt::dec << info.serviceData(id).size() << "bytes"
                    //         << "/" << info.serviceData(id).toHex();

                    dd->parseAdvertisementData(DeviceUtils::BLE_ADV_SERVICEDATA,
                                               id.toUInt16(), info.serviceData(id));

                    doc["servicedata"] = info.serviceData(id).toHex().toStdString();
                    doc["servicedatauuid"] = QByteArray::number(id.toUInt16(), 16).rightJustified(4, '0').toStdString();
                }

                //
                TheengsDecoder dec;
                ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();
                if (dec.decodeBLEJson(obj) >= 0)
                {
                    obj.remove("manufacturerdata");
                    obj.remove("servicedata");
                    obj.remove("servicedatauuid");

                    std::string output;
                    serializeJson(obj, output);
                    //qDebug() << "decodeBLEJson(known) output:" << output.c_str();

                    // Do not process devices with random macs or IBEACONS packets
                    if (doc["type"] == "RMAC" || doc["prmac"] || doc["model_id"] == "IBEACON") break;

                    dd->setTheengsModelId(QString::fromStdString(doc["model"]), QString::fromStdString(doc["model_id"]));

                    DeviceTheengs *ddd = dynamic_cast<DeviceTheengs*>(dd);
                    if (ddd) ddd->parseTheengsAdvertisement(QString::fromStdString(output));

                    // We need a valid MAC address to send MQTT data
                    // If available, use MAC address decoded from advertisement packets
                    if (mac_qstr.isEmpty())
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
                            qWarning() << "MQTT publishData(" << topic << ") FAILED";
                            qDebug() << "- data:" << output;
                        }
                    }

                    status_device = true;
                }
                else
                {
                    std::string input;
                    serializeJson(doc, input);
                    qWarning() << "decodeBLEJson(known) error:" << input.c_str();
                }
            }

            // Dynamic updates
            if (m_listening)
            {
                if (!dd->isEnabled()) return;
                if (!dd->hasBluetoothConnection()) return;

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

            break;
        }
    }

    /// UN-KNOWN DEVICES ///////////////////////////////////////////////////////

    if (!status_device)
    {
        QString mac_qstr = info.address().toString();
        QString mac_qstr_clean = info.address().toString().remove(':');

        const QList<quint16> &manufacturerIds = info.manufacturerIds();
        const QList<QBluetoothUuid> &serviceIds = info.serviceIds();

        int maxLoop = std::max(manufacturerIds.size(), serviceIds.size());

        for (int i = 0; i < maxLoop; i++)
        {
            ArduinoJson::DynamicJsonDocument doc(4096);
            doc["id"] = mac_qstr.toStdString();
            doc["name"] = info.name().toStdString();
            doc["rssi"] = info.rssi();

            if (manufacturerIds.size() > i)
            {
                const auto id = manufacturerIds.at(i);
                doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).rightJustified(4, '0').toStdString() + info.manufacturerData(id).toHex().toStdString();
            }

            if (serviceIds.size() > i)
            {
                const auto id = serviceIds.at(i);
                doc["servicedata"] = info.serviceData(id).toHex().toStdString();
                doc["servicedatauuid"] = QByteArray::number(id.toUInt16(), 16).rightJustified(4, '0').toStdString();
            }

            TheengsDecoder dec;
            ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();

            if (dec.decodeBLEJson(obj) >= 0)
            {
                obj.remove("manufacturerdata");
                obj.remove("servicedata");
                obj.remove("servicedatauuid");

                std::string output;
                serializeJson(obj, output);
                //qDebug() << "decodeBLEJson(unknown) output:" << output.c_str();

                // Do not process devices with random macs
                if (!(doc["type"] == "RMAC" || doc["prmac"]))
                {
                    // We need a valid MAC address to send MQTT data
                    // If available, use MAC address decoded from advertisement packets
                    if (mac_qstr.isEmpty())
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
                        status_device = mq->publishData(topic, QString::fromStdString(output));
                    }

                    status_device = true;
                }
            }
            else
            {
                //std::string input;
                //serializeJson(doc, input);
                //qWarning() << "decodeBLEJson(unknown) error:" << input.c_str();
            }
        }
    }

    /// Dynamic scanning ///////////////////////////////////////////////////////

    if (m_scanning)
    {
        if (status_gateway && info.name().startsWith("OMG_"))
        {
            //qDebug() << "addBleGateway(" << info.name() << ") FROM DYNAMIC SCANNING";
            addBleGateway(info);
        }
        else if (status_device)
        {
            //qDebug() << "addBleDevice(" << info.name() << ") FROM DYNAMIC SCANNING";
            addBleDevice(info);
        }
    }
}

/* ************************************************************************** */
