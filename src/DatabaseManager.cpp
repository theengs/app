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

#include "DatabaseManager.h"
#include "SettingsManager.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QDateTime>
#include <QStandardPaths>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

/* ************************************************************************** */

DatabaseManager *DatabaseManager::instance = nullptr;

DatabaseManager *DatabaseManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new DatabaseManager();
    }

    return instance;
}

DatabaseManager::DatabaseManager()
{
    bool status = false;

    if (!status)
    {
        status = openDatabase_mysql();
    }
    if (!status)
    {
        status = openDatabase_sqlite();
    }
}

DatabaseManager::~DatabaseManager()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DatabaseManager::openDatabase_sqlite()
{
    if (QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
        m_dbInternalAvailable = true;

        if (m_dbInternalOpen)
        {
            closeDatabase();
        }

        QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

        if (dbPath.isEmpty() == false)
        {
            QDir dbDirectory(dbPath);
            if (dbDirectory.exists() == false)
            {
                if (dbDirectory.mkpath(dbPath) == false)
                {
                    qWarning() << "Cannot create dbDirectory...";
                }
            }

            if (dbDirectory.exists())
            {
                dbPath += "/data.db";

                QSqlDatabase dbFile(QSqlDatabase::addDatabase("QSQLITE"));
                dbFile.setDatabaseName(dbPath);

                if (dbFile.isOpen())
                {
                    m_dbInternalOpen = true;
                }
                else
                {
                    if (dbFile.open())
                    {
                        m_dbInternalOpen = true;

                        // Migrations //////////////////////////////////////////

                        // Must be done before the creation, so we migrate old data tables
                        // instead of creating new empty tables

                        migrateDatabase();

                        // Check if our tables exists //////////////////////////

                        createDatabase();

                        // Sanitize database ///////////////////////////////////

                        int maxDays = SettingsManager::getInstance()->getDataRetentionDays();
                        if (maxDays < 30) maxDays = 30;

                        // Delete everything xx days old
                        QSqlQuery sanitizePastData1("DELETE FROM plantData WHERE timestamp < DATE('now', '-" + QString::number(maxDays) + " days')");
                        QSqlQuery sanitizePastData2("DELETE FROM thermoData WHERE timestamp < DATE('now', '-" + QString::number(maxDays) + " days')");
                        QSqlQuery sanitizePastData3("DELETE FROM sensorData WHERE timestamp < DATE('now', '-" + QString::number(maxDays) + " days')");

                        // Basic check to see if the device clock is correctly set
                        if (QDate::currentDate().year() >= 2022)
                        {
                            // Delete everything that's in the future
                            QSqlQuery sanitizeFuruteData1("DELETE FROM plantData WHERE timestamp > DATE('now', '+1 days')");
                            QSqlQuery sanitizeFuruteData2("DELETE FROM thermoData WHERE timestamp > DATE('now', '+1 days')");
                            QSqlQuery sanitizeFuruteData3("DELETE FROM sensorData WHERE timestamp > DATE('now', '+1 days')");
                        }
                    }
                    else
                    {
                        qWarning() << "Cannot open database... Error:" << dbFile.lastError();
                    }
                }
            }
            else
            {
                qWarning() << "Cannot create nor open dbDirectory...";
            }
        }
        else
        {
            qWarning() << "Cannot find QStandardPaths::AppDataLocation directory...";
        }
    }
    else
    {
        qWarning() << "> SQLite is NOT available";
        m_dbInternalAvailable = false;
    }

    return m_dbInternalOpen;
}

/* ************************************************************************** */

