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

/*!
 * \brief The MqttManager class
 */
class MqttManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool status READ getStatus NOTIFY statusChanged)

    QMqttClient *m_mqttclient = nullptr;

    QString m_mqttLog; // DEBUG
    Q_PROPERTY(QString log READ getLog NOTIFY logChanged) // DEBUG

    static MqttManager *instance;

    MqttManager();
    ~MqttManager();

Q_SIGNALS:
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
