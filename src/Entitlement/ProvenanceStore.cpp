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

#include "ProvenanceStore.h"

#include <QSettings>

namespace ProvenanceStore
{

const char *const kSettingsKey = "provenance/json_v2";

Provenance load(QSettings &settings)
{
    if (!settings.contains(kSettingsKey)) return Provenance{};
    const QString s = settings.value(kSettingsKey).toString();
    if (s.isEmpty()) return Provenance{};
    bool ok = false;
    Provenance p = Provenance::fromJsonString(s, &ok);
    if (!ok) return Provenance{};
    return p;
}

void save(QSettings &settings, const Provenance &p)
{
    settings.setValue(kSettingsKey, p.toJsonString());
}

Provenance stampIfFirstLaunch(QSettings &settings,
                              qint64 nowMs,
                              int currentBuild,
                              const QString &eraId,
                              const QString &source)
{
    Provenance existing = load(settings);
    if (existing.isStamped()) return existing;

    Provenance p;
    p.firstSeenEpoch = nowMs;
    p.firstSeenBuild = currentBuild;
    p.acquisitionEra = eraId;
    p.pro.granted    = true;
    p.pro.source     = source;
    p.pro.grantedAt  = nowMs;
    Provenance::TransitionEntry entry;
    entry.era   = eraId;
    entry.event = QStringLiteral("stamped");
    entry.at    = nowMs;
    p.transitionLog.append(entry);

    save(settings, p);
    settings.sync();
    return p;
}

} // namespace ProvenanceStore
