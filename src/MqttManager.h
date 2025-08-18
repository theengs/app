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

#include <QtMqtt/QtMqtt>

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

    Q_PROPERTY(QString log READ getLog NOTIFY logChanged) // DEBUG

    Q_PROPERTY(QVariant brokersAvailable READ getBrokersAvailable NOTIFY brokersUpdated)

    QMqttClient *m_mqttclient = nullptr;

    QString m_mqttLog; // DEBUG

    QList <Broker *> m_brokersAvailable;
    QVariant getBrokersAvailable() const { return QVariant::fromValue(m_brokersAvailable); }

    static MqttManager *instance;

    MqttManager();
    ~MqttManager();

Q_SIGNALS:
    void brokersUpdated();
    void statusChanged();
    void logChanged();
    void connected();

private slots:
    void handleMessage(const QMqttMessage &qmsg);
    void updateStateChange();
    void brokerConnected();
    void brokerDisconnected();

public:
    static MqttManager *getInstance();

    Q_INVOKABLE bool connect();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void reconnect_forced();
    Q_INVOKABLE void reconnect();

    bool publishConfig(QString topic, QString str);
    bool publishData(QString topic, QString str);
    bool subscribe(QString topic);

    bool getStatus() const { return (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected); }
    QString getLog() const { return m_mqttLog; }
};

/* ************************************************************************** */
#endif // MQTT_MANAGER_H
