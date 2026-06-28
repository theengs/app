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

#include "EntitlementManager.h"

#include "Provenance.h"
#include "ProvenanceStore.h"

#include <QCoreApplication>
#include <QSettings>

EntitlementManager::EntitlementManager(QObject *parent) : QObject(parent)
{
    refresh();
}

void EntitlementManager::refresh()
{
    // Same QSettings keying as Entitlement::stampOnceAtStartup() so we read the
    // exact record the startup stamp wrote (org/app both "Theengs").
    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    const Provenance p = ProvenanceStore::load(settings);

    const bool pro = p.pro.granted;
    const bool stamped = p.isStamped();
    if (pro == m_isPro && stamped == m_stamped
        && p.pro.source == m_proSource && p.acquisitionEra == m_acquisitionEra)
        return;

    m_isPro = pro;
    m_stamped = stamped;
    m_proSource = p.pro.source;
    m_acquisitionEra = p.acquisitionEra;
    emit changed();
}
