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
#include "MqttManager.h"

#include "utils/utils_app.h"
#include "utils/utils_bits.h"

#include <decoder.h> // Theengs decoder

#include "device_theengs.h"
#include "devices/device_theengs_generic.h"
#include "devices/device_theengs_beacons.h"
#include "devices/device_theengs_probes.h"
#include "devices/device_theengs_scales.h"
#include "devices/device_theengs_motionsensors.h"

#include <string>

#include <QBluetoothUuid>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>

#include <QList>
#include <QDebug>

/* ************************************************************************** */

Device * DeviceManager::createTheengsDevice_fromDb(const QString &deviceName,
                                                   const QString &deviceModel_theengs,
                                                   const QString &deviceAddr)
{
    Device *device = nullptr;

    QString device_props = QString::fromUtf8(TheengsDecoder().getTheengProperties(deviceModel_theengs.toLatin1()));

    if (!deviceModel_theengs.isEmpty() && !device_props.isEmpty())
    {
        if (deviceModel_theengs == "TPMS" ||
            deviceModel_theengs == "H5055" ||
            deviceModel_theengs == "IBT-2X" ||
            deviceModel_theengs == "IBT-4XS" ||
            deviceModel_theengs == "IBT-6XS" ||
            deviceModel_theengs == "SOLIS-6" ||
            deviceModel_theengs == "IBT-6XS/SOLIS-6")
        {
            device = new DeviceTheengsProbes(deviceAddr, deviceName,
                                             deviceModel_theengs, device_props, this);
        }
        else if (deviceModel_theengs == "XMTZC01HM/XMTZC04HM" ||
                 deviceModel_theengs == "XMTZC02HM/XMTZC05HM")
        {
            device = new DeviceTheengsScales(deviceAddr, deviceName,
                                             deviceModel_theengs, device_props, this);
        }
        else if (deviceModel_theengs == "MUE4094RT" ||
                 deviceModel_theengs == "CGPR1" ||
                 deviceModel_theengs == "CGH1")
        {
            device = new DeviceTheengsMotionSensors(deviceAddr, deviceName,
                                                    deviceModel_theengs, device_props, this);
        }/*
        else if (deviceModel_theengs == "MiBand" ||
                 deviceModel_theengs == "INEM" ||
                 deviceModel_theengs == "Mokobeacon" ||
                 deviceModel_theengs == "RuuviTag_RAWv1" ||
                 deviceModel_theengs == "RuuviTag_RAWv2")
        {
            device = new DeviceTheengsBeacons(deviceAddr, deviceName,
                                         deviceModel_theengs, device_props, this);
        }*/
        else
        {
            device = new DeviceTheengsGeneric(deviceAddr, deviceName,
                                              deviceModel_theengs, device_props, this);
        }
    }
    else
    {
        qWarning() << "Unknown device model: " << deviceModel_theengs << device_props;
    }

    return device;
}

