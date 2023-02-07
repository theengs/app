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

#include "device_theengs.h"
#include "SettingsManager.h"
#include "MqttManager.h"

#include <cstdint>
#include <cmath>

#include <QBluetoothUuid>
#include <QBluetoothServiceInfo>
#include <QLowEnergyService>

#include <QSqlQuery>
#include <QSqlError>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDateTime>
#include <QDebug>

/* ************************************************************************** */

DeviceTheengs::DeviceTheengs(const QString &deviceAddr, const QString &deviceName, const QString &deviceModel, QObject *parent):
    DeviceSensor(deviceAddr, deviceName, parent)
{
    m_deviceModel = deviceModel;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
}

DeviceTheengs::DeviceTheengs(const QBluetoothDeviceInfo &d, const QString &deviceModel, QObject *parent):
    DeviceSensor(d, parent)
{
    m_deviceModel = deviceModel;
    m_deviceBluetoothMode = DeviceUtils::DEVICE_BLE_ADVERTISEMENT;
}

DeviceTheengs::~DeviceTheengs()
{
    //
}

/* ************************************************************************** */

void DeviceTheengs::serviceScanDone()
{
    //qDebug() << "DeviceTheengs::serviceScanDone(" << m_deviceAddress << ")";
}

void DeviceTheengs::addLowEnergyService(const QBluetoothUuid &uuid)
{
    //qDebug() << "DeviceTheengs::addLowEnergyService(" << uuid.toString() << ")";
    Q_UNUSED (uuid)
}

/* ************************************************************************** */

bool DeviceTheengs::getSqlProbeData(int minutes)
{
    //qDebug() << "DeviceSensor::getSqlProbeData(" << m_deviceAddress << ")";
    bool status = false;

    QSqlQuery cachedData;
    if (m_dbInternal) // sqlite
    {
        cachedData.prepare("SELECT timestamp, temperature1, temperature2, temperature3, temperature4, temperature5, temperature6 " \
                           "FROM sensorTheengs " \
                           "WHERE deviceAddr = :deviceAddr AND timestamp >= datetime('now', 'localtime', '-" + QString::number(minutes) + " minutes') " \
                           "ORDER BY timestamp DESC " \
                           "LIMIT 1;");
    }
    cachedData.bindValue(":deviceAddr", getAddress());

    if (cachedData.exec() == false)
    {
        qWarning() << "> cachedDataProbe.exec() ERROR"
                   << cachedData.lastError().type() << ":" << cachedData.lastError().text();
    }

    while (cachedData.next())
    {
        m_temperature1 = cachedData.value(1).toFloat();
        m_temperature2 = cachedData.value(2).toFloat();
        m_temperature3 = cachedData.value(3).toFloat();
        m_temperature4 = cachedData.value(4).toFloat();
        m_temperature5 = cachedData.value(5).toFloat();
        m_temperature6 = cachedData.value(6).toFloat();

        QString datetime = cachedData.value(0).toString();
        m_lastUpdateDatabase = m_lastUpdate = QDateTime::fromString(datetime, "yyyy-MM-dd hh:mm:ss");
/*
        qDebug() << ">> timestamp" << m_lastUpdate;
        qDebug() << "- m_temperature1:" << m_temperature1;
        qDebug() << "- m_temperature2:" << m_temperature2;
        qDebug() << "- m_temperature3:" << m_temperature3;
        qDebug() << "- m_temperature4:" << m_temperature4;
        qDebug() << "- m_temperature5:" << m_temperature5;
        qDebug() << "- m_temperature6:" << m_temperature6;
*/
        status = true;
    }

    refreshDataFinished(status, true);
    return status;
}

