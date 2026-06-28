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

#ifndef PROVENANCE_H
#define PROVENANCE_H
/* ************************************************************************** */

#include <QString>
#include <QList>
#include <QJsonObject>

// Provenance record. Written exactly once at first launch, immutable
// thereafter. JSON-serialized into QSettings under the key
// "provenance/json_v2" so the entire object is one atomic value and
// forward-compatible via the explicit `schema` field. Unknown JSON fields
// are round-tripped through `extras` so a future schema bump does not
// silently truncate an older client's write.
struct Provenance
{
    int     schema         = 2;
    qint64  firstSeenEpoch = 0;   //!< ms since epoch, immutable once non-zero
    int     firstSeenBuild = 0;   //!< platform-specific build int at first launch
    QString acquisitionEra;       //!< e.g. "paid_era"

    struct ProGrant
    {
        bool    granted   = false;
        QString source;            //!< e.g. "GF_ANDROID_STAMP"
        qint64  grantedAt = 0;
    };
    ProGrant pro;

    struct TransitionEntry
    {
        QString era;
        QString event;
        qint64  at = 0;
    };
    QList<TransitionEntry> transitionLog;

    //! Forward-compat holding bay for fields a future schema may add. Any
    //! top-level keys we don't recognize get stashed here and re-emitted on
    //! the next save, so an older binary doesn't silently truncate a newer
    //! object that was written by a future client.
    QJsonObject extras;

    bool isStamped() const { return firstSeenEpoch != 0; }

    QJsonObject toJson() const;
    QString     toJsonString() const;

    static Provenance fromJson(const QJsonObject &obj, bool *ok = nullptr);
    static Provenance fromJsonString(const QString &s, bool *ok = nullptr);
};

/* ************************************************************************** */
#endif // PROVENANCE_H
