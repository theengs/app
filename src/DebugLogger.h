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

#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QMutex>

// Captures Qt log messages to a rotating file inside the app's data dir
// so users can attach a single log via the platform share sheet when
// reporting bugs. Pass-through to the default Qt handler is preserved.
//
// qDebug() calls are elided at compile time when CMAKE_BUILD_TYPE=Release
// (CMakeLists.txt sets QT_NO_DEBUG_OUTPUT there), so the log will only
// contain qInfo/qWarning/qCritical on release builds. Ship Debug or
// RelWithDebInfo to test users when verbose tracing is needed.
class DebugLogger : public QObject
{
    Q_OBJECT

    static DebugLogger *instance;
    DebugLogger();
    ~DebugLogger();

    QFile *m_file = nullptr;
    mutable QMutex m_mutex;
    QString m_path;
    static constexpr qint64 kRotateBytes = 1 * 1024 * 1024; // 1 MiB

    static void messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
    void write(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
    void rotateIfNeeded();

public:
    static DebugLogger *getInstance();
    static void install();

    Q_INVOKABLE QString logFilePath() const { return m_path; }
    Q_INVOKABLE qint64 sizeBytes() const;
    Q_INVOKABLE void clear();

    // Build a STORED-format .zip in the app cache containing the current log
    // plus the rolled-over .1 backup (if it exists). Returns the absolute
    // path of the archive, or an empty string on failure / nothing to share.
    Q_INVOKABLE QString archive();
};

#endif // DEBUG_LOGGER_H
