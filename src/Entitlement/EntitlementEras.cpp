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

#include "EntitlementEras.h"

namespace EntitlementEras
{

const Era kEras[] = {
    // paid_era: every install up to and including the last paid build /
    // pre-boundary.
    {   "paid_era",
        /*iosBuildMax=*/         kIosLastPaidBuild,
        /*iosBuildMin=*/         0,
        /*androidBeforeEpochMs=*/ kAndroidFreeFlipEpochMs,
        /*androidAfterEpochMs=*/  0 },

    // next_era: placeholder for the next cohort so the ledger ships with the
    // forward boundary already shaped. Entitlements for this row are decided
    // by the release that activates it, not by the current stamp release.
    {   "next_era",
        /*iosBuildMax=*/         0,
        /*iosBuildMin=*/         kIosFirstFreeBuild,
        /*androidBeforeEpochMs=*/ 0,
        /*androidAfterEpochMs=*/  kAndroidFreeFlipEpochMs },
};

const int kErasCount = static_cast<int>(sizeof(kEras) / sizeof(kEras[0]));

const Era &currentEraForStampRelease()
{
    // kEras[0] is paid_era and is asserted by the unit test.
    return kEras[0];
}

} // namespace EntitlementEras
