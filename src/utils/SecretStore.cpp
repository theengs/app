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

#include "utils/SecretStore.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QDebug>

#include <qtkeychain/keychain.h>

using namespace QKeychain;

/* ************************************************************************** */

QString SecretStore::service()
{
    return QStringLiteral("Theengs");
}

QString SecretStore::readSync(const QString &key)
{
    ReadPasswordJob job(service());
    job.setAutoDelete(false);
    job.setKey(key);

    // Startup runs before the main event loop, so drive a local one until the
    // keychain job reports finished. This keeps readSync() synchronous so the
    // secret is in memory before the first MQTT/MySQL connection attempt.
    bool finished = false;
    QObject::connect(&job, &Job::finished, &job, [&finished](Job *) { finished = true; });
    job.start();
    while (!finished)
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 50);

    if (job.error())
    {
        // EntryNotFound is expected on a fresh install / after deletion.
        if (job.error() != QKeychain::EntryNotFound)
            qWarning() << "SecretStore::readSync(" << key << ") failed:" << job.errorString();
        return QString();
    }
    return job.textData();
}

void SecretStore::write(const QString &key, const QString &secret)
{
    auto *job = new WritePasswordJob(service());
    job->setAutoDelete(true);
    job->setKey(key);
    job->setTextData(secret);
    QObject::connect(job, &Job::finished, job, [key](Job *j) {
        if (j->error())
            qWarning() << "SecretStore::write(" << key << ") failed:" << j->errorString();
    });
    job->start();
}

void SecretStore::remove(const QString &key)
{
    auto *job = new DeletePasswordJob(service());
    job->setAutoDelete(true);
    job->setKey(key);
    job->start();
}

/* ************************************************************************** */
