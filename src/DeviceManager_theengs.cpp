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

#include "utils_bits.h"

#include <decoder.h> // Theengs decoder

#include "device_theengs.h"
#include "devices/device_theengs_generic.h"
#include "devices/device_theengs_probes.h"
#include "devices/device_theengs_scales.h"
#include "devices/device_theengs_motionsensors.h"
#include "devices/device_theengs_thermometers.h"

#include <string>

#include <QBluetoothUuid>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>

#include <QSqlQuery>

#include <QList>
#include <QDebug>

/* ************************************************************************** */
/* ************************************************************************** */

Device * DeviceManager::createTheengsDevice_fromDb(const QString &deviceName_bluetooth,
                                                   QString &deviceModelID_theengs,
                                                   const QString &deviceAddr)
{
    qDebug() << "createTheengsDevice_fromDb(" << deviceName_bluetooth << "/" << deviceModelID_theengs << "/" << deviceAddr << ")";

    // Device model ID migration?
    bool migration = false;
    if (deviceModelID_theengs == "IBT-2X") { deviceModelID_theengs = "IBT-2X(S)"; migration = true; }
    else if (deviceModelID_theengs == "IBT-4XS") { deviceModelID_theengs = "IBT-4X(S/C)"; migration = true; }
    else if (deviceModelID_theengs == "IBT-6XS") { deviceModelID_theengs = "IBT-6XS/SOLIS-6"; migration = true; }
    else if (deviceModelID_theengs == "SOLIS-6") { deviceModelID_theengs = "IBT-6XS/SOLIS-6"; migration = true; }
    else if (deviceModelID_theengs == "BM_V23") { deviceModelID_theengs = "TD3in1"; migration = true; }
    else if (deviceModelID_theengs == "CGD1") { deviceModelID_theengs = "CGC1/CGD1"; migration = true; }
    else if (deviceModelID_theengs == "H5072") { deviceModelID_theengs = "H5072/75"; migration = true; }
    else if (deviceModelID_theengs == "H5075") { deviceModelID_theengs = "H5072/75"; migration = true; }
    else if (deviceModelID_theengs == "H5102") { deviceModelID_theengs = "H5101/02/74/77"; migration = true; }
    else if (deviceModelID_theengs == "IBS-TH1") { deviceModelID_theengs = "IBS-TH1/TH2/P01B"; migration = true; }
    else if (deviceModelID_theengs == "IBS-TH2/P01B") { deviceModelID_theengs = "IBS-TH1/TH2/P01B"; migration = true; }
    else if (deviceModelID_theengs == "TP357/8") { deviceModelID_theengs = "TP35X/393"; migration = true; }

    // Device loading
    DeviceTheengs *device = nullptr;
    QString deviceTags_theengs = QString::fromUtf8(TheengsDecoder().getTheengAttribute(deviceModelID_theengs.toLatin1(), "tag"));
    QString deviceProps_theengs = QString::fromUtf8(TheengsDecoder().getTheengProperties(deviceModelID_theengs.toLatin1()));
    int deviceType = DeviceTheengs::getTheengsTypeFromTag(deviceTags_theengs, "");

    if (!deviceModelID_theengs.isEmpty() && !deviceProps_theengs.isEmpty())
    {
        if (deviceType == DeviceUtils::DEVICE_THEENGS_PROBE)
        {
            device = new DeviceTheengsProbes(deviceAddr, deviceName_bluetooth,
                                             deviceModelID_theengs, deviceProps_theengs, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_SCALE)
        {
            device = new DeviceTheengsScales(deviceAddr, deviceName_bluetooth,
                                             deviceModelID_theengs, deviceProps_theengs, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_MOTIONSENSOR)
        {
            device = new DeviceTheengsMotionSensors(deviceAddr, deviceName_bluetooth,
                                                    deviceModelID_theengs, deviceProps_theengs, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_THERMOMETER)
        {
            device = new DeviceTheengsThermometers(deviceAddr, deviceName_bluetooth,
                                                   deviceModelID_theengs, deviceProps_theengs, this);
        }
        else
        {
            device = new DeviceTheengsGeneric(deviceAddr, deviceName_bluetooth,
                                              deviceModelID_theengs, deviceProps_theengs, this);
        }

        if (migration)
        {
            // Write new model ID
            device->changeTheengsModelId("", deviceModelID_theengs);
        }
    }
    else
    {
        qWarning() << "Unknown device model: " << deviceName_bluetooth << deviceModelID_theengs << deviceProps_theengs;
    }

    return device;
}

/* ************************************************************************** */

Device * DeviceManager::createTheengsDevice_fromAdv(const QBluetoothDeviceInfo &deviceInfo)
{
    qDebug() << "createTheengsDevice_fromAdv(" << deviceInfo.name() << ")";

    DeviceTheengs *device = nullptr;

    QString deviceModel;
    QString deviceModelID;
    QString deviceTags;
    QString deviceTypes;
    QString deviceProps;

    const QList<quint16> &manufacturerIds = deviceInfo.manufacturerIds();
    for (const auto id: manufacturerIds)
    {
        if (deviceModelID.isEmpty() == false) break;

        ArduinoJson::DynamicJsonDocument doc(4096);
        doc["id"] = deviceInfo.address().toString().toStdString();
        doc["name"] = deviceInfo.name().toStdString();
        doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).rightJustified(4, '0').toStdString() + deviceInfo.manufacturerData(id).toHex().toStdString();

        TheengsDecoder dec;
        ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();

        if (dec.decodeBLEJson(obj) >= 0)
        {
            deviceModel = QString::fromStdString(doc["model"]);
            deviceModelID = QString::fromStdString(doc["model_id"]);
            deviceTags = QString::fromStdString(doc["tag"]);
            deviceTypes = QString::fromStdString(doc["type"]);
            deviceProps = QString::fromStdString(dec.getTheengProperties(deviceModelID.toLatin1()));

            // Do not process devices with random macs
            if (deviceTypes == "RMAC") continue;

            qDebug() << "addDevice() FOUND [mfd] :" << deviceModel << deviceModelID << deviceTags << deviceTypes << deviceProps;
            break;
        }
        else
        {
            std::string input;
            serializeJson(doc, input);
            qDebug() << "decodeBLEJson(mfd_add) error:" << input.c_str();
        }
    }

    const QList<QBluetoothUuid> &serviceIds = deviceInfo.serviceIds();
    for (const auto id: serviceIds)
    {
        if (deviceModelID.isEmpty() == false) break;

        ArduinoJson::DynamicJsonDocument doc(4096);
        doc["id"] = deviceInfo.address().toString().toStdString();
        doc["name"] = deviceInfo.name().toStdString();
        doc["servicedata"] = deviceInfo.serviceData(id).toHex().toStdString();
        doc["servicedatauuid"] = QByteArray::number(id.toUInt16(), 16).rightJustified(4, '0').toStdString();

        TheengsDecoder dec;
        ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();

        if (dec.decodeBLEJson(obj) >= 0)
        {
            deviceModel = QString::fromStdString(doc["model"]);
            deviceModelID = QString::fromStdString(doc["model_id"]);
            deviceTags = QString::fromStdString(doc["tag"]);
            deviceTypes = QString::fromStdString(doc["type"]);
            deviceProps = QString::fromStdString(dec.getTheengProperties(deviceModelID.toLatin1()));

            // Do not process devices with random macs
            if (deviceTypes == "RMAC") continue;

            qDebug() << "addDevice() FOUND [svd] :" << deviceModel << deviceModelID << deviceTags << deviceTypes << deviceProps;
            break;
        }
        else
        {
            std::string input;
            serializeJson(doc, input);
            qDebug() << "decodeBLEJson(svd_add) error:" << input.c_str();
        }
    }

    if ((!deviceModelID.isEmpty() && !deviceProps.isEmpty()))
    {
        int deviceType = DeviceTheengs::getTheengsTypeFromTag(deviceTags, deviceTypes);

        if (deviceType == DeviceUtils::DEVICE_THEENGS_PROBE)
        {
            device = new DeviceTheengsProbes(deviceInfo, deviceModelID, deviceProps, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_SCALE)
        {
            device = new DeviceTheengsScales(deviceInfo, deviceModelID, deviceProps, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_MOTIONSENSOR)
        {
            device = new DeviceTheengsMotionSensors(deviceInfo, deviceModelID, deviceProps, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_THERMOMETER)
        {
            device = new DeviceTheengsThermometers(deviceInfo, deviceModelID, deviceProps, this);
        }
        else
        {
            device = new DeviceTheengsGeneric(deviceInfo, deviceModelID, deviceProps, this);
        }
/*
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_SMARTWATCH)
        {
            device = new DeviceTheengsWatches(deviceAddr, deviceName,
                                              deviceModel_theengs, device_props, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_THEENGS_BEACON)
        {
            device = new DeviceTheengsBeacons(deviceAddr, deviceName,
                                              deviceModel_theengs, device_props, this);
        }
        else if (deviceType == DeviceUtils::DEVICE_ENVIRONMENTAL)
        {
            device = new DeviceTheengsGeneric(deviceInfo, device_modelid_theengs, device_props, this);
            device->setEnvironmental();
        }
        else if (deviceType == DeviceUtils::DEVICE_PLANTSENSOR)
        {
            device = new DeviceTheengsGeneric(deviceInfo, device_modelid_theengs, device_props, this);
            device->setPlantSensor();
        }
*/
        if (!device->isValid())
        {
            qWarning() << "Device is invalid:" << deviceInfo.name();
            delete device;
            device = nullptr;
        }
    }

    if (!device)
    {
        qWarning() << "Couldn't add device:" << deviceInfo.name();
    }

    return device;
}

/* ************************************************************************** */

QString DeviceManager::getDeviceModelIdTheengs_fromAdv(const QBluetoothDeviceInfo &deviceInfo)
{
    //qDebug() << "getDeviceModelIdTheengs_fromAdv(" << deviceInfo.name() << ")";

    const QList<quint16> &manufacturerIds = deviceInfo.manufacturerIds();
    for (const auto id: manufacturerIds)
    {
        ArduinoJson::DynamicJsonDocument doc(4096);
        doc["name"] = deviceInfo.name().toStdString();
        doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).rightJustified(4, '0').toStdString() + deviceInfo.manufacturerData(id).toHex().toStdString();

        TheengsDecoder dec;
        ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();

        if (dec.decodeBLEJson(obj) >= 0)
        {
            QString model = QString::fromStdString(doc["model"]);
            QString modelId = QString::fromStdString(doc["model_id"]);
            QString deviceTypes = QString::fromStdString(doc["type"]);

            // Do not process devices with random macs
            if (deviceTypes == "RMAC") continue;
            if (modelId.isEmpty()) continue;

            return modelId;
        }
    }

    const QList<QBluetoothUuid> &serviceIds = deviceInfo.serviceIds();
    for (const auto id: serviceIds)
    {
        ArduinoJson::DynamicJsonDocument doc(4096);
        doc["name"] = deviceInfo.name().toStdString();
        doc["servicedata"] = deviceInfo.serviceData(id).toHex().toStdString();
        doc["servicedatauuid"] = id.toString(QUuid::Id128).toStdString();

        TheengsDecoder dec;
        ArduinoJson::JsonObject obj = doc.as<ArduinoJson::JsonObject>();

        if (dec.decodeBLEJson(obj) >= 0)
        {
            QString model = QString::fromStdString(doc["model"]);
            QString modelId = QString::fromStdString(doc["model_id"]);
            QString deviceTypes = QString::fromStdString(doc["type"]);

            // Do not process devices with random macs
            if (deviceTypes == "RMAC") continue;
            if (modelId.isEmpty()) continue;

            return modelId;
        }
    }

    return QString();
}

/* ************************************************************************** */

QString DeviceManager::getDeviceBrandTheengs(const QString &modelid)
{
    return QString::fromUtf8(TheengsDecoder().getTheengAttribute(modelid.toLatin1(), "brand"));
}
QString DeviceManager::getDeviceModelTheengs(const QString &modelid)
{
    return QString::fromUtf8(TheengsDecoder().getTheengAttribute(modelid.toLatin1(), "model"));
}
QString DeviceManager::getDeviceTagTheengs(const QString &modelid)
{
    return QString::fromUtf8(TheengsDecoder().getTheengAttribute(modelid.toLatin1(), "tag"));
}
QString DeviceManager::getDevicePropsTheengs(const QString &modelid)
{
    return QString::fromUtf8(TheengsDecoder().getTheengProperties(modelid.toLatin1()));
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceManager::discoverTheengsDevices()
{
    //qDebug() << "discoverTheengsDevices()";

    // Load saved devices and sent discovery requests to the MQTT broker
    QSqlQuery queryDevices;
    if (queryDevices.exec("SELECT deviceName, deviceModel, deviceAddr, deviceAddrMAC FROM devices"))
    {
        while (queryDevices.next())
        {
            QString deviceName = queryDevices.value(0).toString();
            QString deviceModel_theengs = queryDevices.value(1).toString();
            QString deviceManufacturer_theengs = getDeviceBrandTheengs(deviceModel_theengs);
            QString deviceAddr = queryDevices.value(2).toString();
#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
            deviceAddr = queryDevices.value(3).toString();
#endif
            QString device_props = getDevicePropsTheengs(deviceModel_theengs);

            DeviceTheengs::createDiscoveryMQTT(deviceAddr, deviceName, deviceModel_theengs,
                                               deviceManufacturer_theengs, device_props,
                                               m_bluetoothAdapter->address().toString());
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceManager::fakeTheengsDevices()
{
    // Theengs fake data ///////////////////////////////////////////////////////
    {
        connect(&m_faker, &QTimer::timeout, this, &DeviceManager::fakeTheengsData);
        m_faker.setInterval(1*100); // 100 ms
        m_faker.start();
    }

    // Theengs fake devices ////////////////////////////////////////////////////
    {
        QString deviceName = "JQJCY01YM";
        QString deviceModel_theengs = "JQJCY01YM";
        QString deviceAddr = "11:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "CGDN1";
        QString deviceModel_theengs = "CGDN1";
        QString deviceAddr = "11:57:43:01:5C:3B";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "CGP1W";
        QString deviceModel_theengs = "CGP1W";
        QString deviceAddr = "11:57:43:01:5C:3C";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "GVH5106_4313";
        QString deviceModel_theengs = "H5106";
        QString deviceAddr = "11:57:43:01:5C:3D";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }


    {
        QString deviceName = "TPMS1_10CA8F";
        QString deviceModel_theengs = "TPMS";
        QString deviceAddr = "21:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "GVH5055";
        QString deviceModel_theengs = "H5055";
        QString deviceAddr = "22:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "iBBQ";
        QString deviceModel_theengs = "IBT-2X(S)";
        QString deviceAddr = "23:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "iBBQ";
        QString deviceModel_theengs = "IBT-4X(S/C)";
        QString deviceAddr = "24:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "iBBQ";
        QString deviceModel_theengs = "IBT-6XS/SOLIS-6";
        QString deviceAddr = "25:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }


    {
        QString deviceName = "Mi_Smart_Scale";
        QString deviceModel_theengs = "XMTZC01HM/XMTZC04HM";
        QString deviceAddr = "31:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "Mi_Body_Composition_Scale";
        QString deviceModel_theengs = "XMTZC02HM/XMTZC05HM";
        QString deviceAddr = "32:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }


    {
        QString deviceName = "GVH5072_1234";
        QString deviceModel_theengs = "H5072/75";
        QString deviceAddr = "41:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "GVH5075_1234";
        QString deviceModel_theengs = "H5072/75";
        QString deviceAddr = "42:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "GVH5102_1234";
        QString deviceModel_theengs = "H5101/02/74/77";
        QString deviceAddr = "43:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "V23";
        QString deviceModel_theengs = "TD3in1";
        QString deviceAddr = "44:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "sps";
        QString deviceModel_theengs = "IBS-TH1/TH2/P01B";
        QString deviceAddr = "45:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "tps";
        QString deviceModel_theengs = "IBS-TH1/TH2/P01B";
        QString deviceAddr = "46:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }


    {
        QString deviceName = "CGPR1";
        QString deviceModel_theengs = "CGPR1";
        QString deviceAddr = "51:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "CGH1";
        QString deviceModel_theengs = "CGH1";
        QString deviceAddr = "52:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "MUE4094RT";
        QString deviceModel_theengs = "MUE4094RT";
        QString deviceAddr = "53:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }


    {
        QString deviceName = "ADHS";
        QString deviceModel_theengs = "ADHS";
        QString deviceAddr = "61:57:43:01:5C:3A";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "INEM";
        QString deviceModel_theengs = "INEM";
        QString deviceAddr = "61:57:43:01:5C:3B";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "SDLS";
        QString deviceModel_theengs = "SDLS";
        QString deviceAddr = "61:57:43:01:5C:3C";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "BM2";
        QString deviceModel_theengs = "BM2";
        QString deviceAddr = "61:57:43:01:5C:3D";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
    {
        QString deviceName = "THX1/W230150X";
        QString deviceModel_theengs = "THX1/W230150X";
        QString deviceAddr = "61:57:43:01:5C:3E";

        Device *d = createTheengsDevice_fromDb(deviceName, deviceModel_theengs, deviceAddr);
        if (d)
        {
            m_devices_model->addDevice(d);
            qDebug() << "* Device added (from FAKER): " << deviceName << "/" << deviceAddr;
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceManager::fakeTheengsData()
{
    QBluetoothDeviceInfo info;
    int rrdd = (rand() % 24);


    if (rrdd == 0) // JQJCY01YM
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("11:57:43:01:5C:3A"), "JQJCY01YM", 0);

        int rrrr = (rand() % 4);
        if (rrrr == 0) info.setServiceData(QUuid(0), QByteArray::fromHex("5020df02383a5c014357480a10015e"));
        else if (rrrr == 1) info.setServiceData(QUuid(0), QByteArray::fromHex("5020df02283a5c014357480610025302"));
        else if (rrrr == 2) info.setServiceData(QUuid(0), QByteArray::fromHex("5020df025b3a5c014357481010020800"));
        else if (rrrr == 3) info.setServiceData(QUuid(0), QByteArray::fromHex("5120df023e3a5c01435748041002c400"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 1) // CGDN1
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("11:57:43:01:5C:3B"), "CGDN1", 0);

        int rrrr = (rand() % 3);
        if (rrrr == 0) info.setServiceData(QBluetoothUuid(quint32(0xfdcd)), QByteArray::fromHex("080eaabbccddeeff010422014c011204710072001302ed03"));
        else if (rrrr == 1) info.setServiceData(QBluetoothUuid(quint32(0xfdcd)), QByteArray::fromHex("880eaabbccddeeff0104f900b50112047d0186011302fd02"));
        else if (rrrr == 2) info.setServiceData(QBluetoothUuid(quint32(0xfdcd)), QByteArray::fromHex("880eaabbccddeeff0104f600ab011204a400d7001302c702"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 2) // CGP1W
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("11:57:43:01:5C:3C"), "CGP1W", 0);

        int rrrr = (rand() % 3);
        if (rrrr == 0) info.setServiceData(QBluetoothUuid(quint32(0xfdcd)), QByteArray::fromHex("08094c0140342d5801040801870207024f2702015c"));
        else if (rrrr == 1) info.setServiceData(QBluetoothUuid(quint32(0xfdcd)), QByteArray::fromHex("08094c0140342d5801040f01880207024f2702015c"));
        else if (rrrr == 2) info.setServiceData(QBluetoothUuid(quint32(0xfdcd)), QByteArray::fromHex("08094c0140342d580104fc004a0207026627020120"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 3) // H5106
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("11:57:43:01:5C:3D"), "GVH5106_4313", 0);
        info.setManufacturerData(endian_flip_16(0x0100), QByteArray::fromHex("01010d8f63cb"));
    }


    if (rrdd == 4) // TPMS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("21:57:43:01:5C:3A"), "TPMS1_10CA8F", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x0001), QByteArray::fromHex("215743015c3af46503007c0c00003300"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0x0001), QByteArray::fromHex("215743015c3afb630100ef0900005700"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 5) // H5055
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("22:57:43:01:5C:3A"), "GVH5055", 0);

        int rrrr = (rand() % 4);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("0400461b061700ffff2c01067300ffff2c010000"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("0400417f065600ffff2c01069100ffff2c010"));
        else if (rrrr == 2) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("040061bf065c00ffff2c01063700ffff2c010000"));
        else if (rrrr == 3) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("0400538f06ffffffff2c01065400ffff2c010"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 6) // IBT-2X
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("23:57:43:01:5C:3A"), "iBBQ", 0);

        int rrrr = (rand() % 5);
        if (rrrr == 0) info.setManufacturerData(0, QByteArray::fromHex("0000235743015c3ae600e600"));
        else if (rrrr == 1) info.setManufacturerData(0, QByteArray::fromHex("0000235743015c3a18014001"));
        else if (rrrr == 2) info.setManufacturerData(0, QByteArray::fromHex("0000235743015c3af6ff8a02"));
        else if (rrrr == 3) info.setManufacturerData(0, QByteArray::fromHex("0000235743015c3adc00d200"));
        else if (rrrr == 4) info.setManufacturerData(0, QByteArray::fromHex("0000235743015c3af6ff4402"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 7) // IBT-4XS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("24:57:43:01:5C:3A"), "iBBQ", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(0, QByteArray::fromHex("0000245743015c3a04010401fa00fa00"));
        else if (rrrr == 1) info.setManufacturerData(0, QByteArray::fromHex("0000245743015c3a0401f6ff58021202"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 8) // IBT-6XS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("25:57:43:01:5C:3A"), "iBBQ", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(0, QByteArray::fromHex("00003403de2745cdd200c800f6ffd200f6fff6ff"));
        else if (rrrr == 1) info.setManufacturerData(0, QByteArray::fromHex("00000cb2b71b5b18c800c800f6ffd200f6fff6ff"));
        else qWarning() << "RAND ERROR";
    }


    if (rrdd == 9) // Mi_Smart_Scale
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("31:57:43:01:5C:3A"), "Xiaomi scale", 0);

        int rrrr = (rand() % 8);
        if (rrrr == 0) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("223e30e607020e10293a"));
        else if (rrrr == 1) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("627607e607020e10293a"));
        else if (rrrr == 2) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("a23e30e607020e10293a"));
        else if (rrrr == 3) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("e27607e607020e10293a"));
        else if (rrrr == 4) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("237233e607020e10293a"));
        else if (rrrr == 5) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("637607e607020e10293a"));
        else if (rrrr == 6) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("a37233e607020e10293a"));
        else if (rrrr == 7) info.setServiceData(QBluetoothUuid(quint32(0x181d)), QByteArray::fromHex("e37607e607020e10293a"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 10) // Mi_Body_Composition_Scale
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("32:57:43:01:5C:3A"), "Xiaomi scale", 0);

        int rrrr = (rand() % 12);
        if (rrrr == 0) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("0226e607020e10293af7019a38"));
        else if (rrrr == 1) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("0224e607020e10293a00009a38"));
        else if (rrrr == 2) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("0624e607020e10293a0000fc03"));
        else if (rrrr == 3) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("0326e607020e10293af701f136"));
        else if (rrrr == 4) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("0324e607020e10293a0000f136"));
        else if (rrrr == 5) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("0724e607020e10293a0000ce04"));
        else if (rrrr == 6) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("02a6e607020e10293af7019a38"));
        else if (rrrr == 7) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("06a4e607020e10293a0000fc03"));
        else if (rrrr == 8) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("02a4e607020e10293a00009a38"));
        else if (rrrr == 9) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("03a6e607020e10293af701f136"));
        else if (rrrr ==10) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("03a4e607020e10293a0000f136"));
        else if (rrrr ==11) info.setServiceData(QBluetoothUuid(quint32(0x181b)), QByteArray::fromHex("07a4e607020e10293a0000ce04"));
        else qWarning() << "RAND ERROR";
    }


    if (rrdd == 11) // H5072
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("41:57:43:01:5C:3A"), "GVH5072_1234", 0);
        info.setManufacturerData(endian_flip_16(0x88ec), QByteArray::fromHex("000418ee6400"));
    }
    if (rrdd == 12) // H5075
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("42:57:43:01:5C:3A"), "GVH5075_1234", 0);
        info.setManufacturerData(endian_flip_16(0x88ec), QByteArray::fromHex("0004344b6400"));
    }
    if (rrdd == 13) // H5102
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("43:57:43:01:5C:3A"), "GVH5102_1234", 0);
        info.setManufacturerData(endian_flip_16(0x0100), QByteArray::fromHex("010103590e64"));
    }
    if (rrdd == 14) // BM_V23
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("44:57:43:01:5C:3A"), "V23", 0);
        info.setManufacturerData(endian_flip_16(0x3301), QByteArray::fromHex("17560e10177000ef01b3006c0100"));
    }
    if (rrdd == 15) // IBS-TH1
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("45:57:43:01:5C:3A"), "sps", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x660a), QByteArray::fromHex("03150110805908"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0xcd09), QByteArray::fromHex("a51901d03f0008"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 16) // IBS-TH2
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("46:57:43:01:5C:3A"), "tps", 0);

        int rrrr = (rand() % 3);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x660a), QByteArray::fromHex("03150110805908"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0x76fb), QByteArray::fromHex("03150110805908"));
        else if (rrrr == 2) info.setManufacturerData(endian_flip_16(0xd8f8), QByteArray::fromHex("00000035733206"));
        else qWarning() << "RAND ERROR";
    }


    if (rrdd == 17) // CGPR1
    {
        QBluetoothUuid uuid(static_cast<quint16>(0xfdcd));
        info = QBluetoothDeviceInfo(QBluetoothAddress("51:57:43:01:5C:3A"), "CGPR1", 0);
        info.setServiceData(uuid, QByteArray::fromHex("8812aabbccddeeff0201640f01c4090405020000"));
    }
    if (rrdd == 18) // CGH1
    {
        QBluetoothUuid uuid(static_cast<quint16>(0xfdcd));
        info = QBluetoothDeviceInfo(QBluetoothAddress("52:57:43:01:5C:3A"), "CGH1", 0);

        int rrrr = (rand() % 4);
        if (rrrr == 0) info.setServiceData(uuid, QByteArray::fromHex("0804751060342d580201600f012b0f0100"));
        else if (rrrr == 1) info.setServiceData(uuid, QByteArray::fromHex("0804751060342d580201600f01420f0101"));
        else if (rrrr == 2) info.setServiceData(uuid, QByteArray::fromHex("4804751060342d580401000f01cb"));
        else if (rrrr == 3) info.setServiceData(uuid, QByteArray::fromHex("4804751060342d580401010f01d5"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 19) // MUE4094RT
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("53:57:43:01:5C:3A"), "MUE4094RT", 0);
        info.setServiceData(QUuid(0), QByteArray::fromHex("4030dd031d0300010100"));
    }


    if (rrdd == 20) // ADHS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("61:57:43:01:5C:3A"), "ADHS", 0);

        info.setManufacturerData(endian_flip_16(0xeefa), QByteArray::fromHex("0000240015000015001a0029000c194f000000"));
    }
    if (rrdd == 21) // INEM
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("61:57:43:01:5C:3B"), "INEM", 0);

        int rrrr = (rand() % 7);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x9082), QByteArray::fromHex("6300f0cf0000c409820080"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0x9482), QByteArray::fromHex("6300f0cf0000c409260080"));
        else if (rrrr == 2) info.setManufacturerData(endian_flip_16(0x9082), QByteArray::fromHex("6300f0cf0000c409b60080"));
        else if (rrrr == 3) info.setManufacturerData(endian_flip_16(0x9282), QByteArray::fromHex("6300f0cf0000c409160080"));
        else if (rrrr == 4) info.setManufacturerData(endian_flip_16(0x9082), QByteArray::fromHex("dd0061b80000c4096b0080"));
        else if (rrrr == 5) info.setManufacturerData(endian_flip_16(0x9082), QByteArray::fromHex("6300f0cf0000c419760080"));
        else if (rrrr == 6) info.setManufacturerData(endian_flip_16(0x9682), QByteArray::fromHex("dd0061b80000c4193b0080"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 22) // SDLS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("61:57:43:01:5C:3C"), "SDLS", 0);

        int rrrr = (rand() % 6);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0xae01), QByteArray::fromHex("56d708420000c84252006907"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0xae01), QByteArray::fromHex("9bc8af4108d7c34208016807"));
        else if (rrrr == 2) info.setManufacturerData(endian_flip_16(0xae01), QByteArray::fromHex("8c60fe41b8fbc64233006d07"));
        else if (rrrr == 3) info.setManufacturerData(endian_flip_16(0xae01), QByteArray::fromHex("ca9dec4160fc5f424a005207"));
        else if (rrrr == 4) info.setManufacturerData(endian_flip_16(0xae01), QByteArray::fromHex("ca9dec4160fc5f424a005200"));
        else if (rrrr == 5) info.setManufacturerData(endian_flip_16(0xae01), QByteArray::fromHex("ca9dec4160fc5f424a005206"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 23) // BM2
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("61:57:43:01:5C:3D"), "BM2", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x4c00), QByteArray::fromHex("0215655f83caae16a10a702e31f30d58dd82f644000064"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0x4c00), QByteArray::fromHex("0215655f83caae16a10a702e31f30d58dd82f441423144"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 24) // THX1/W230150X
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("61:57:43:01:5C:3E"), "THX1/W230150X", 0);

        int rrrr = (rand() % 3);
        if (rrrr == 0) info.setServiceData(QBluetoothUuid(quint32(0xfd3d)), QByteArray::fromHex("6900ba18993b"));
        else if (rrrr == 1) info.setServiceData(QBluetoothUuid(quint32(0xfd3d)), QByteArray::fromHex("6900ba031938"));
        else if (rrrr == 2) info.setServiceData(QBluetoothUuid(quint32(0xfd3d)), QByteArray::fromHex("6900ba379ab8"));
        else qWarning() << "RAND ERROR";
    }


    //qDebug() << "DeviceManager::fakeTheengsData(" << info.name() << ")";
    info.setCoreConfigurations(QBluetoothDeviceInfo::LowEnergyCoreConfiguration);
    updateBleDevice(info, QBluetoothDeviceInfo::Field::None);
}

/* ************************************************************************** */
