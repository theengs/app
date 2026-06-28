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

#ifndef ENTITLEMENT_H
#define ENTITLEMENT_H
/* ************************************************************************** */

namespace Entitlement
{
    //! Called once during app startup, immediately after SettingsManager is
    //! initialized. Idempotent: writes the provenance stamp exactly once,
    //! never overwrites an existing one. No-op on subsequent launches.
    //!
    //! Invariant: this binary is the PAID build. Every install executing
    //! this code is, by definition, a legitimate paid_era buyer, so the
    //! stamp grants Pro with source=GF_ANDROID_STAMP unconditionally.
    void stampOnceAtStartup();
}

/* ************************************************************************** */
#endif // ENTITLEMENT_H
