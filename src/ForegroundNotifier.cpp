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

#include "ForegroundNotifier.h"

#if defined(Q_OS_ANDROID)

#include "DeviceManager.h"
#include "DeviceFilter.h"
#include "device.h"

#include <QJniObject>
#include <QStringList>
#include <QVariant>
#include <QtMath>
#include <QDebug>

/* ************************************************************************** */

namespace {

// Ordered list of Q_PROPERTY names the C++ Device subclasses expose; we
// surface the first 1-2 that resolve to a sensible numeric value. The unit
// strings here are the canonical SI / common ones used elsewhere in the app;
// the notification is informational so we don't localise the unit per-locale
// in v1 (this stays consistent with how Settings already labels values).
struct KnownProp { const char *prop; const char *unit; int precision; };
constexpr KnownProp kKnownProps[] = {
    { "temperature", "\u00b0C", 1 },   // °C
    { "humidity",    "%",       0 },
    { "co2",         "ppm",     0 },
    { "voc",         "ppb",     0 },
    { "pm25",        "\u00b5g/m\u00b3", 0 }, // µg/m³
    { "pressure",    "hPa",     0 },
    { "luminosity",  "lx",      0 },
    { "weight",      "kg",      2 },
};

bool readDouble(const QVariant &v, double &out)
{
    if (!v.isValid()) return false;
    bool ok = false;
    double d = v.toDouble(&ok);
    if (!ok) return false;
    if (qIsNaN(d)) return false;
    out = d;
    return true;
}

} // namespace

/* ************************************************************************** */

ForegroundNotifier::ForegroundNotifier(QObject *parent) : QObject(parent)
{
    m_debounce.setSingleShot(true);
    m_debounce.setInterval(kDebounceMs);
    connect(&m_debounce, &QTimer::timeout, this, &ForegroundNotifier::flush);

    m_refresh.setInterval(kRefreshMs);
    connect(&m_refresh, &QTimer::timeout, this, &ForegroundNotifier::flush);
    m_refresh.start();
}

ForegroundNotifier::~ForegroundNotifier() = default;

/* ************************************************************************** */

void ForegroundNotifier::attachDeviceManager(DeviceManager *dm)
{
    if (m_dm == dm && dm != nullptr) {
        rewireDeviceList();
        return;
    }

    if (m_dm) {
        disconnect(m_dm, nullptr, this, nullptr);
    }
    // QObject::sender() returns nullptr for already-destroyed senders, so
    // m_wired entries become safe stale pointers — clear and rewire.
    m_wired.clear();
    m_dm = dm;

    if (m_dm) {
        connect(m_dm, &DeviceManager::devicesListUpdated,
                this, &ForegroundNotifier::onDeviceListChanged);
        rewireDeviceList();
    }
}

void ForegroundNotifier::rewireDeviceList()
{
    if (!m_dm) return;
    for (Device *d : m_dm->getDeviceList()) {
        if (!d) continue;
        if (m_wired.contains(d)) continue;
        connect(d, &Device::dataUpdated,
                this, &ForegroundNotifier::onDeviceUpdated,
                Qt::UniqueConnection);
        m_wired.insert(d);
    }
}

void ForegroundNotifier::onDeviceListChanged()
{
    rewireDeviceList();
}

/* ************************************************************************** */

void ForegroundNotifier::onDeviceUpdated()
{
    Device *d = qobject_cast<Device *>(sender());
    if (!d) return;

    Reading r;
    r.device = d;
    // Label fallback: user location > BLE name > Theengs model > last 5 of
    // MAC. Devices early in their discovery often have no name yet, so we
    // need a stable identifier for the bigText line rather than "(unnamed)".
    const QString loc   = d->getLocationName();
    const QString name  = d->getName();
    const QString model = d->getModel();
    const QString addr  = d->getAddress();
    if (!loc.isEmpty())        r.label = loc;
    else if (!name.isEmpty())  r.label = name;
    else if (!model.isEmpty()) r.label = model;
    else if (!addr.isEmpty())  r.label = QStringLiteral("…") + addr.right(5);
    else                       r.label = QStringLiteral("(BLE)");
    r.valueSummary = formatValueSummary(d);
    r.when = QDateTime::currentDateTime();

    // De-dup by device pointer: most recent reading per device wins.
    for (int i = 0; i < m_recent.size(); ++i) {
        if (m_recent[i].device == d) {
            m_recent.removeAt(i);
            break;
        }
    }
    m_recent.prepend(r);
    while (m_recent.size() > kMaxRecent) m_recent.removeLast();

    if (!m_debounce.isActive()) m_debounce.start();
}

