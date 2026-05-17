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

#ifndef FOREGROUND_NOTIFIER_H
#define FOREGROUND_NOTIFIER_H
/* ************************************************************************** */

#include <QtGlobal>

#if defined(Q_OS_ANDROID)

#include <QObject>
#include <QPointer>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QSet>
#include <QTimer>

class Device;
class DeviceManager;

/*!
 * \brief Feeds live BLE-reading content into the Theengs Android foreground
 *        service notification. Lives in the :qt_service process and pushes
 *        updates via JNI to TheengsAndroidService.updateNotification().
 *
 * Triggered by per-Device dataUpdated() signals, debounced to at most one
 * notification rebuild every ~3 s, plus a 10 s tick that refreshes the
 * "Xs ago" math even when no fresh readings arrive.
 */
class ForegroundNotifier: public QObject
{
    Q_OBJECT

public:
    explicit ForegroundNotifier(QObject *parent = nullptr);
    ~ForegroundNotifier() override;

    /*! Re-points the notifier at a (possibly new) DeviceManager. Safe to
     *  call repeatedly — re-attaches to the current device list and the
     *  manager's devicesListUpdated signal so newly-discovered Devices
     *  are picked up too. */
    void attachDeviceManager(DeviceManager *dm);

private slots:
    void onDeviceUpdated();
    void onDeviceListChanged();
    void flush();

private:
    struct Reading {
        QPointer<Device> device;
        QString label;        // location if set, else device name
        QString valueSummary; // e.g. "21.4 °C · 56 %" — may be empty
        QDateTime when;
    };

    void rewireDeviceList();
    QString formatValueSummary(Device *d) const;
    QString relativeAge(const QDateTime &when, const QDateTime &now) const;
    void pushUpdate();

    QPointer<DeviceManager> m_dm;
    QSet<Device *> m_wired;     // devices we've connected dataUpdated() on
    QList<Reading> m_recent;    // most-recent first; capped at kMaxRecent

    QTimer m_debounce;          // single-shot trailing-edge, kDebounceMs
    QTimer m_refresh;           // periodic, kRefreshMs

    static constexpr int kMaxRecent = 6;
    static constexpr int kDebounceMs = 3000;
    static constexpr int kRefreshMs = 10000;
    static constexpr int kActiveWindowSecs = 5 * 60;
};

#endif // Q_OS_ANDROID

/* ************************************************************************** */
#endif // FOREGROUND_NOTIFIER_H