bool DatabaseManager::openDatabase_mysql()
{
    if (QSqlDatabase::isDriverAvailable("QMYSQL"))
    {
        m_dbExternalAvailable = true;

        if (m_dbExternalOpen)
        {
            closeDatabase();
        }

        SettingsManager *sm = SettingsManager::getInstance();

        if (sm && sm->getMySQL())
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
            db.setHostName(sm->getMysqlHost());
            db.setPort(sm->getMysqlPort());
            db.setDatabaseName(sm->getMysqlName());
            db.setUserName(sm->getMysqlUser());
            db.setPassword(sm->getMysqlPassword());

            if (db.isOpen())
            {
                m_dbExternalOpen = true;
            }
            else
            {
                if (db.open())
                {
                    m_dbExternalOpen = true;

                    // Migrations ///////////////////////////////////////////////////

                    // Must be done before the creation, so we migrate old data tables
                    // instead of creating new empty tables

                    migrateDatabase();

                    // Check if our tables exists //////////////////////////////////

                    createDatabase();

                    // Delete everything that's in the future //////////////////////

                    // TODO

                    // Sanitize database ///////////////////////////////////////////

                    // We don't sanetize MySQL databases
                }
                else
                {
                    qWarning() << "Cannot open database... Error:" << db.lastError();
                }
            }
        }
    }
    else
    {
        qDebug() << "> MySQL is NOT available";
        m_dbExternalAvailable = false;
    }

    return m_dbExternalOpen;
}

/* ************************************************************************** */

void DatabaseManager::closeDatabase()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isValid())
    {
        QString conName = db.connectionName();

        // close db
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(conName);

        m_dbInternalOpen = false;
        m_dbExternalOpen = false;
    }
}

/* ************************************************************************** */