bool DeviceTheengs::getSqlTpmsData(int minutes)
{
    //qDebug() << "DeviceSensor::getSqlTpmsData(" << m_deviceAddress << ")";
    bool status = false;

    QSqlQuery cachedData;
    if (m_dbInternal) // sqlite
    {
        cachedData.prepare("SELECT timestamp, temperature1, temperature2, temperature3, temperature4, " \
                             "pressure1, pressure2, pressure3, pressure4, " \
                             "battery1, battery2, battery3, battery4, " \
                             "alarm1, alarm2, alarm3, alarm4 " \
                           "FROM sensorTheengs " \
                           "WHERE deviceAddr = :deviceAddr AND timestamp >= datetime('now', 'localtime', '-" + QString::number(minutes) + " minutes') " \
                           "ORDER BY timestamp DESC " \
                           "LIMIT 1;");
    }
    cachedData.bindValue(":deviceAddr", getAddress());

    if (cachedData.exec() == false)
    {
        qWarning() << "> cachedDataTPMS.exec() ERROR"
                   << cachedData.lastError().type() << ":" << cachedData.lastError().text();
    }

    while (cachedData.next())
    {
        if (!cachedData.value(1).isNull())
        {
            m_temperature1 = cachedData.value(1).toFloat();
            m_pressure1 = cachedData.value(5).toInt();
            m_battery1 = cachedData.value(9).toInt();
            m_alarm1 = cachedData.value(13).toBool();
        }
        if (!cachedData.value(2).isNull())
        {
            m_temperature2 = cachedData.value(2).toFloat();
            m_pressure2 = cachedData.value(6).toInt();
            m_battery2 = cachedData.value(10).toInt();
            m_alarm2 = cachedData.value(14).toBool();
        }
        if (!cachedData.value(2).isNull())
        {
            m_temperature3 = cachedData.value(3).toFloat();
            m_pressure3 = cachedData.value(7).toInt();
            m_battery3 = cachedData.value(11).toInt();
            m_alarm3 = cachedData.value(15).toBool();
        }
        if (!cachedData.value(2).isNull())
        {
            m_temperature4 = cachedData.value(4).toFloat();
            m_pressure4 = cachedData.value(8).toInt();
            m_battery4 = cachedData.value(12).toInt();
            m_alarm4 = cachedData.value(16).toBool();
        }

        QString datetime = cachedData.value(0).toString();
        m_lastUpdateDatabase = m_lastUpdate = QDateTime::fromString(datetime, "yyyy-MM-dd hh:mm:ss");
/*
        qDebug() << ">> timestamp" << m_lastUpdate;
        qDebug() << "- m_temperature1:" << m_temperature1;
        qDebug() << "- m_temperature2:" << m_temperature2;
        qDebug() << "- m_temperature3:" << m_temperature3;
        qDebug() << "- m_temperature4:" << m_temperature4;

        qDebug() << "- m_pressure1:" << m_pressure1;
        qDebug() << "- m_pressure2:" << m_pressure2;
        qDebug() << "- m_pressure3:" << m_pressure3;
        qDebug() << "- m_pressure4:" << m_pressure4;
*/
        status = true;
    }

    refreshDataFinished(status, true);
    return status;
}

bool DeviceTheengs::getSqlScaleData(int minutes)
{
    //qDebug() << "DeviceSensor::getSqlScaleData(" << m_deviceAddress << ")";
    bool status = false;

    QSqlQuery cachedData;
    if (m_dbInternal) // sqlite
    {
        cachedData.prepare("SELECT timestamp, weight, impedance " \
                           "FROM sensorTheengs " \
                           "WHERE deviceAddr = :deviceAddr AND timestamp >= datetime('now', 'localtime', '-" + QString::number(minutes) + " minutes') " \
                           "ORDER by timestamp DESC " \
                           "LIMIT 1;");
    }
    cachedData.bindValue(":deviceAddr", getAddress());

    if (cachedData.exec() == false)
    {
        qWarning() << "> cachedDataScale.exec() ERROR"
                   << cachedData.lastError().type() << ":" << cachedData.lastError().text();
    }

    while (cachedData.next())
    {
        m_weight = cachedData.value(1).toFloat();
        m_impedance = cachedData.value(2).toInt();

        QString datetime = cachedData.value(0).toString();
        m_lastUpdateDatabase = m_lastUpdate = QDateTime::fromString(datetime, "yyyy-MM-dd hh:mm:ss");
/*
        qDebug() << ">> timestamp" << m_lastUpdate;
        qDebug() << "- m_weight:" << m_weight;
        qDebug() << "- m_impedance:" << m_impedance;
*/
        status = true;
    }

    refreshDataFinished(status, true);
    return status;
}

/* ************************************************************************** */

bool DeviceTheengs::hasData() const
{
    // If we have immediate data (<12h old)

    if (isProbe())
    {
        if (m_temperature1 > -80.f || m_temperature2 > -80.f ||
            m_temperature3 > -80.f || m_temperature4 > -80.f ||
            m_temperature5 > -80.f || m_temperature6 > -80.f)
            return true;
    }
    else if (isScale())
    {
        if (m_weight > -80.f)
            return true;
    }
    else if (isMotionSensor())
    {
        return true;
    }
    else
    {
        return DeviceSensor::hasData();
    }

    // Otherwise, check if we have stored data
    if (m_dbInternal || m_dbExternal)
    {
        QSqlQuery hasData;
        hasData.prepare("SELECT COUNT(*) FROM sensorTheengs WHERE deviceAddr = :deviceAddr;");
        hasData.bindValue(":deviceAddr", getAddress());

        if (hasData.exec() == false)
        {
            qWarning() << "> hasData.exec(t) ERROR" << hasData.lastError().type() << ":" << hasData.lastError().text();
            qWarning() << "> hasData.exec(t) >" << hasData.lastQuery();
        }

        while (hasData.next())
        {
            if (hasData.value(0).toInt() > 0) // data count
                return true;
        }
    }

    return false;
}

bool DeviceTheengs::needsUpdateRt() const
{
    return !isDataFresh_rt();
}

