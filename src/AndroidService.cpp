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

#include "AndroidService.h"

#if defined(Q_OS_ANDROID)

#include "DatabaseManager.h"
#include "SettingsManager.h"
#include "DeviceManager.h"
#include "MqttManager.h"
#include "NotificationManager.h"

#include <QtCore/private/qandroidextras_p.h>
#include <QCoreApplication>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

/* ************************************************************************** */

AndroidService::AndroidService(QObject *parent) : QObject(parent)
{
    DatabaseManager *db = DatabaseManager::getInstance();

    m_settingsManager = SettingsManager::getInstance();

    //m_notificationManager = NotificationManager::getInstance(); // DEBUG
    //m_notificationManager->setNotification2("AndroidService starting", QDateTime::currentDateTime().toString());

    m_deviceManager = new DeviceManager(true);

    // Configure update timer
    connect(&m_workTimer, &QTimer::timeout, this, &AndroidService::gotowork);
    setWorkTimer(3);
}

AndroidService::~AndroidService()
{
    //
}

/* ************************************************************************** */

void AndroidService::setWorkTimer(int workInterval_mins)
{
    m_workTimer.setInterval(workInterval_mins*60*1000);
    m_workTimer.start();
}

void AndroidService::gotowork()
{
    if (m_deviceManager)
    {
        delete m_deviceManager;
        m_deviceManager = new DeviceManager(true);
    }

    if (m_deviceManager && m_deviceManager->areDevicesAvailable())
    {
        // Restart timer
        setWorkTimer(m_settingsManager->getUpdateIntervalBackground());

        // Reload a few things
        m_settingsManager->reloadSettings();

        if (m_settingsManager->getMQTT())
        {
            MqttManager *mq = MqttManager::getInstance();
            mq->reconnect();
        }

        //m_notificationManager = NotificationManager::getInstance(); // DEBUG
        //m_notificationManager->setNotification2("AndroidService working", QDateTime::currentDateTime().toString());

        // Start background refresh process
        m_deviceManager->refreshDevices_background();
    }
}

/* ************************************************************************** */

void AndroidService::service_start()
{
    QJniObject::callStaticMethod<void>("com.theengs.app.TheengsAndroidService",
                                       "serviceStart",
                                       "(Landroid/content/Context;)V",
                                       QNativeInterface::QAndroidApplication::context());
}

void AndroidService::service_stop()
{
    QJniObject::callStaticMethod<void>("com.theengs.app.TheengsAndroidService",
                                       "serviceStop", "(Landroid/content/Context;)V",
                                       QNativeInterface::QAndroidApplication::context());
}

void AndroidService::service_registerCommService()
{
    QJniEnvironment env;
    jclass javaClass = env.findClass("com/theengs/app/ActivityUtils");
    QJniObject classObject(javaClass);

    classObject.callMethod<void>("registerServiceBroadcastReceiver",
                                 "(Landroid/content/Context;)V",
                                 QNativeInterface::QAndroidApplication::context());
}

/* ************************************************************************** */
#endif // Q_OS_ANDROID