void DatabaseManager::resetDatabase()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isValid())
    {
        QString dbName = db.databaseName();
        QString conName = db.connectionName();

        // close db
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(conName);

        m_dbInternalOpen = false;
        m_dbExternalOpen = false;

        // remove db file
        QFile::remove(dbName);
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

bool DatabaseManager::tableExists(const QString &tableName)
{
    bool result = false;

    if (tableName.isEmpty())
    {
        qWarning() << "tableExists() with empty table name!";
    }
    else
    {
        QSqlQuery checkTable;
        if (m_dbInternalOpen) // sqlite
        {
            checkTable.exec("PRAGMA table_info(" + tableName + ");");
        }
        else if (m_dbExternalOpen) // mysql
        {
            checkTable.exec("SELECT * FROM information_schema.tables WHERE table_schema = 'theengs' AND table_name = '" + tableName + "' LIMIT 1;");
            //checkTable.exec("SELECT * FROM information_schema.TABLES WHERE TABLE_NAME = '" + tableName + "' AND TABLE_SCHEMA in (SELECT DATABASE());");
        }
        if (checkTable.first())
        {
            result = true;
        }
    }

    return result;
}

void DatabaseManager::createDatabase()
{
    if (!tableExists("version"))
    {
        qDebug() << "+ Adding 'version' table to local database";

        QSqlQuery createDbVersion;
        createDbVersion.prepare("CREATE TABLE version (dbVersion INT);");
        if (createDbVersion.exec())
        {
            QSqlQuery writeDbVersion;
            writeDbVersion.prepare("INSERT INTO version (dbVersion) VALUES (:dbVersion)");
            writeDbVersion.bindValue(":dbVersion", s_dbCurrentVersion);
            writeDbVersion.exec();
        }
        else
        {
            qWarning() << "> createDbVersion.exec() ERROR"
                       << createDbVersion.lastError().type() << ":" << createDbVersion.lastError().text();
        }
    }

    if (!tableExists("lastRun"))
    {
        qDebug() << "+ Adding 'lastRun' table to local database";

        QSqlQuery createLastRun;
        createLastRun.prepare("CREATE TABLE lastRun (lastRun DATETIME);");
        if (createLastRun.exec())
        {
            QSqlQuery writeLastRun;
            writeLastRun.prepare("INSERT INTO lastRun (lastRun) VALUES (:lastRun)");
            writeLastRun.bindValue(":lastRun", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            writeLastRun.exec();
        }
        else
        {
            qWarning() << "> createLastRun.exec() ERROR"
                       << createLastRun.lastError().type() << ":" << createLastRun.lastError().text();
        }
    }

    if (!tableExists("devices"))
    {
        qDebug() << "+ Adding 'devices' table to local database";

        QSqlQuery createDevices;
        createDevices.prepare("CREATE TABLE devices (" \
                              "deviceAddr VARCHAR(38) PRIMARY KEY," \
                              "deviceAddrMAC VARCHAR(17)," \
                              "deviceName VARCHAR(255)," \
                              "deviceModel VARCHAR(255)," \
                              "deviceFirmware VARCHAR(255)," \
                              "deviceBattery INT," \
                              "associatedName VARCHAR(255)," \
                              "locationName VARCHAR(255)," \
                              "lastSeen DATETIME," \
                              "lastSync DATETIME," \
                              "isEnabled BOOLEAN DEFAULT TRUE," \
                              "isOutside BOOLEAN DEFAULT FALSE," \
                              "manualOrderIndex INT," \
                              "settings VARCHAR(255)" \
                              ");");

        if (createDevices.exec() == false)
        {
            qWarning() << "> createDevices.exec() ERROR"
                       << createDevices.lastError().type() << ":" << createDevices.lastError().text();
        }
    }

    if (!tableExists("devicesBlacklist"))
    {
        qDebug() << "+ Adding 'devicesBlacklist' table to local database";

        QSqlQuery createDevicesBlacklist;
        createDevicesBlacklist.prepare("CREATE TABLE devicesBlacklist (" \
                                       "deviceAddr VARCHAR(38) PRIMARY KEY" \
                                       ");");

        if (createDevicesBlacklist.exec() == false)
        {
            qWarning() << "> createDevicesBlacklist.exec() ERROR"
                       << createDevicesBlacklist.lastError().type() << ":" << createDevicesBlacklist.lastError().text();
        }
    }

    if (!tableExists("plantData"))
    {
        qDebug() << "+ Adding 'plantData' table to local database";

        QSqlQuery createPlantData;
        createPlantData.prepare("CREATE TABLE plantData (" \
                                "deviceAddr VARCHAR(38)," \
                                "timestamp_rounded DATETIME," \
                                "timestamp DATETIME," \
                                  "soilMoisture INT," \
                                  "soilConductivity INT," \
                                  "soilTemperature FLOAT," \
                                  "soilPH FLOAT," \
                                  "temperature FLOAT," \
                                  "humidity FLOAT," \
                                  "luminosity INT," \
                                  "watertank FLOAT," \
                                "PRIMARY KEY(deviceAddr, timestamp_rounded)," \
                                "FOREIGN KEY(deviceAddr) REFERENCES devices(deviceAddr) ON DELETE NO ACTION ON UPDATE NO ACTION" \
                                ");");

        if (createPlantData.exec() == false)
        {
            qWarning() << "> createPlantData.exec() ERROR"
                       << createPlantData.lastError().type() << ":" << createPlantData.lastError().text();
        }
    }

    if (!tableExists("thermoData"))
    {
        qDebug() << "+ Adding 'thermoData' table to local database";

        QSqlQuery createThermoData;
        createThermoData.prepare("CREATE TABLE thermoData (" \
                                 "deviceAddr VARCHAR(38)," \
                                 "timestamp_rounded DATETIME," \
                                 "timestamp DATETIME," \
                                   "temperature FLOAT," \
                                   "humidity FLOAT," \
                                   "pressure FLOAT," \
                                 "PRIMARY KEY(deviceAddr, timestamp_rounded)," \
                                 "FOREIGN KEY(deviceAddr) REFERENCES devices(deviceAddr) ON DELETE NO ACTION ON UPDATE NO ACTION" \
                                 ");");

        if (createThermoData.exec() == false)
        {
            qWarning() << "> createThermoData.exec() ERROR"
                       << createThermoData.lastError().type() << ":" << createThermoData.lastError().text();
        }
    }

    if (!tableExists("sensorData"))
    {
        qDebug() << "+ Adding 'sensorData' table to local database";
        QSqlQuery createSensorData;
        createSensorData.prepare("CREATE TABLE sensorData (" \
                                 "deviceAddr VARCHAR(38)," \
                                 "timestamp_rounded DATETIME," \
                                 "timestamp DATETIME," \
                                   "temperature FLOAT," \
                                   "humidity FLOAT," \
                                   "pressure FLOAT," \
                                   "luminosity INT," \
                                   "uv FLOAT," \
                                   "sound FLOAT," \
                                   "water FLOAT," \
                                   "windDirection FLOAT," \
                                   "windSpeed FLOAT," \
                                   "pm1 FLOAT," \
                                   "pm25 FLOAT," \
                                   "pm10 FLOAT," \
                                   "o2 FLOAT," \
                                   "o3 FLOAT," \
                                   "co FLOAT," \
                                   "co2 FLOAT," \
                                   "no2 FLOAT," \
                                   "so2 FLOAT," \
                                   "voc FLOAT," \
                                   "hcho FLOAT," \
                                   "radioactivity FLOAT," \
                                 "PRIMARY KEY(deviceAddr, timestamp_rounded)," \
                                 "FOREIGN KEY(deviceAddr) REFERENCES devices(deviceAddr) ON DELETE NO ACTION ON UPDATE NO ACTION" \
                                 ");");

        if (createSensorData.exec() == false)
        {
            qWarning() << "> createSensorData.exec() ERROR"
                       << createSensorData.lastError().type() << ":" << createSensorData.lastError().text();
        }
    }

    if (!tableExists("sensorTheengs"))
    {
        qDebug() << "+ Adding 'sensorTheengs' table to local database";
        QSqlQuery createSensorTheengs;
        createSensorTheengs.prepare("CREATE TABLE sensorTheengs (" \
                                    "deviceAddr CHAR(38)," \
                                    "timestamp_rounded DATETIME," \
                                    "timestamp DATETIME," \
                                      "temperature1 FLOAT," \
                                      "temperature2 FLOAT," \
                                      "temperature3 FLOAT," \
                                      "temperature4 FLOAT," \
                                      "temperature5 FLOAT," \
                                      "temperature6 FLOAT," \
                                      "pressure1 INT," \
                                      "pressure2 INT," \
                                      "pressure3 INT," \
                                      "pressure4 INT," \
                                      "battery1 INT," \
                                      "battery2 INT," \
                                      "battery3 INT," \
                                      "battery4 INT," \
                                      "alarm1 BOOLEAN," \
                                      "alarm2 BOOLEAN," \
                                      "alarm3 BOOLEAN," \
                                      "alarm4 BOOLEAN," \
                                      "weight FLOAT," \
                                      "weightUnit VARCHAR(32)," \
                                      "weightMode VARCHAR(32)," \
                                      "impedance INT," \
                                      "acclX FLOAT," \
                                      "acclY FLOAT," \
                                      "acclZ FLOAT," \
                                      "gyroX FLOAT," \
                                      "gyroY FLOAT," \
                                      "gyroZ FLOAT," \
                                      "steps INT," \
                                    "PRIMARY KEY(deviceAddr, timestamp_rounded)," \
                                    "FOREIGN KEY(deviceAddr) REFERENCES devices(deviceAddr) ON DELETE NO ACTION ON UPDATE NO ACTION" \
                                    ");");

        if (createSensorTheengs.exec() == false)
        {
            qWarning() << "> createSensorTheengs.exec() ERROR"
                       << createSensorTheengs.lastError().type() << ":" << createSensorTheengs.lastError().text();
        }
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

void DatabaseManager::migrateDatabase()
{
    int dbVersion = 0;

    QSqlQuery readVersion;
    readVersion.prepare("SELECT dbVersion FROM version");
    readVersion.exec();
    if (readVersion.first())
    {
        dbVersion = readVersion.value(0).toInt();
        //qDebug() << "dbVersion is #" << dbVersion;
    }
    readVersion.finish();

    if (dbVersion > 0 && dbVersion != s_dbCurrentVersion)
    {
        if (dbVersion == 1)
        {
            if (migrate_v1v2())
            {
                QSqlQuery updateDbVersion("UPDATE version SET dbVersion=2");
                dbVersion = 2;
            }
        }

        if (dbVersion == 2)
        {
            if (migrate_v2v3())
            {
                QSqlQuery updateDbVersion("UPDATE version SET dbVersion=3");
                dbVersion = 3;
            }
        }
    }
}

/* ************************************************************************** */

bool DatabaseManager::migrate_v1v2()
{
    qWarning() << "DatabaseManager::migrate_v1v2()";

    return false;
}

/* ************************************************************************** */

bool DatabaseManager::migrate_v2v3()
{
    qWarning() << "DatabaseManager::migrate_v2v3()";

    return false;
}

/* ************************************************************************** */