/* ************************************************************************** */

QString ForegroundNotifier::formatValueSummary(Device *d) const
{
    if (!d) return {};
    QStringList parts;
    for (const auto &kp : kKnownProps) {
        QVariant v = d->property(kp.prop);
        double x = 0.0;
        if (!readDouble(v, x)) continue;
        // Theengs Device subclasses use -99 / -99.0 as a "no reading yet"
        // sentinel across many sensors (temperature, humidity, co2, voc,
        // pm25...). Drop those so the notification doesn't surface garbage.
        if (x <= -98.9) continue;
        // Skip zeros for properties that almost always read as 0 when
        // uninitialised (humidity / co2 / etc.). Temperature stays even at
        // 0.0 since it's a legitimate value.
        if (qFuzzyIsNull(x) && QByteArray(kp.prop) != QByteArray("temperature"))
            continue;
        parts.append(QStringLiteral("%1\u202F%2")
                     .arg(QString::number(x, 'f', kp.precision))
                     .arg(QString::fromUtf8(kp.unit)));
        if (parts.size() >= 2) break; // cap at two values per device
    }
    return parts.join(QStringLiteral(" \u00b7 "));
}

QString ForegroundNotifier::relativeAge(const QDateTime &when, const QDateTime &now) const
{
    qint64 secs = when.secsTo(now);
    if (secs < 0) secs = 0;
    if (secs < 60) return tr("%1 s ago").arg(secs);
    if (secs < 3600) return tr("%1 min ago").arg(secs / 60);
    return tr("%1 h ago").arg(secs / 3600);
}

/* ************************************************************************** */

void ForegroundNotifier::flush()
{
    if (m_recent.isEmpty()) return; // nothing to show yet — keep defaults
    pushUpdate();
}

void ForegroundNotifier::pushUpdate()
{
    const QDateTime now = QDateTime::currentDateTime();

    // "active" = distinct labels seen within the last kActiveWindowSecs.
    int active = 0;
    for (const Reading &r : std::as_const(m_recent)) {
        if (r.when.secsTo(now) <= kActiveWindowSecs) ++active;
    }
    if (active < 1) active = m_recent.size();

    const QDateTime &mostRecent = m_recent.first().when;
    const QString title = tr("Theengs \u00b7 Scanning Bluetooth sensors");
    // Qt's tr("%n …", n) plural form only switches catalogues when one is
    // loaded; with no Android-side translation catalog, %n leaves "(s)"
    // visible. Build the singular/plural form ourselves.
    const QString sensorWord = (active == 1) ? tr("sensor") : tr("sensors");
    const QString body = tr("%1 %2 active \u00b7 last reading %3")
                         .arg(active)
                         .arg(sensorWord)
                         .arg(relativeAge(mostRecent, now));

    QStringList lines;
    lines.reserve(m_recent.size());
    for (const Reading &r : std::as_const(m_recent)) {
        QString line = r.label;
        if (!r.valueSummary.isEmpty()) {
            line += QStringLiteral(" \u00b7 ") + r.valueSummary;
        }
        line += QStringLiteral(" \u00b7 ") + relativeAge(r.when, now);
        lines.append(line);
    }
    const QString bigText = lines.join(QChar('\n'));

    // JNI: TheengsAndroidService.updateNotification(String, String, String)
    QJniObject jTitle = QJniObject::fromString(title);
    QJniObject jBody  = QJniObject::fromString(body);
    QJniObject jBig   = QJniObject::fromString(bigText);
    QJniObject::callStaticMethod<void>(
        "com/theengs/app/TheengsAndroidService",
        "updateNotification",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
        jTitle.object<jstring>(),
        jBody.object<jstring>(),
        jBig.object<jstring>()
    );
}

/* ************************************************************************** */

#endif // Q_OS_ANDROID
