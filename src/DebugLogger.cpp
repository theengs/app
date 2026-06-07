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

#include "DebugLogger.h"

#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QMutexLocker>
#include <QFileInfo>

DebugLogger *DebugLogger::instance = nullptr;
static QtMessageHandler s_defaultHandler = nullptr;

DebugLogger::DebugLogger()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs";
    QDir().mkpath(dir);
    m_path = dir + "/theengs.log";

    m_file = new QFile(m_path);
    m_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

DebugLogger::~DebugLogger()
{
    if (m_file)
    {
        m_file->close();
        delete m_file;
    }
}

DebugLogger *DebugLogger::getInstance()
{
    if (!instance) instance = new DebugLogger();
    return instance;
}

void DebugLogger::install()
{
    getInstance();
    s_defaultHandler = qInstallMessageHandler(&DebugLogger::messageHandler);
}

void DebugLogger::messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    if (s_defaultHandler) s_defaultHandler(type, ctx, msg);
    if (instance) instance->write(type, ctx, msg);
}

void DebugLogger::write(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    QMutexLocker locker(&m_mutex);
    if (!m_file || !m_file->isOpen()) return;

    const char *typeStr = "?";
    switch (type)
    {
        case QtDebugMsg:    typeStr = "DBG"; break;
        case QtInfoMsg:     typeStr = "INF"; break;
        case QtWarningMsg:  typeStr = "WRN"; break;
        case QtCriticalMsg: typeStr = "ERR"; break;
        case QtFatalMsg:    typeStr = "FTL"; break;
    }

    QString line = QStringLiteral("[%1] %2 %3")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"),
             QString::fromLatin1(typeStr),
             msg);
    if (ctx.file && ctx.line > 0)
        line += QStringLiteral(" (%1:%2)").arg(QString::fromLatin1(ctx.file)).arg(ctx.line);
    line += QChar('\n');

    m_file->write(line.toUtf8());
    m_file->flush();

    rotateIfNeeded();
}

void DebugLogger::rotateIfNeeded()
{
    if (!m_file || m_file->size() < kRotateBytes) return;

    m_file->close();
    QFile::remove(m_path + QStringLiteral(".1"));
    QFile::rename(m_path, m_path + QStringLiteral(".1"));
    m_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

qint64 DebugLogger::sizeBytes() const
{
    QMutexLocker locker(&m_mutex);
    return m_file ? m_file->size() : 0;
}

void DebugLogger::clear()
{
    QMutexLocker locker(&m_mutex);
    if (!m_file) return;
    m_file->close();
    QFile::remove(m_path + QStringLiteral(".1"));
    m_file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
}

/* ************************************************************************** */
/* Minimal STORED-format .zip writer — enough to package the current log plus
 * the rolled-over backup. Standard CRC-32 (poly 0xEDB88320), all multi-byte
 * fields little-endian, no compression. Avoids the dependency on QuaZip /
 * private QZipWriter so the patch stays self-contained. */

static quint32 s_crc32_table[256];
static bool s_crc32_table_ready = false;

static void crc32_init()
{
    for (quint32 i = 0; i < 256; ++i)
    {
        quint32 c = i;
        for (int k = 0; k < 8; ++k)
            c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        s_crc32_table[i] = c;
    }
    s_crc32_table_ready = true;
}

static quint32 crc32(const QByteArray &data)
{
    if (!s_crc32_table_ready) crc32_init();
    quint32 c = 0xFFFFFFFFu;
    for (char b : data)
        c = s_crc32_table[(c ^ static_cast<uchar>(b)) & 0xFFu] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

static inline void putU16(QByteArray &out, quint16 v)
{
    out.append(static_cast<char>(v & 0xFF));
    out.append(static_cast<char>((v >> 8) & 0xFF));
}

static inline void putU32(QByteArray &out, quint32 v)
{
    out.append(static_cast<char>(v & 0xFF));
    out.append(static_cast<char>((v >> 8) & 0xFF));
    out.append(static_cast<char>((v >> 16) & 0xFF));
    out.append(static_cast<char>((v >> 24) & 0xFF));
}

QString DebugLogger::archive()
{
    QMutexLocker locker(&m_mutex);
    if (m_file && m_file->isOpen()) m_file->flush();

    struct Entry { QByteArray name; QByteArray data; quint32 crc; quint32 offset; };
    QList<Entry> entries;

    auto loadIfExists = [&](const QString &path, const QString &name) {
        QFile f(path);
        if (!f.exists() || !f.open(QIODevice::ReadOnly)) return;
        const QByteArray d = f.readAll();
        if (!d.isEmpty())
            entries.append({name.toUtf8(), d, crc32(d), 0u});
    };

    // Older first so unpacking gives chronological order.
    loadIfExists(m_path + QStringLiteral(".1"), QStringLiteral("theengs.log.1"));
    loadIfExists(m_path, QStringLiteral("theengs.log"));

    if (entries.isEmpty()) return QString();

    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(cacheDir);
    const QString zipPath = cacheDir + QStringLiteral("/theengs-logs-")
        + QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss"))
        + QStringLiteral(".zip");

    QFile out(zipPath);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) return QString();

    for (Entry &e : entries)
    {
        e.offset = static_cast<quint32>(out.pos());
        QByteArray hdr;
        putU32(hdr, 0x04034b50u);                   // local file header sig
        putU16(hdr, 20);                            // version needed
        putU16(hdr, 0);                             // flags
        putU16(hdr, 0);                             // method (STORED)
        putU16(hdr, 0); putU16(hdr, 0);             // mod time, mod date
        putU32(hdr, e.crc);
        putU32(hdr, static_cast<quint32>(e.data.size())); // compressed
        putU32(hdr, static_cast<quint32>(e.data.size())); // uncompressed
        putU16(hdr, static_cast<quint16>(e.name.size()));
        putU16(hdr, 0);                             // extra length
        out.write(hdr);
        out.write(e.name);
        out.write(e.data);
    }

    const quint32 cdOffset = static_cast<quint32>(out.pos());
    for (const Entry &e : entries)
    {
        QByteArray cd;
        putU32(cd, 0x02014b50u);                    // central dir sig
        putU16(cd, 0x0014);                         // version made by
        putU16(cd, 20);                             // version needed
        putU16(cd, 0); putU16(cd, 0);               // flags, method
        putU16(cd, 0); putU16(cd, 0);               // mod time, mod date
        putU32(cd, e.crc);
        putU32(cd, static_cast<quint32>(e.data.size()));
        putU32(cd, static_cast<quint32>(e.data.size()));
        putU16(cd, static_cast<quint16>(e.name.size()));
        putU16(cd, 0); putU16(cd, 0);               // extra, comment len
        putU16(cd, 0); putU16(cd, 0);               // disk start, internal attrs
        putU32(cd, 0);                              // external attrs
        putU32(cd, e.offset);
        out.write(cd);
        out.write(e.name);
    }
    const quint32 cdSize = static_cast<quint32>(out.pos()) - cdOffset;

    QByteArray eocd;
    putU32(eocd, 0x06054b50u);                      // EOCD sig
    putU16(eocd, 0); putU16(eocd, 0);               // disk num, disk with CD
    putU16(eocd, static_cast<quint16>(entries.size()));
    putU16(eocd, static_cast<quint16>(entries.size()));
    putU32(eocd, cdSize);
    putU32(eocd, cdOffset);
    putU16(eocd, 0);                                // comment len
    out.write(eocd);
    out.close();

    return zipPath;
}
