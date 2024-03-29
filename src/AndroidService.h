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
#include <QTimer>

class DeviceManager;
class SettingsManager;
class NotificationManager;

#if defined(Q_OS_ANDROID)
/* ************************************************************************** */

/*!
 * \brief The AndroidService class
 */
class AndroidService: public QObject
{
    Q_OBJECT

    QTimer m_workTimer;
    void setWorkTimer(int workInterval_mins = 5);

    DeviceManager *m_deviceManager = nullptr;
    SettingsManager *m_settingsManager = nullptr;
    NotificationManager *m_notificationManager = nullptr;

private slots:
    void gotowork();

public:
    AndroidService(QObject *parent = nullptr);
    ~AndroidService();

    static void service_start();
    static void service_stop();
    static void service_registerCommService();
};

/* ************************************************************************** */
#endif // Q_OS_ANDROID
#endif // ANDROID_SERVICE_H
