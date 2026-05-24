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

#ifndef SECRET_STORE_H
#define SECRET_STORE_H
/* ************************************************************************** */

#include <QString>

/*!
 * \brief Thin wrapper over QtKeychain for storing secrets in the OS secure store.
 *
 * Backends (provided by QtKeychain): Keychain on macOS/iOS, Credential Store on
 * Windows, libsecret/KWallet on Linux, Android Keystore on Android. QtKeychain
 * refuses to fall back to plaintext unless setInsecureFallback(true) is set, which
 * we deliberately never do.
 *
 * Reads are synchronous (spinning a local QEventLoop) because credentials must be
 * resolved before the first MQTT/MySQL connection attempt at startup, which happens
 * before the main Qt event loop is entered. Writes/removes are fire-and-forget.
 *
 * Only compiled when ENABLE_SECURE_STORAGE is defined.
 */
namespace SecretStore
{
    //! Service name namespacing all Theengs secrets in the OS store.
    QString service();

    //! Blocking read. Returns an empty string if the key is absent or on error.
    QString readSync(const QString &key);

    //! Asynchronous, best-effort write.
    void write(const QString &key, const QString &secret);

    //! Asynchronous, best-effort delete.
    void remove(const QString &key);
}

/* ************************************************************************** */
#endif // SECRET_STORE_H
