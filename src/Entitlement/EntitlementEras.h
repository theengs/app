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

#ifndef ENTITLEMENT_ERAS_H
#define ENTITLEMENT_ERAS_H
/* ************************************************************************** */

#include <QtGlobal>

// Boundary constants ledger. Append-only — never edit past rows; only append
// a new row at each future cohort boundary, freezing that boundary's build
// number / epoch in place. This file is the single source of truth that any
// future cohort decision reads.
//
// TODO(florian): Fill the four constants below from the store consoles
// before release. They are intentional placeholders — the engineer who
// authored this code cannot read the store consoles. The PR description
// must list these constants explicitly.
namespace EntitlementEras
{

// iOS: last paid CFBundleVersion. Read from App Store Connect.
constexpr int kIosLastPaidBuild = 0; // TODO(florian): fill from store console

// iOS: first CFBundleVersion of the next cohort. = kIosLastPaidBuild + 1.
constexpr int kIosFirstFreeBuild = 0; // TODO(florian): fill from store console

// Android: UTC epoch (ms) at the relevant cohort boundary. Set when the
// boundary becomes effective, not before.
constexpr qint64 kAndroidFreeFlipEpochMs = 0; // TODO(florian): fill at boundary

// Android: last paid Play versionCode. Informational; not used by the stamp
// logic, recorded here so future cohort math has it on hand.
constexpr int kAndroidLastPaidVersionCode = 0; // TODO(florian): fill from store console

struct Era
{
    const char *id;                   //!< canonical era slug, written into provenance
    int         iosBuildMax;          //!< <= boundary, 0 = N/A
    int         iosBuildMin;          //!< >= boundary, 0 = N/A
    qint64      androidBeforeEpochMs; //!< install < boundary, 0 = N/A
    qint64      androidAfterEpochMs;  //!< install >= boundary, 0 = N/A
};

extern const Era kEras[];
extern const int kErasCount;

//! Returns the era that the current stamp release must assign. Every install
//! running this binary is, by definition, in paid_era.
const Era &currentEraForStampRelease();

} // namespace EntitlementEras

/* ************************************************************************** */
#endif // ENTITLEMENT_ERAS_H
