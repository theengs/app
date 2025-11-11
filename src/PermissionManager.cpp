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
        //qApp->requestPermission(bluetoothPermission, [this](const QPermission &permission) { checkBluetoothPermission(); });
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
