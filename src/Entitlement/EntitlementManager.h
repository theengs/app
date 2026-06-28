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

#ifndef ENTITLEMENT_MANAGER_H
#define ENTITLEMENT_MANAGER_H
/* ************************************************************************** */

#include <QObject>
#include <QString>

//! Read-only, QML-facing view of the persisted entitlement provenance (the
//! Release-A buyer stamp; see Entitlement/Provenance.h). This is the single
//! gate the UI binds to — ``entitlement.isPro`` — per the grandfathering spec
//! P3 ("no feature queries the store directly").
//!
//! On this PAID build the grant is unconditional, so ``isPro`` is true for
//! every stamped install; the freemium build will back the same property with
//! the full resolution engine without changing this QML contract. Read-only:
//! the stamp is written by Entitlement::stampOnceAtStartup(), never here.
class EntitlementManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isPro READ isPro NOTIFY changed)
    Q_PROPERTY(bool stamped READ isStamped NOTIFY changed)
    Q_PROPERTY(QString proSource READ proSource NOTIFY changed)
    Q_PROPERTY(QString acquisitionEra READ acquisitionEra NOTIFY changed)

public:
    explicit EntitlementManager(QObject *parent = nullptr);

    bool isPro() const { return m_isPro; }
    bool isStamped() const { return m_stamped; }
    QString proSource() const { return m_proSource; }
    QString acquisitionEra() const { return m_acquisitionEra; }

    //! Re-read the persisted provenance. Call after the startup stamp so the
    //! freshly-written record is reflected; cheap and idempotent.
    Q_INVOKABLE void refresh();

signals:
    void changed();

private:
    bool    m_isPro = false;
    bool    m_stamped = false;
    QString m_proSource;
    QString m_acquisitionEra;
};

/* ************************************************************************** */
#endif // ENTITLEMENT_MANAGER_H
