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

    static PermissionManager *instance;
    PermissionManager();
    ~PermissionManager();

    static const int s_waittimeout = 12000; // in ms
    static const int s_waittimeout_interval = 33; // in ms

    bool m_bluetoothPermission = false;
    bool m_calendarPermission = false;
    bool m_cameraPermission = false;
    bool m_contactsPermission = false;
    bool m_locationPermission = false;
    bool m_microphonePermission = false;

    void setBluetoothPermission(bool perm);
    void setCalendarPermission(bool perm);
    void setCameraPermission(bool perm);
    void setContactsPermission(bool perm);
    void setLocationPermission(bool perm);
    void setMicrophonePermission(bool perm);

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

public:
    static PermissionManager *getInstance();

    bool hasBluetoothPermission() const { return m_bluetoothPermission; }
    bool hasCalendarPermission() const { return m_calendarPermission; }
    bool hasCameraPermission() const { return m_cameraPermission; }
    bool hasContactsPermission() const { return m_contactsPermission; }
    bool hasLocationPermission() const { return m_locationPermission; }
    bool hasMicrophonePermission() const { return m_microphonePermission; }

    Q_INVOKABLE bool requestBluetoothPermission();
    Q_INVOKABLE bool checkBluetoothPermission();
    Q_INVOKABLE bool waitBluetoothPermission();

    Q_INVOKABLE bool requestCameraPermission();
    Q_INVOKABLE bool checkCameraPermission();
    Q_INVOKABLE bool waitCameraPermission();

    Q_INVOKABLE bool requestLocationPermission();
    Q_INVOKABLE bool checkLocationPermission();
    Q_INVOKABLE bool waitLocationPermission();
};

/* ************************************************************************** */
#endif // PERMISSION_MANAGER_H
