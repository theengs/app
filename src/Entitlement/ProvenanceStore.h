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

#ifndef PROVENANCE_STORE_H
#define PROVENANCE_STORE_H
/* ************************************************************************** */

#include "Provenance.h"

class QSettings;

// Pure functions over an externally-owned QSettings instance. No global state,
// no clock access — unit tests drive these with a temp QSettings file and an
// injected `nowMs`. The Release A startup glue (Entitlement::stampOnceAtStartup)
// wires the real clock + the real QSettings into stampIfFirstLaunch().
namespace ProvenanceStore
{
    //! QSettings key under which the JSON-serialized Provenance lives.
    extern const char *const kSettingsKey; // "provenance/json_v2"

    //! Reads and parses the provenance object. Returns an unstamped Provenance
    //! when the key is absent or the JSON is malformed (never throws).
    Provenance load(QSettings &settings);

    //! Overwrites the persisted JSON unconditionally. Callers must NOT use this
    //! to mutate an already-stamped object; use stampIfFirstLaunch instead.
    void save(QSettings &settings, const Provenance &p);

    //! Idempotent stamp. If `load(settings)` returns an unstamped Provenance,
    //! a fully populated stamped Provenance is written and returned. If it
    //! returns an already-stamped Provenance, the existing record is returned
    //! unchanged. Used by Entitlement::stampOnceAtStartup at app launch.
    Provenance stampIfFirstLaunch(QSettings &settings,
                                  qint64 nowMs,
                                  int currentBuild,
                                  const QString &eraId,
                                  const QString &source);
}

/* ************************************************************************** */
#endif // PROVENANCE_STORE_H
