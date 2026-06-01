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

#ifndef PERMISSION_MANAGER_H
#define PERMISSION_MANAGER_H
/* ************************************************************************** */

#include <QObject>

#if !QT_CONFIG(permissions)
#error "QtPermission is not available. Qt 6.6 is required!"
#endif

#include <QPermission>

/* ************************************************************************** */

/*!
 * \brief The PermissionManager class
 *
 * REQUIRES Qt 6.6+
 * https://doc.qt.io/qt-6/permissions.html
 *
 * - https://doc.qt.io/qt-6/qbluetoothpermission.html
 * - https://doc.qt.io/qt-6/qcalendarpermission.html
 * - https://doc.qt.io/qt-6/qcamerapermission.html
 * - https://doc.qt.io/qt-6/qcontactpermission.html
 * - https://doc.qt.io/qt-6/qlocationpermission.html
 * - https://doc.qt.io/qt-6/qmicrophonepermission.html
 */
class PermissionManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool bluetoothPermission READ hasBluetoothPermission NOTIFY bluetoothPermissionChanged)
    Q_PROPERTY(bool calendarPermission READ hasCalendarPermission NOTIFY calendarPermissionChanged)
    Q_PROPERTY(bool cameraPermission READ checkCameraPermission NOTIFY cameraPermissionChanged)
    Q_PROPERTY(bool contactsPermission READ hasContactsPermission NOTIFY contactsPermissionChanged)
    Q_PROPERTY(bool locationPermission READ hasLocationPermission NOTIFY locationPermissionChanged)
    Q_PROPERTY(bool microphonePermission READ hasMicrophonePermission NOTIFY microphonePermissionChanged)
    // Android 13+ (API 33+) POST_NOTIFICATIONS — runtime-gated and required
    // for the foreground service to post its persistent notification. Qt
    // 6.10 has no typed QPermission for this, so we go through JNI (the
    // Java side lives in TheengsAndroidService).
    Q_PROPERTY(bool notificationPermission READ hasNotificationPermission NOTIFY notificationPermissionChanged)
    // Android 12+ (API 31+) SCHEDULE_EXACT_ALARM — lets the background-work
    // alarm fire at the exact "Update interval" through doze. Auto-granted on
    // API 31-33, but DENIED by default on API 34+ (the user must grant it in
    // Settings → Alarms & reminders). True on older Android / non-Android.
    Q_PROPERTY(bool exactAlarmPermission READ hasExactAlarmPermission NOTIFY exactAlarmPermissionChanged)
    // Android 6+ (API 23+) battery-optimization exemption. False when the app
    // is still optimized (Doze/app-standby can defer its background-work
    // alarms); the user grants it through a one-tap system dialog. True on
    // older Android / non-Android.
    Q_PROPERTY(bool batteryOptimizationPermission READ hasBatteryOptimizationPermission NOTIFY batteryOptimizationPermissionChanged)

    static PermissionManager *instance;
    PermissionManager();
    ~PermissionManager();

    static const int s_waittimeout = 10000; // in ms
    static const int s_waittimeout_interval = 33; // in ms

    bool m_bluetoothPermission = false;
    bool m_calendarPermission = false;
    bool m_cameraPermission = false;
    bool m_contactsPermission = false;
    bool m_locationPermission = false;
    bool m_microphonePermission = false;
    bool m_notificationPermission = false;
    bool m_exactAlarmPermission = true; // default true: only Android 12+ can deny it
    bool m_batteryOptimizationPermission = true; // default true: only Android 6+ can deny it

    void setBluetoothPermission(bool perm);
    void setCalendarPermission(bool perm);
    void setCameraPermission(bool perm);
    void setContactsPermission(bool perm);
    void setLocationPermission(bool perm);
    void setMicrophonePermission(bool perm);
    void setNotificationPermission(bool perm);
    void setExactAlarmPermission(bool perm);
    void setBatteryOptimizationPermission(bool perm);

    void requestBluetoothPermission_results(const QPermission &permission);
    void requestCameraPermission_results(const QPermission &permission);
    void requestLocationPermission_results(const QPermission &permission);

Q_SIGNALS:
    void bluetoothPermissionChanged();
    void calendarPermissionChanged();
    void cameraPermissionChanged();
    void contactsPermissionChanged();
    void locationPermissionChanged();
    void microphonePermissionChanged();
    void notificationPermissionChanged();
    void exactAlarmPermissionChanged();
    void batteryOptimizationPermissionChanged();

public:
    static PermissionManager *getInstance();

    bool hasBluetoothPermission() const { return m_bluetoothPermission; }
    bool hasCalendarPermission() const { return m_calendarPermission; }
    bool hasCameraPermission() const { return m_cameraPermission; }
    bool hasContactsPermission() const { return m_contactsPermission; }
    bool hasLocationPermission() const { return m_locationPermission; }
    bool hasMicrophonePermission() const { return m_microphonePermission; }
    bool hasNotificationPermission() const { return m_notificationPermission; }
    bool hasExactAlarmPermission() const { return m_exactAlarmPermission; }
    bool hasBatteryOptimizationPermission() const { return m_batteryOptimizationPermission; }

    Q_INVOKABLE bool requestBluetoothPermission();
    Q_INVOKABLE bool checkBluetoothPermission();
    Q_INVOKABLE bool waitBluetoothPermission();

    Q_INVOKABLE bool requestCameraPermission();
    Q_INVOKABLE bool checkCameraPermission();
    Q_INVOKABLE bool waitCameraPermission();

    Q_INVOKABLE bool requestLocationPermission();
    Q_INVOKABLE bool checkLocationPermission();
    Q_INVOKABLE bool waitLocationPermission();

    // POST_NOTIFICATIONS request goes through JNI (no typed QPermission
    // class for it in Qt 6.10). No ``wait`` variant — the dialog is
    // fire-and-forget and the FGS picks up the grant on its next post
    // attempt; QML can re-check via the property when the activity resumes.
    Q_INVOKABLE bool requestNotificationPermission();
    Q_INVOKABLE bool checkNotificationPermission();

    // SCHEDULE_EXACT_ALARM (Android 12+). check returns current state;
    // request opens Settings → Alarms & reminders (ACTION_REQUEST_SCHEDULE_
    // EXACT_ALARM) on API 31+. No-op / always-granted elsewhere. Like the
    // notification one, fire-and-forget: re-check the property on resume.
    Q_INVOKABLE bool requestExactAlarmPermission();
    Q_INVOKABLE bool checkExactAlarmPermission();

    // REQUEST_IGNORE_BATTERY_OPTIMIZATIONS (Android 6+). check returns current
    // exemption state; request shows the system one-tap dialog. No-op /
    // always-granted elsewhere. Fire-and-forget: re-check on resume.
    Q_INVOKABLE bool requestBatteryOptimizationPermission();
    Q_INVOKABLE bool checkBatteryOptimizationPermission();
};

/* ************************************************************************** */
#endif // PERMISSION_MANAGER_H
