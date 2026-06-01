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

#ifndef ANDROID_SERVICE_H
#define ANDROID_SERVICE_H
/* ************************************************************************** */

#include <QtGlobal>
#include <QObject>

class DeviceManager;
class SettingsManager;
class NotificationManager;
class ForegroundNotifier;

#if defined(Q_OS_ANDROID)
/* ************************************************************************** */

/*!
 * \brief The AndroidService class
 */
class AndroidService: public QObject
{
    Q_OBJECT

    // Background work is driven by an AllowWhileIdle AlarmManager alarm (Java
    // side), not a QTimer: a non-wakeup QTimer is deferred to doze maintenance
    // windows (device-measured ~1-3h gaps), so the user's "Update interval"
    // was not honoured in deep doze. scheduleNextWork() asks Java to set the
    // next alarm; the alarm fires nativeOnWorkAlarm() -> gotowork().
    void scheduleNextWork(int workInterval_mins);

    DeviceManager *m_deviceManager = nullptr;
    SettingsManager *m_settingsManager = nullptr;
    NotificationManager *m_notificationManager = nullptr;
    ForegroundNotifier *m_foregroundNotifier = nullptr;

private slots:
    void gotowork();

public:
    AndroidService(QObject *parent = nullptr);
    ~AndroidService();

    // Set in the ctor; used by the alarm JNI callback to post gotowork() onto
    // this object's (Qt service) thread.
    static AndroidService *s_instance;

    static void service_start();
    static void service_stop();
    static void service_registerCommService();
};

/* ************************************************************************** */
#endif // Q_OS_ANDROID
#endif // ANDROID_SERVICE_H
