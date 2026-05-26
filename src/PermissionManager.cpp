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

#include "PermissionManager.h"

#include <QCoreApplication>
#include <QDebug>

#include <QPermission>
#include <QThread>

#if defined(Q_OS_ANDROID)
#include <QJniObject>
#include <QCoreApplication>
#endif

/* ************************************************************************** */
/* ************************************************************************** */

PermissionManager *PermissionManager::instance = nullptr;

PermissionManager *PermissionManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new PermissionManager();
        return instance;
    }

    return instance;
}

PermissionManager::PermissionManager()
{
    //
}

PermissionManager::~PermissionManager()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

void PermissionManager::setBluetoothPermission(bool perm)
{
    if (m_bluetoothPermission != perm)
    {
        m_bluetoothPermission = perm;
        Q_EMIT bluetoothPermissionChanged();
    }
}

void PermissionManager::setCalendarPermission(bool perm)
{
    if (m_calendarPermission != perm)
    {
        m_calendarPermission = perm;
        Q_EMIT calendarPermissionChanged();
    }
}

void PermissionManager::setCameraPermission(bool perm)
{
    if (m_cameraPermission != perm)
    {
        m_cameraPermission = perm;
        Q_EMIT cameraPermissionChanged();
    }
}

void PermissionManager::setContactsPermission(bool perm)
{
    if (m_contactsPermission != perm)
    {
        m_contactsPermission = perm;
        Q_EMIT contactsPermissionChanged();
    }
}

void PermissionManager::setLocationPermission(bool perm)
{
    if (m_locationPermission != perm)
    {
        m_locationPermission = perm;
        Q_EMIT locationPermissionChanged();
    }
}

void PermissionManager::setMicrophonePermission(bool perm)
{
    if (m_microphonePermission != perm)
    {
        m_microphonePermission = perm;
        Q_EMIT microphonePermissionChanged();
    }
}

void PermissionManager::setNotificationPermission(bool perm)
{
    if (m_notificationPermission != perm)
    {
        m_notificationPermission = perm;
        Q_EMIT notificationPermissionChanged();
    }
}

void PermissionManager::setExactAlarmPermission(bool perm)
{
    if (m_exactAlarmPermission != perm)
    {
        m_exactAlarmPermission = perm;
        Q_EMIT exactAlarmPermissionChanged();
    }
}

/* ************************************************************************** */
/* ************************************************************************** */

bool PermissionManager::requestBluetoothPermission()
{
    QBluetoothPermission bluetoothPermission;
    bluetoothPermission.setCommunicationModes(QBluetoothPermission::Access);

    switch (qApp->checkPermission(bluetoothPermission))
    {
    case Qt::PermissionStatus::Granted:
        setBluetoothPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
    case Qt::PermissionStatus::Undetermined:
        qDebug() << "Requesting BLUETOOTH permission...";
        qApp->requestPermission(bluetoothPermission, this, &PermissionManager::requestBluetoothPermission_results);
        break;
    }

    return m_bluetoothPermission;
}

void PermissionManager::requestBluetoothPermission_results(const QPermission &permission)
{
    // evaluate the results
    switch (permission.status())
    {
    case Qt::PermissionStatus::Granted:
        setBluetoothPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
    case Qt::PermissionStatus::Undetermined:
        setBluetoothPermission(false);
        break;
    }
}

bool PermissionManager::checkBluetoothPermission()
{
    QBluetoothPermission bluetoothPermission;
    bluetoothPermission.setCommunicationModes(QBluetoothPermission::Access);

    switch (qApp->checkPermission(bluetoothPermission))
    {
    case Qt::PermissionStatus::Granted:
        setBluetoothPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
        setBluetoothPermission(false);
        break;
    case Qt::PermissionStatus::Undetermined:
        break;
    }

    return m_bluetoothPermission;
}

bool PermissionManager::waitBluetoothPermission()
{
    QBluetoothPermission bluetoothPermission;
    bluetoothPermission.setCommunicationModes(QBluetoothPermission::Access);

    if (qApp->checkPermission(bluetoothPermission) == Qt::PermissionStatus::Granted)
    {
        setBluetoothPermission(true);
    }
    else
    {
        qDebug() << "Requesting BLUETOOTH permission...";
        qApp->requestPermission(bluetoothPermission, this, &PermissionManager::requestBluetoothPermission_results);

        int timeout = s_waittimeout;
        while (timeout > 0)
        {
            QThread::msleep(s_waittimeout_interval);
            timeout -= s_waittimeout_interval;

            switch (qApp->checkPermission(bluetoothPermission))
            {
            case Qt::PermissionStatus::Granted:
                setBluetoothPermission(true);
                return m_bluetoothPermission;

            case Qt::PermissionStatus::Denied:
                setBluetoothPermission(false);
                return m_bluetoothPermission;

            case Qt::PermissionStatus::Undetermined:
                break;
            }
        }
    }

    return m_bluetoothPermission;
}

