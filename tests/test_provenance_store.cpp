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
#include <QSettings>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QFile>

#include "Entitlement/ProvenanceStore.h"
#include "Entitlement/Provenance.h"

class TestProvenanceStore : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void firstLaunchWritesStamp();
    void secondLaunchIsIdempotent();
    void simulatedUpgradePreservesFirstSeen();
    void loadOfMissingKeyReturnsEmpty();
    void loadOfMalformedJsonReturnsEmpty();

private:
    QTemporaryDir m_dir;
    QString settingsPath() const { return m_dir.path() + "/test.conf"; }
};

void TestProvenanceStore::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    QVERIFY(m_dir.isValid());
}

void TestProvenanceStore::init()
{
    // Start each test with a clean settings file.
    QFile::remove(settingsPath());
}

void TestProvenanceStore::firstLaunchWritesStamp()
{
    QSettings s(settingsPath(), QSettings::IniFormat);
    const qint64 t = 1719500000000LL;
    Provenance p = ProvenanceStore::stampIfFirstLaunch(s, t, 142, "paid_era", "GF_ANDROID_STAMP");

    QVERIFY(p.isStamped());
    QCOMPARE(p.firstSeenEpoch, t);
    QCOMPARE(p.firstSeenBuild, 142);
    QCOMPARE(p.acquisitionEra, QStringLiteral("paid_era"));
    QVERIFY(p.pro.granted);
    QCOMPARE(p.pro.source, QStringLiteral("GF_ANDROID_STAMP"));
    QCOMPARE(p.pro.grantedAt, t);
    QCOMPARE(p.transitionLog.size(), 1);
    QCOMPARE(p.transitionLog.first().event, QStringLiteral("stamped"));

    s.sync();
    Provenance reload = ProvenanceStore::load(s);
    QCOMPARE(reload.firstSeenEpoch, t);
    QCOMPARE(reload.firstSeenBuild, 142);
    QCOMPARE(reload.acquisitionEra, QStringLiteral("paid_era"));
}

void TestProvenanceStore::secondLaunchIsIdempotent()
{
    QSettings s(settingsPath(), QSettings::IniFormat);
    const qint64 t1 = 1719500000000LL;
    ProvenanceStore::stampIfFirstLaunch(s, t1, 142, "paid_era", "GF_ANDROID_STAMP");
    s.sync();

    const qint64 t2 = 1719600000000LL;
    Provenance p2 = ProvenanceStore::stampIfFirstLaunch(s, t2, 999, "different_era", "GF_ANDROID_STAMP");

    // All originally-stamped fields must be preserved exactly.
    QCOMPARE(p2.firstSeenEpoch, t1);
    QCOMPARE(p2.firstSeenBuild, 142);
    QCOMPARE(p2.acquisitionEra, QStringLiteral("paid_era"));
    QCOMPARE(p2.pro.source, QStringLiteral("GF_ANDROID_STAMP"));
    QCOMPARE(p2.pro.grantedAt, t1);
    QCOMPARE(p2.transitionLog.size(), 1); // no duplicate log entry
}

void TestProvenanceStore::simulatedUpgradePreservesFirstSeen()
{
    // Pre-populate the same way an old install would.
    {
        QSettings s(settingsPath(), QSettings::IniFormat);
        ProvenanceStore::stampIfFirstLaunch(s, 1719500000000LL, 100, "paid_era", "GF_ANDROID_STAMP");
        s.sync();
    }
    // Now "upgrade" — re-open and re-call stamp with a higher build number.
    QSettings s2(settingsPath(), QSettings::IniFormat);
    Provenance p = ProvenanceStore::stampIfFirstLaunch(s2, 1720000000000LL, 200, "paid_era", "GF_ANDROID_STAMP");

    QCOMPARE(p.firstSeenBuild, 100);
    QCOMPARE(p.firstSeenEpoch, 1719500000000LL);
    QCOMPARE(p.pro.grantedAt, 1719500000000LL);
}

void TestProvenanceStore::loadOfMissingKeyReturnsEmpty()
{
    QSettings s(settingsPath(), QSettings::IniFormat);
    Provenance p = ProvenanceStore::load(s);
    QVERIFY(!p.isStamped());
}

void TestProvenanceStore::loadOfMalformedJsonReturnsEmpty()
{
    QSettings s(settingsPath(), QSettings::IniFormat);
    s.setValue(ProvenanceStore::kSettingsKey, QStringLiteral("{not-json"));
    s.sync();
    Provenance p = ProvenanceStore::load(s);
    QVERIFY(!p.isStamped());
}

QTEST_APPLESS_MAIN(TestProvenanceStore)
#include "test_provenance_store.moc"
