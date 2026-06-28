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

#include <QtTest>

#include "Entitlement/EntitlementEras.h"

class TestEntitlementEras : public QObject
{
    Q_OBJECT
private slots:
    void paidEraRowExists();
    void paidEraIsCurrentForStampRelease();
};

void TestEntitlementEras::paidEraRowExists()
{
    bool found = false;
    for (int i = 0; i < EntitlementEras::kErasCount; ++i)
    {
        if (QString::fromLatin1(EntitlementEras::kEras[i].id) == "paid_era")
        {
            found = true;
            break;
        }
    }
    QVERIFY(found);
}

void TestEntitlementEras::paidEraIsCurrentForStampRelease()
{
    const auto &era = EntitlementEras::currentEraForStampRelease();
    QCOMPARE(QString::fromLatin1(era.id), QStringLiteral("paid_era"));
}

QTEST_APPLESS_MAIN(TestEntitlementEras)
#include "test_entitlement_eras.moc"