bool DeviceTheengs::needsUpdateDb() const
{
    return !isDataFresh_db();
}

/* ************************************************************************** */

bool DeviceTheengs::isValid() const
{
    if (m_deviceType == 0)
    {
        qWarning() << "DeviceTheengs::isValid(false)" << "m_deviceType == 0";
        return false;
    }
    if (m_deviceSensors == 0 && m_deviceSensorsTheengs == 0)
    {
        qWarning() << "DeviceTheengs::isValid(false)" << "m_deviceSensors == 0 && m_deviceSensorsTheengs == 0";
        return false;
    }

    return true;
}

/* ************************************************************************** */

float DeviceTheengs::getTemp1() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp1F();
    return getTemp1C();
}
float DeviceTheengs::getTemp2() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp2F();
    return getTemp2C();
}
float DeviceTheengs::getTemp3() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp3F();
    return getTemp3C();
}
float DeviceTheengs::getTemp4() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp4F();
    return getTemp4C();
}
float DeviceTheengs::getTemp5() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp5F();
    return getTemp5C();
}
float DeviceTheengs::getTemp6() const
{
    SettingsManager *s = SettingsManager::getInstance();
    if (s->getTempUnit() == "F") return getTemp6F();
    return getTemp6C();
}

/* ************************************************************************** */

void DeviceTheengs::parseTheengsProps(const QString &json)
{
    Q_UNUSED(json)
}

void DeviceTheengs::parseTheengsAdvertisement(const QString &json)
{
    Q_UNUSED(json)
}

/* ************************************************************************** */

bool DeviceTheengs::createDiscoveryMQTT(const QString &deviceAddr, const QString &deviceName,
                                        const QString &deviceModel, const QString &deviceManufacturer,
                                        const QString &devicePropsJson, const QString &appAddr)
{
    //qDebug() << "DeviceTheengs::createDiscoveryMQTT() deviceName" << deviceName << "  -  " << devicePropsJson;
    bool status = false;

    SettingsManager *sm = SettingsManager::getInstance();
    MqttManager *mqtt = MqttManager::getInstance();

    if (sm && sm->getMqttDiscovery() && mqtt && mqtt->getStatus())
    {
        QString appAddrClean = appAddr;
        appAddrClean.remove(':');

        QString deviceAddrClean = deviceAddr;
        deviceAddrClean.remove(':');

        // check required params
        if (deviceAddrClean.isEmpty() || deviceModel.isEmpty() || devicePropsJson.isEmpty())
        {
            return false;
        }

        QJsonArray idarr, connarr;
        idarr.push_back(QJsonValue::fromVariant(deviceAddrClean));
        QStringList conn; conn.push_back("mac"); conn.push_back(deviceAddrClean);
        connarr.push_back(QJsonValue::fromVariant(conn).toArray());

        // create device object
        QJsonObject deviceObject;
        deviceObject.insert("identifiers", idarr);
        deviceObject.insert("connections", connarr);
        deviceObject.insert("manufacturer", QJsonValue::fromVariant(deviceManufacturer));
        deviceObject.insert("model", QJsonValue::fromVariant(deviceModel));
        deviceObject.insert("name", QJsonValue::fromVariant(deviceName));
        deviceObject.insert("via_device", QJsonValue::fromVariant(appAddrClean));

        QJsonObject prop = QJsonDocument::fromJson(devicePropsJson.toUtf8()).object()["properties"].toObject();
        for (auto it = prop.begin(), end = prop.end(); it != end; ++it)
        {
            QString prop_key = it.key();
            QJsonObject prop_value = it.value().toObject();

            QString value_name;
            QString value_unit;
            if (prop_value.contains("name")) value_name = prop_value["name"].toString();
            if (prop_value.contains("unit")) value_unit = prop_value["unit"].toString();

            if (!availableHASSClasses.contains(value_name)) continue;
            if (!availableHASSUnits.contains(value_unit)) continue;

            // create discovery object
            QJsonObject discovery;
            discovery.insert("state_class", "measurement");
            discovery.insert("state_topic", "+/+/BTtoMQTT/" + deviceAddrClean);
            discovery.insert("device", deviceObject);

            discovery.insert("name", deviceModel + "-" + prop_key);
            discovery.insert("unique_id", deviceAddrClean + "-" + prop_key);
            discovery.insert("device_class", value_name);
            discovery.insert("unit_of_measurement", value_unit);
            discovery.insert("value_template", "{{ value_json." + prop_key + " | is_defined }}");

            QString mqtt_topic = "homeassistant/sensor/" + deviceAddrClean + "-" + prop_key + "/config";
            QString str_out(QJsonDocument(discovery).toJson(QJsonDocument::Compact));

            status = mqtt->publishConfig(mqtt_topic, str_out);
        }
    }

    return status;
}

/* ************************************************************************** */
