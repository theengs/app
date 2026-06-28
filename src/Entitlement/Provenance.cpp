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

#include "Provenance.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>

namespace {
// Top-level keys this binary understands. Any other top-level key encountered
// during fromJson() is stashed into Provenance::extras to be re-emitted on the
// next save — that keeps older binaries non-destructive against newer schemas.
const QStringList kKnownTopLevel = {
    QStringLiteral("schema"),
    QStringLiteral("firstSeenEpoch"),
    QStringLiteral("firstSeenBuild"),
    QStringLiteral("acquisitionEra"),
    QStringLiteral("entitlements"),
    QStringLiteral("transitionLog"),
};
}

QJsonObject Provenance::toJson() const
{
    QJsonObject obj = extras; // start with forward-compat extras, then overwrite known keys
    obj.insert("schema", schema);
    obj.insert("firstSeenEpoch", static_cast<double>(firstSeenEpoch));
    obj.insert("firstSeenBuild", firstSeenBuild);
    obj.insert("acquisitionEra", acquisitionEra);

    QJsonObject entitlements;
    QJsonObject proObj;
    proObj.insert("granted", pro.granted);
    proObj.insert("source", pro.source);
    proObj.insert("grantedAt", static_cast<double>(pro.grantedAt));
    entitlements.insert("pro", proObj);
    obj.insert("entitlements", entitlements);

    QJsonArray log;
    for (const auto &t : transitionLog)
    {
        QJsonObject e;
        e.insert("era", t.era);
        e.insert("event", t.event);
        e.insert("at", static_cast<double>(t.at));
        log.append(e);
    }
    obj.insert("transitionLog", log);
    return obj;
}

QString Provenance::toJsonString() const
{
    return QString::fromUtf8(QJsonDocument(toJson()).toJson(QJsonDocument::Compact));
}

Provenance Provenance::fromJson(const QJsonObject &obj, bool *ok)
{
    Provenance p;
    p.schema         = obj.value("schema").toInt(2);
    p.firstSeenEpoch = static_cast<qint64>(obj.value("firstSeenEpoch").toDouble(0));
    p.firstSeenBuild = obj.value("firstSeenBuild").toInt(0);
    p.acquisitionEra = obj.value("acquisitionEra").toString();

    const QJsonObject entitlements = obj.value("entitlements").toObject();
    const QJsonObject proObj = entitlements.value("pro").toObject();
    p.pro.granted   = proObj.value("granted").toBool(false);
    p.pro.source    = proObj.value("source").toString();
    p.pro.grantedAt = static_cast<qint64>(proObj.value("grantedAt").toDouble(0));

    const QJsonArray log = obj.value("transitionLog").toArray();
    for (const auto &v : log)
    {
        const QJsonObject e = v.toObject();
        TransitionEntry t;
        t.era   = e.value("era").toString();
        t.event = e.value("event").toString();
        t.at    = static_cast<qint64>(e.value("at").toDouble(0));
        p.transitionLog.append(t);
    }

    // Stash any unknown top-level fields into extras so a re-save preserves them.
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it)
    {
        if (!kKnownTopLevel.contains(it.key()))
            p.extras.insert(it.key(), it.value());
    }

    if (ok) *ok = true;
    return p;
}

Provenance Provenance::fromJsonString(const QString &s, bool *ok)
{
    QJsonParseError err;
    const auto doc = QJsonDocument::fromJson(s.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
    {
        if (ok) *ok = false;
        return Provenance{};
    }
    return fromJson(doc.object(), ok);
}
