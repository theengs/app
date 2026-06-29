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

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QVariant>
#include <QList>
#include <QDateTime>

#if defined(ENABLE_MQTT)
#include <QtMqtt/QtMqtt>
#endif

class QTimer;

/* ************************************************************************** */

class Broker: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int type READ getType CONSTANT)

    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString host READ getHost CONSTANT)
    Q_PROPERTY(int port READ getPort CONSTANT)
    Q_PROPERTY(QString username READ getUsername CONSTANT)
    Q_PROPERTY(QString password READ getPassword CONSTANT)
    Q_PROPERTY(QString topicA READ getTopicA CONSTANT)
    Q_PROPERTY(QString topicB READ getTopicB CONSTANT)

    int m_type = 0; // 1: my broker // 2: preset // 3: discovery
    QString m_name;
    QString m_host;
    int m_port = 0;
    QString m_username;
    QString m_password;
    QString m_topicA = "home";
    QString m_topicB = "";

    QString getName() const { return m_name; }
    QString getHost() const { return m_host; }
    int getPort() const { return m_port; }
    QString getUsername() const { return m_username; }
    QString getPassword() const { return m_password; }
    QString getTopicA() const { return m_topicA; }
    QString getTopicB() const { return m_topicB; }

public:
    Broker(int type, QString name,
           QString host, int port,
           QObject *parent) : QObject(parent) {
        m_type = type; m_name = name;
        m_host = host; m_port = port;
    }
    Broker(int type, QString name,
           QString host, int port,
           QString user, QString pwd,
           QString topicA, QString topicB,
           QObject *parent) : QObject(parent) {
        m_type = type; m_name = name;
        m_host = host; m_port = port;
        m_username = user; m_password = pwd;
        m_topicA = topicA; m_topicB = topicB;
    }
    ~Broker() = default;

    int getType() const { return m_type; }
};

/* ************************************************************************** */

/*!
 * \brief The MqttManager class
 */
class MqttManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool status READ getStatus NOTIFY statusChanged)

    // Count of publishData() calls dropped because the client wasn't Connected.
    // Resets to 0 when the broker comes back. Surfaced in the device list and
    // foreground notification so users know data is being lost.
    Q_PROPERTY(qint64 droppedSinceDisconnect READ getDroppedSinceDisconnect NOTIFY droppedChanged)
    Q_PROPERTY(QDateTime disconnectedSince READ getDisconnectedSince NOTIFY statusChanged)

    Q_PROPERTY(QString log READ getLog NOTIFY logChanged) // DEBUG

    Q_PROPERTY(QVariant brokersAvailable READ getBrokersAvailable NOTIFY brokersUpdated)

#if defined(ENABLE_MQTT)
    QMqttClient *m_mqttclient = nullptr;
#endif

    QString m_mqttLog; // DEBUG

    // Dropped-message bookkeeping. Updated from publishData() / state handlers.
    qint64 m_droppedSinceDisconnect = 0;
    QDateTime m_disconnectedSince;

    // Auto-reconnect with exponential backoff. QtMqtt does NOT reconnect on
    // its own, so an unexpected drop (e.g. broker restart) would otherwise
    // strand the app on the disconnect banner until the user taps retry.
    // m_reconnectWanted tracks intent: true while we want to stay connected,
    // false after an explicit disconnect()/MQTT-off, so the timer never
    // fights the user.
    static constexpr int kReconnectBaseMs = 2000;
    static constexpr int kReconnectMaxMs = 60000;
    QTimer *m_reconnectTimer = nullptr;
    int m_reconnectInterval = kReconnectBaseMs;
    bool m_reconnectWanted = false;

    QList <Broker *> m_brokersAvailable;
    QVariant getBrokersAvailable() const { return QVariant::fromValue(m_brokersAvailable); }

    static MqttManager *instance;

    MqttManager();
    ~MqttManager();

Q_SIGNALS:
    void brokersUpdated();
    void statusChanged();
    void droppedChanged();
    void logChanged();
    void connected();

private slots:
    void handleMessage(/*const QMqttMessage &qmsg*/);
    void updateStateChange();
    void brokerConnected();
    void brokerDisconnected();
    void reconnectTimerFired();

private:
    // Prepend a timestamped line to m_mqttLog (newest on top) and emit
    // logChanged so the QML broker panel re-renders. Called from the
    // state-change / error / TLS handlers.
    void logLine(const QString &msg);

    // Arm the single-shot backoff timer for the next reconnect attempt.
    // No-op unless m_reconnectWanted (and MQTT is still enabled in settings).
    void scheduleReconnect();

public:
    static MqttManager *getInstance();

    Q_INVOKABLE bool connect();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void reconnect_forced();
    Q_INVOKABLE void reconnect();

    bool publishConfig(QString topic, QString str);
    bool publishData(QString topic, QString str);
    bool subscribe(QString topic);

    bool getStatus() const;
    qint64 getDroppedSinceDisconnect() const { return m_droppedSinceDisconnect; }
    QDateTime getDisconnectedSince() const { return m_disconnectedSince; }

    QString getLog() const { return m_mqttLog; }
};

/* ************************************************************************** */
#endif // MQTT_MANAGER_H