/* ************************************************************************** */
/* ************************************************************************** */

bool PermissionManager::requestCameraPermission()
{
    QCameraPermission cameraPermission;

    switch (qApp->checkPermission(cameraPermission))
    {
    case Qt::PermissionStatus::Granted:
        setCameraPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
    case Qt::PermissionStatus::Undetermined:
        qDebug() << "Requesting CAMERA permission...";
        qApp->requestPermission(cameraPermission, this, &PermissionManager::requestCameraPermission_results);
        break;
    }

    return m_cameraPermission;
}

void PermissionManager::requestCameraPermission_results(const QPermission &permission)
{
    switch (permission.status())
    {
    case Qt::PermissionStatus::Granted:
        setCameraPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
    case Qt::PermissionStatus::Undetermined:
        setCameraPermission(false);
        break;
    }
}

bool PermissionManager::checkCameraPermission()
{
    QCameraPermission cameraPermission;
    switch (qApp->checkPermission(cameraPermission))
    {
    case Qt::PermissionStatus::Granted:
        setCameraPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
        setCameraPermission(false);
        break;
    case Qt::PermissionStatus::Undetermined:
        break;
    }

    return m_cameraPermission;
}

bool PermissionManager::waitCameraPermission()
{
    QCameraPermission cameraPermission;

    if (qApp->checkPermission(cameraPermission) == Qt::PermissionStatus::Granted)
    {
        setCameraPermission(true);
    }
    else
    {
        qDebug() << "Requesting CAMERA permission...";
        qApp->requestPermission(cameraPermission, this, &PermissionManager::requestCameraPermission_results);

        int timeout = s_waittimeout;
        while (timeout > 0)
        {
            QThread::msleep(s_waittimeout_interval);
            timeout -= s_waittimeout_interval;

            switch (qApp->checkPermission(cameraPermission))
            {
            case Qt::PermissionStatus::Granted:
                setCameraPermission(true);
                return m_cameraPermission;

            case Qt::PermissionStatus::Denied:
                setCameraPermission(false);
                return m_cameraPermission;

            case Qt::PermissionStatus::Undetermined:
                break;
            }
        }
    }

    return m_cameraPermission;
}

/* ************************************************************************** */
/* ************************************************************************** */

bool PermissionManager::requestLocationPermission()
{
    QLocationPermission locationPermission;
    locationPermission.setAccuracy(QLocationPermission::Precise);
    locationPermission.setAvailability(QLocationPermission::WhenInUse);

    switch (qApp->checkPermission(locationPermission))
    {
    case Qt::PermissionStatus::Granted:
        setLocationPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
    case Qt::PermissionStatus::Undetermined:
        qDebug() << "Requesting LOCATION permission...";
        qApp->requestPermission(locationPermission, this, &PermissionManager::requestLocationPermission_results);
        break;
    }

    return m_locationPermission;
}

void PermissionManager::requestLocationPermission_results(const QPermission &permission)
{
    // evaluate the results
    switch (permission.status())
    {
    case Qt::PermissionStatus::Granted:
        setLocationPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
    case Qt::PermissionStatus::Undetermined:
        setLocationPermission(false);
        break;
    }
}

bool PermissionManager::checkLocationPermission()
{
    QLocationPermission locationPermission;
    locationPermission.setAccuracy(QLocationPermission::Precise);
    locationPermission.setAvailability(QLocationPermission::WhenInUse);

    switch (qApp->checkPermission(locationPermission))
    {
    case Qt::PermissionStatus::Granted:
        setLocationPermission(true);
        break;
    case Qt::PermissionStatus::Denied:
        setLocationPermission(false);
        break;
    case Qt::PermissionStatus::Undetermined:
        break;
    }

    return m_locationPermission;
}

