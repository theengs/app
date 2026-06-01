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
#include "ForegroundNotifier.h"

#include <QtCore/private/qandroidextras_p.h>
#include <QCoreApplication>
#include <QDateTime>
#include <QMetaObject>
#include <QDebug>
#include <jni.h>

/* ************************************************************************** */

AndroidService *AndroidService::s_instance = nullptr;

// Called by TheengsAndroidService.mWorkReceiver when the AllowWhileIdle alarm
// fires (on the service's Java/Binder thread). Marshal onto the Qt service
// thread so gotowork() touches DeviceManager/QObjects safely.
static void jni_onWorkAlarm(JNIEnv *, jclass)
{
    if (AndroidService::s_instance)
        QMetaObject::invokeMethod(AndroidService::s_instance, "gotowork", Qt::QueuedConnection);
}

/* ************************************************************************** */

AndroidService::AndroidService(QObject *parent) : QObject(parent)
{
    s_instance = this;

    DatabaseManager::getInstance();

    m_settingsManager = SettingsManager::getInstance();

    //m_notificationManager = NotificationManager::getInstance(); // DEBUG
    //m_notificationManager->setNotification("AndroidService starting", QDateTime::currentDateTime().toString());

    // Bind the Java `nativeOnWorkAlarm()` declaration to jni_onWorkAlarm above
    // so the alarm broadcast can re-enter C++.
    {
        QJniEnvironment env;
        const JNINativeMethod m = {
            const_cast<char *>("nativeOnWorkAlarm"),
            const_cast<char *>("()V"),
            reinterpret_cast<void *>(jni_onWorkAlarm)
        };
        if (!env.registerNativeMethods("com/theengs/app/TheengsAndroidService", &m, 1))
            qWarning() << "AndroidService: registerNativeMethods(nativeOnWorkAlarm) failed";
    }

    // Pushes live BLE-reading content into the foreground-service
    // notification owned by the :qt_service process.
    m_foregroundNotifier = new ForegroundNotifier(this);

    // First background tick in 3 min (matches the prior QTimer(3) startup
    // behaviour); gotowork() then reschedules at updateIntervalBackground.
    scheduleNextWork(3);
}

AndroidService::~AndroidService()
{
    if (s_instance == this) s_instance = nullptr;
}

/* ************************************************************************** */

void AndroidService::scheduleNextWork(int workInterval_mins)
{
    QJniObject::callStaticMethod<void>("com.theengs.app.TheengsAndroidService",
                                       "scheduleWork", "(I)V",
                                       static_cast<jint>(workInterval_mins * 60 * 1000));
}

void AndroidService::gotowork()
{
    //m_notificationManager = NotificationManager::getInstance(); // DEBUG
    //m_notificationManager->setNotification("AndroidService gotowork", QDateTime::currentDateTime().toString());

    // Reload settings, user might have changed them
    m_settingsManager->reloadSettings();

    // Is the background service enabled?
    if (m_settingsManager->getSysTray())
    {
        // Reload the device manager, a new scan might have occured
        if (m_deviceManager) delete m_deviceManager;
        m_deviceManager = new DeviceManager(true);

        // Hand the fresh DeviceManager to the foreground-service notifier so
        // it re-attaches to the per-Device dataUpdated() signals (the old
        // manager's Devices were destroyed with it).
        if (m_foregroundNotifier) m_foregroundNotifier->attachDeviceManager(m_deviceManager);

        // Device manager is operational?
        if (m_deviceManager &&
            m_deviceManager->checkBluetooth() &&
            m_deviceManager->areDevicesAvailable())
        {
            // Reload MQTT settings, user might have changed them
            if (m_settingsManager->getMQTT())
            {
                MqttManager *mq = MqttManager::getInstance();
                mq->reconnect();
            }

            // Start background refresh process
            m_deviceManager->refreshDevices_background();
        }
    }

    // Schedule the next tick (AllowWhileIdle alarm — fires through deep doze).
    scheduleNextWork(m_settingsManager->getUpdateIntervalBackground());
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