Device * DeviceManager::createTheengsDevice_fromAdv(const QBluetoothDeviceInfo &deviceInfo)
{
    //qDebug() << "createTheengsDevice_fromAdv(" << deviceInfo.name() << ")";

    DeviceTheengs *device = nullptr;

    QString device_model_theengs;
    QString device_modelid_theengs;
    QString device_props;

    const QList<quint16> &manufacturerIds = deviceInfo.manufacturerIds();
    for (const auto id: manufacturerIds)
    {
        if (device_modelid_theengs.isEmpty() == false) break;

        DynamicJsonDocument doc(1024);
        doc["id"] = deviceInfo.address().toString().toStdString();
        doc["name"] = deviceInfo.name().toStdString();
        doc["manufacturerdata"] = QByteArray::number(endian_flip_16(id), 16).rightJustified(4, '0').toStdString() + deviceInfo.manufacturerData(id).toHex().toStdString();

        TheengsDecoder dec;
        JsonObject obj = doc.as<JsonObject>();

        if (dec.decodeBLEJson(obj) >= 0)
        {
            device_model_theengs = QString::fromStdString(doc["model"]);
            device_modelid_theengs = QString::fromStdString(doc["model_id"]);
            device_props = QString::fromLatin1(dec.getTheengProperties(device_modelid_theengs.toLatin1()));

            qDebug() << "addDevice() FOUND [mfd] :" << device_modelid_theengs << device_props;
            break;
        }
    }

    const QList<QBluetoothUuid> &serviceIds = deviceInfo.serviceIds();
    for (const auto id: serviceIds)
    {
        if (device_modelid_theengs.isEmpty() == false) break;

        DynamicJsonDocument doc(1024);
        doc["id"] = deviceInfo.address().toString().toStdString();
        doc["name"] = deviceInfo.name().toStdString();
        doc["servicedata"] = deviceInfo.serviceData(id).toHex().toStdString();
        doc["servicedatauuid"] = id.toString(QUuid::Id128).toStdString();

        TheengsDecoder dec;
        JsonObject obj = doc.as<JsonObject>();

        if (dec.decodeBLEJson(obj) >= 0)
        {
            device_model_theengs = QString::fromStdString(doc["model"]);
            device_modelid_theengs = QString::fromStdString(doc["model_id"]);
            device_props = QString::fromUtf8(dec.getTheengProperties(device_modelid_theengs.toLatin1()));

            qDebug() << "addDevice() FOUND [svd] :" << device_modelid_theengs << device_props;
            break;
        }
    }

    if ((!device_modelid_theengs.isEmpty() && !device_props.isEmpty()))
    {
        //qDebug() << "device_modelId[out]  " << device_modelid_theengs;
        //qDebug() << "device_props[out] " << device_props;

        if (device_modelid_theengs == "TPMS" ||
            device_modelid_theengs == "H5055"  ||
            device_modelid_theengs == "IBT-2X" ||
            device_modelid_theengs == "IBT-4XS" ||
            device_modelid_theengs == "IBT-6XS" ||
            device_modelid_theengs == "SOLIS-6" ||
            device_modelid_theengs == "IBT-6XS/SOLIS-6")
        {
            device = new DeviceTheengsProbes(deviceInfo, device_modelid_theengs, device_props, this);
        }
        else if (device_modelid_theengs == "XMTZC01HM/XMTZC04HM" ||
                 device_modelid_theengs == "XMTZC02HM/XMTZC05HM")
        {
            device = new DeviceTheengsScales(deviceInfo, device_modelid_theengs, device_props, this);
        }
        else if (device_modelid_theengs == "MUE4094RT" ||
                 device_modelid_theengs == "CGPR1" ||
                 device_modelid_theengs == "CGH1")
        {
            device = new DeviceTheengsMotionSensors(deviceInfo, device_modelid_theengs, device_props, this);
        }
        else
        {
            device = new DeviceTheengsGeneric(deviceInfo, device_modelid_theengs, device_props, this);
        }

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
/* ************************************************************************** */

QString DeviceManager::getDeviceModelTheengs(const QString &modelid) const
{
    return QString::fromUtf8(TheengsDecoder().getTheengAttribute(modelid.toLatin1(), "model"));
}

/* ************************************************************************** */
/* ************************************************************************** */

void DeviceManager::fakeTheengsDevices()
{
    // Theengs fake data ///////////////////////////////////////////////////////
    {
        connect(&m_faker, &QTimer::timeout, this, &DeviceManager::fakeTheengsData);
        m_faker.setInterval(1*1000); // 1s
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
        QString deviceModel_theengs = "IBT-2X";
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
        QString deviceModel_theengs = "IBT-4XS";
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
        QString deviceModel_theengs = "H5072";
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
        QString deviceModel_theengs = "H5075";
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
        QString deviceModel_theengs = "H5102";
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
        QString deviceModel_theengs = "BM_V23";
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
        QString deviceModel_theengs = "IBS-TH1";
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
        QString deviceModel_theengs = "IBS-TH2";
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
}

/* ************************************************************************** */

void DeviceManager::fakeTheengsData()
{
    QBluetoothDeviceInfo info;
    int rrdd = (rand() % 17);

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


    if (rrdd == 1) // TPMS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("21:57:43:01:5C:3A"), "TPMS1_10CA8F", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x0001), QByteArray::fromHex("80eaca10ca8ff46503007c0c00003300"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0x0001), QByteArray::fromHex("80eaca115a954b630100ef0900005700"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 2) // H5055
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("22:57:43:01:5C:3A"), "GVH5055", 0);

        int rrrr = (rand() % 4);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("0400461b061700ffff2c01067300ffff2c010000"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("0400417f065600ffff2c01069100ffff2c010"));
        else if (rrrr == 2) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("040061bf065c00ffff2c01063700ffff2c010000"));
        else if (rrrr == 3) info.setManufacturerData(endian_flip_16(0xcf04), QByteArray::fromHex("0400538f06ffffffff2c01065400ffff2c010"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 3) // IBT-2X
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("23:57:43:01:5C:3A"), "iBBQ", 0);

        int rrrr = (rand() % 5);
        if (rrrr == 0) info.setManufacturerData(0, QByteArray::fromHex("0000fc45c30c458ee600e600"));
        else if (rrrr == 1) info.setManufacturerData(0, QByteArray::fromHex("0000fc45c30c458e18014001"));
        else if (rrrr == 2) info.setManufacturerData(0, QByteArray::fromHex("0000fc45c30c458ef6ff8a02"));
        else if (rrrr == 3) info.setManufacturerData(0, QByteArray::fromHex("0000fc45c30d38a8dc00d200"));
        else if (rrrr == 4) info.setManufacturerData(0, QByteArray::fromHex("0000fc45c30d38a8f6ff4402"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 4) // IBT-4XS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("24:57:43:01:5C:3A"), "iBBQ", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(0, QByteArray::fromHex("000010082c40abe604010401fa00fa00"));
        else if (rrrr == 1) info.setManufacturerData(0, QByteArray::fromHex("000010082c40abe60401f6ff58021202"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 5) // IBT-6XS
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("25:57:43:01:5C:3A"), "iBBQ", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(0, QByteArray::fromHex("00003403de2745cdd200c800f6ffd200f6fff6ff"));
        else if (rrrr == 1) info.setManufacturerData(0, QByteArray::fromHex("00000cb2b71b5b18c800c800f6ffd200f6fff6ff"));
        else qWarning() << "RAND ERROR";
    }


    if (rrdd == 6) // Mi_Smart_Scale
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
    if (rrdd == 7) // Mi_Body_Composition_Scale
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


    if (rrdd == 8) // H5072
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("41:57:43:01:5C:3A"), "GVH5072_1234", 0);
        info.setManufacturerData(endian_flip_16(0x88ec), QByteArray::fromHex("000418ee6400"));
    }
    if (rrdd == 9) // H5075
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("42:57:43:01:5C:3A"), "GVH5075_1234", 0);
        info.setManufacturerData(endian_flip_16(0x88ec), QByteArray::fromHex("0004344b6400"));
    }
    if (rrdd == 10) // H5102
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("43:57:43:01:5C:3A"), "GVH5102_1234", 0);
        info.setManufacturerData(endian_flip_16(0x0100), QByteArray::fromHex("010103590e64"));
    }
    if (rrdd == 11) // BM_V23
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("44:57:43:01:5C:3A"), "V23", 0);
        info.setManufacturerData(endian_flip_16(0x3301), QByteArray::fromHex("17560e10177000ef01b3006c0100"));
    }
    if (rrdd == 12) // IBS-TH1
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("45:57:43:01:5C:3A"), "sps", 0);

        int rrrr = (rand() % 2);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x660a), QByteArray::fromHex("03150110805908"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0xcd09), QByteArray::fromHex("a51901d03f0008"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 13) // IBS-TH2
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("46:57:43:01:5C:3A"), "tps", 0);

        int rrrr = (rand() % 3);
        if (rrrr == 0) info.setManufacturerData(endian_flip_16(0x660a), QByteArray::fromHex("03150110805908"));
        else if (rrrr == 1) info.setManufacturerData(endian_flip_16(0x76fb), QByteArray::fromHex("03150110805908"));
        else if (rrrr == 2) info.setManufacturerData(endian_flip_16(0xd8f8), QByteArray::fromHex("00000035733206"));
        else qWarning() << "RAND ERROR";
    }


    if (rrdd == 14) // CGPR1
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("51:57:43:01:5C:3A"), "CGPR1", 0);
        info.setServiceData(QUuid(0), QByteArray::fromHex("0812443660342d580201530f0118090400000000"));
    }
    if (rrdd == 15) // CGH1
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("52:57:43:01:5C:3A"), "CGH1", 0);

        int rrrr = (rand() % 4);
        if (rrrr == 0) info.setServiceData(QUuid(0), QByteArray::fromHex("0804751060342d580201600f012b0f0100"));
        else if (rrrr == 1) info.setServiceData(QUuid(0), QByteArray::fromHex("0804751060342d580201600f01420f0101"));
        else if (rrrr == 2) info.setServiceData(QUuid(0), QByteArray::fromHex("4804751060342d580401000f01cb"));
        else if (rrrr == 3) info.setServiceData(QUuid(0), QByteArray::fromHex("4804751060342d580401010f01d5"));
        else qWarning() << "RAND ERROR";
    }
    if (rrdd == 16) // MUE4094RT
    {
        info = QBluetoothDeviceInfo(QBluetoothAddress("53:57:43:01:5C:3A"), "MUE4094RT", 0);
        info.setServiceData(QUuid(0), QByteArray::fromHex("4030dd031d0300010100"));
    }

    //qDebug() << "DeviceManager::fakeTheengsData(" << info.name() << ")";
    updateBleDevice(info, QBluetoothDeviceInfo::Field::None);
}

/* ************************************************************************** */