bool PermissionManager::waitLocationPermission()
{
    QLocationPermission locationPermission;
    locationPermission.setAccuracy(QLocationPermission::Precise);
    locationPermission.setAvailability(QLocationPermission::WhenInUse);

    if (qApp->checkPermission(locationPermission) == Qt::PermissionStatus::Granted)
    {
        setLocationPermission(true);
    }
    else
    {
        qDebug() << "Requesting LOCATION permission...";
        qApp->requestPermission(locationPermission, this, &PermissionManager::requestLocationPermission_results);

        int timeout = s_waittimeout;
        while (timeout > 0)
        {
            QThread::msleep(s_waittimeout_interval);
            timeout -= s_waittimeout_interval;

            switch (qApp->checkPermission(locationPermission))
            {
            case Qt::PermissionStatus::Granted:
                setLocationPermission(true);
                return m_locationPermission;

            case Qt::PermissionStatus::Denied:
                setLocationPermission(false);
                return m_locationPermission;

            case Qt::PermissionStatus::Undetermined:
                break;
            }
        }
    }

    return m_locationPermission;
}

/* ************************************************************************** */
/* ************************************************************************** */

bool PermissionManager::checkNotificationPermission()
{
#if defined(Q_OS_ANDROID)
    // The Java helper handles the SDK_INT < 33 short-circuit and the
    // ContextCompat.checkSelfPermission call. We only need to thread a
    // Context across the JNI boundary.
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
    {
        setNotificationPermission(false);
        return m_notificationPermission;
    }

    jboolean granted = QJniObject::callStaticMethod<jboolean>(
        "com/theengs/app/TheengsAndroidService",
        "isPostNotificationsGranted",
        "(Landroid/content/Context;)Z",
        context.object<jobject>());
    setNotificationPermission(granted == JNI_TRUE);
    return m_notificationPermission;
#else
    // No runtime gate on desktop / iOS — the manifest / Info.plist (or
    // platform defaults) decides; tracking it as ``granted`` keeps QML
    // bindings uniform across platforms.
    setNotificationPermission(true);
    return m_notificationPermission;
#endif
}

bool PermissionManager::requestNotificationPermission()
{
#if defined(Q_OS_ANDROID)
    // Skip the round-trip if Android already says yes (covers both
    // pre-API-33 phones via the helper's SDK_INT check, and any later
    // call after the user has already granted).
    if (checkNotificationPermission()) return true;

    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (!activity.isValid()) return false;

    // Asynchronous. The system dialog appears once the Activity is in a
    // resumed state; the result lands on onRequestPermissionsResult and
    // the FGS notification starts being painted on the next post. QML
    // observers re-read the property after Qt.ApplicationActive.
    qDebug() << "Requesting POST_NOTIFICATIONS permission...";
    QJniObject::callStaticMethod<void>(
        "com/theengs/app/TheengsAndroidService",
        "requestPostNotifications",
        "(Landroid/app/Activity;)V",
        activity.object<jobject>());
    return m_notificationPermission;
#else
    setNotificationPermission(true);
    return m_notificationPermission;
#endif
}

/* ************************************************************************** */

bool PermissionManager::checkExactAlarmPermission()
{
#if defined(Q_OS_ANDROID)
    // The Java helper short-circuits to true below API 31 and otherwise
    // returns AlarmManager.canScheduleExactAlarms().
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
    {
        setExactAlarmPermission(false);
        return m_exactAlarmPermission;
    }

    jboolean granted = QJniObject::callStaticMethod<jboolean>(
        "com/theengs/app/TheengsAndroidService",
        "canScheduleExactAlarms",
        "(Landroid/content/Context;)Z",
        context.object<jobject>());
    setExactAlarmPermission(granted == JNI_TRUE);
    return m_exactAlarmPermission;
#else
    setExactAlarmPermission(true);
    return m_exactAlarmPermission;
#endif
}

bool PermissionManager::requestExactAlarmPermission()
{
#if defined(Q_OS_ANDROID)
    // Already granted (or pre-API-31, or 31-33 auto-grant) — nothing to do.
    if (checkExactAlarmPermission()) return true;

    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid()) return false;

    // Opens Settings → Alarms & reminders for this app. Asynchronous; the
    // user grants out-of-app and we re-check the property on resume.
    qDebug() << "Requesting SCHEDULE_EXACT_ALARM (opening settings)...";
    QJniObject::callStaticMethod<void>(
        "com/theengs/app/TheengsAndroidService",
        "requestScheduleExactAlarms",
        "(Landroid/content/Context;)V",
        context.object<jobject>());
    return m_exactAlarmPermission;
#else
    setExactAlarmPermission(true);
    return m_exactAlarmPermission;
#endif
}

/* ************************************************************************** */
/* ************************************************************************** */
