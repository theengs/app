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
#include <QJsonDocument>

#include "Entitlement/Provenance.h"

class TestProvenance : public QObject
{
    Q_OBJECT
private slots:
    void emptyProvenanceIsNotStamped();
    void stampedProvenanceRoundTripsJson();
    void schemaFieldIsTwo();
    void unknownExtraFieldsArePreservedOnRoundTrip();
    void malformedJsonStringFailsCleanly();
};

void TestProvenance::emptyProvenanceIsNotStamped()
{
    Provenance p;
    QVERIFY(!p.isStamped());
    QCOMPARE(p.schema, 2);
}

void TestProvenance::stampedProvenanceRoundTripsJson()
{
    Provenance p;
    p.firstSeenEpoch = 1719500000000LL;
    p.firstSeenBuild = 142;
    p.acquisitionEra = "paid_era";
    p.pro.granted = true;
    p.pro.source = "GF_ANDROID_STAMP";
    p.pro.grantedAt = 1719500000000LL;
    Provenance::TransitionEntry entry;
    entry.era = "paid_era";
    entry.event = "stamped";
    entry.at = 1719500000000LL;
    p.transitionLog.append(entry);

    const QString s = p.toJsonString();
    bool ok = false;
    Provenance round = Provenance::fromJsonString(s, &ok);
    QVERIFY(ok);
    QCOMPARE(round.schema, 2);
    QCOMPARE(round.firstSeenEpoch, 1719500000000LL);
    QCOMPARE(round.firstSeenBuild, 142);
    QCOMPARE(round.acquisitionEra, QStringLiteral("paid_era"));
    QVERIFY(round.pro.granted);
    QCOMPARE(round.pro.source, QStringLiteral("GF_ANDROID_STAMP"));
    QCOMPARE(round.pro.grantedAt, 1719500000000LL);
    QCOMPARE(round.transitionLog.size(), 1);
    QCOMPARE(round.transitionLog.first().era, QStringLiteral("paid_era"));
    QCOMPARE(round.transitionLog.first().event, QStringLiteral("stamped"));
    QCOMPARE(round.transitionLog.first().at, 1719500000000LL);
}

void TestProvenance::schemaFieldIsTwo()
{
    Provenance p;
    const QJsonObject obj = p.toJson();
    QCOMPARE(obj.value("schema").toInt(), 2);
}

void TestProvenance::unknownExtraFieldsArePreservedOnRoundTrip()
{
    // Forward-compat: a future schema may add fields. Loading and re-saving
    // must not silently drop them.
    QJsonObject extra;
    extra.insert("schema", 2);
    extra.insert("firstSeenEpoch", 1.0);
    extra.insert("futureField", QStringLiteral("payload"));

    bool ok = false;
    Provenance p = Provenance::fromJson(extra, &ok);
    QVERIFY(ok);
    const QJsonObject out = p.toJson();
    QVERIFY(out.contains("futureField"));
    QCOMPARE(out.value("futureField").toString(), QStringLiteral("payload"));
}

void TestProvenance::malformedJsonStringFailsCleanly()
{
    bool ok = true;
    Provenance p = Provenance::fromJsonString(QStringLiteral("not-json"), &ok);
    QVERIFY(!ok);
    QVERIFY(!p.isStamped());
}

QTEST_APPLESS_MAIN(TestProvenance)
#include "test_provenance.moc"
