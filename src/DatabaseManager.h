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

#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H
/* ************************************************************************** */

#include <QObject>
#include <QString>

/* ************************************************************************** */

/*!
 * \brief The DatabaseManager class
 */
class DatabaseManager: public QObject
{
    Q_OBJECT

    const static int s_dbCurrentVersion = 3;

    bool m_dbInternalAvailable = false;
    bool m_dbInternalOpen = false;
    bool m_dbExternalAvailable = false;
    bool m_dbExternalOpen = false;

    bool openDatabase_sqlite();
    bool openDatabase_mysql();
    void closeDatabase();

    void createDatabase();
    void resetDatabase();
    void deleteDatabase();

    bool tableExists(const QString &tableName);
    void migrateDatabase();
    bool migrate_v1v2();
    bool migrate_v2v3();

    // Singleton
    static DatabaseManager *instance;
    DatabaseManager();
    ~DatabaseManager();

public:
    static DatabaseManager *getInstance();

    Q_INVOKABLE bool hasDatabaseInternal() const { return m_dbInternalOpen; }
    Q_INVOKABLE bool hasDatabaseExternal() const { return m_dbExternalOpen; }
};

/* ************************************************************************** */
#endif // DATABASE_MANAGER_H
