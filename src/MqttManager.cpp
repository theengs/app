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

#include "MqttManager.h"
#include "SettingsManager.h"

#if defined(ENABLE_MQTT)
#include <QtMqtt/QtMqtt>
#endif

#ifndef QT_NO_SSL
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslSocket>
#endif

#include <QFile>
#include <QTime>

/* ************************************************************************** */

MqttManager *MqttManager::instance = nullptr;

MqttManager *MqttManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new MqttManager();
    }

    return instance;
}

MqttManager::MqttManager()
{
    // broker presets:

    Broker *p1 = new Broker(2, "HomeAssistant", "homeassistant.local", 1883, this); m_brokersAvailable.push_back(p1);
    //Broker *p1ssl = new Broker(2, "HomeAssistant", "homeassistant.local", 8883, this); m_brokersAvailable.push_back(p1ssl);

    Broker *p2 = new Broker(2, "OpenHAB", "openhab.local", 1883, this); m_brokersAvailable.push_back(p2);
    //Broker *p2ssl = new Broker(2, "OpenHAB", "openhab.local", 8883, this); m_brokersAvailable.push_back(p2ssl);

    Broker *p3 = new Broker(2, "NodeRED", "nodered.local", 1883, this); m_brokersAvailable.push_back(p3);
    //Broker *p3ssl = new Broker(2, "NodeRED", "nodered.local", 8883, this); m_brokersAvailable.push_back(p3ssl);

    Broker *p4 = new Broker(2, "Jeedom", "jeedom.local", 1883, this); m_brokersAvailable.push_back(p4);
    //Broker *p4ssl = new Broker(2, "Jeedom", "jeedom.local", 8883, this); m_brokersAvailable.push_back(p4ssl);
}

MqttManager::~MqttManager()
{
    qDeleteAll(m_brokersAvailable);
    m_brokersAvailable.clear();
}

/* ************************************************************************** */
/* ************************************************************************** */

bool MqttManager::connect()
{
#if defined(ENABLE_MQTT)

    if (!m_mqttclient)
    {
        m_mqttclient = new QMqttClient(this);
        QObject::connect(m_mqttclient, &QMqttClient::stateChanged, this, &MqttManager::updateStateChange);
        QObject::connect(m_mqttclient, &QMqttClient::connected, this, &MqttManager::brokerConnected);
        QObject::connect(m_mqttclient, &QMqttClient::disconnected, this, &MqttManager::brokerDisconnected);
        QObject::connect(m_mqttclient, &QMqttClient::errorChanged, this,
                         [this](QMqttClient::ClientError e) {
                             logLine(QString("error: %1").arg(int(e)));
                         });
    }

    if (m_mqttclient)
    {
        //qDebug() << "MqttManager::connect()";

        SettingsManager *sm = SettingsManager::getInstance();
        m_mqttclient->setHostname(sm->getMqttHost());
        m_mqttclient->setPort(sm->getMqttPort());
        m_mqttclient->setUsername(sm->getMqttUser());
        m_mqttclient->setPassword(sm->getMqttPassword());

#ifndef QT_NO_SSL
        if (sm->getMqttTls())
        {
            QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
            const QString caPath = sm->getMqttTlsCaPath();
            if (!caPath.isEmpty())
            {
                if (caPath.startsWith(":/") || QFile::exists(caPath))
                {
                    QList<QSslCertificate> caList = sslConf.caCertificates();
                    caList.append(QSslCertificate::fromPath(caPath, QSsl::Pem));
                    sslConf.setCaCertificates(caList);
                }
                else
                {
                    qWarning() << "MQTT TLS CA path not found, falling back to system store:" << caPath;
                }
            }
            if (sm->getMqttTlsInsecure())
            {
                sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
            }
            m_mqttclient->connectToHostEncrypted(sslConf);
        }
        else
        {
            m_mqttclient->connectToHost();
        }
#else
        m_mqttclient->connectToHost();
#endif
    }
#endif

    return false;
}

void MqttManager::disconnect()
{
#if defined(ENABLE_MQTT)

    if (m_mqttclient)
    {
        //qDebug() << "MqttManager::disconnect()";

        m_mqttclient->disconnectFromHost();
    }

#endif
}

void MqttManager::reconnect_forced()
{
#if defined(ENABLE_MQTT)

    SettingsManager *sm = SettingsManager::getInstance();
    if (sm && sm->getMQTT())
    {
        disconnect();
        connect();
    }

#endif
}

void MqttManager::reconnect()
{
#if defined(ENABLE_MQTT)

    SettingsManager *sm = SettingsManager::getInstance();
    if (sm && sm->getMQTT())
    {
        if (!m_mqttclient || m_mqttclient->state() != QMqttClient::Connected)
        {
            connect();
        }
    }
    else
    {
        disconnect();
    }

#endif
}

/* ************************************************************************** */
/* ************************************************************************** */

bool MqttManager::publishConfig(QString topic, QString str)
{
#if defined(ENABLE_MQTT)
    if (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected)
    {
        if (!topic.isEmpty() && !str.isEmpty())
        {
            //qDebug() << "MqttManager::publishConfig(" << topic << " : " << str << ")";

            QMqttTopicName t(topic);
            QByteArray m(str.toUtf8());

            //QString l = "config: " + topic + " / " + str + "\n";
            //m_mqttLog.push_front(l);
            //Q_EMIT logChanged();

            m_mqttclient->publish(t, m, 0, true);
            return true;
        }
    }
#endif

    return false;
}

/* ************************************************************************** */

bool MqttManager::publishData(QString topic, QString str)
{
#if defined(ENABLE_MQTT)
    if (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected)
    {
        if (topic.isEmpty())
        {
            SettingsManager *sm = SettingsManager::getInstance();
            if (!sm || (sm && (sm->getMqttTopicA().isEmpty() || sm->getMqttTopicB().isEmpty()))) return false;

            topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT";
        }

        //qDebug() << "MqttManager::publishData(" << topic << " : " << str << ")";

        QMqttTopicName t(topic);
        QByteArray m(str.toUtf8());

        //QString l = "publish: " + topic + " / " + str + "\n";
        //m_mqttLog.push_front(l);
        //Q_EMIT logChanged();

        m_mqttclient->publish(t, m);
        return true;
    }

    // Track drops while disconnected so the UI can surface "MQTT down — N
    // readings lost since HH:MM". Only count when MQTT is *configured* on,
    // otherwise an opted-out user would see scary counters.
    SettingsManager *sm = SettingsManager::getInstance();
    if (sm && sm->getMQTT())
    {
        ++m_droppedSinceDisconnect;
        Q_EMIT droppedChanged();
    }
#else
    Q_UNUSED(topic) Q_UNUSED(str)
#endif

    return false;
}

/* ************************************************************************** */

bool MqttManager::subscribe(QString topic)
{
#if defined(ENABLE_MQTT)
    if (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected)
    {
        Q_UNUSED(topic)
    }
#endif

    return false;
}

/* ************************************************************************** */

bool MqttManager::getStatus() const
{
#if defined(ENABLE_MQTT)
    return (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected);
#endif

    return false;
}

/* ************************************************************************** */
/* ************************************************************************** */

void MqttManager::updateStateChange()
{
#if defined(ENABLE_MQTT)

    if (m_mqttclient)
    {
        //qDebug() << "MqttManager::updateStateChange()" << m_mqttclient->state();
        Q_EMIT statusChanged();

        if (m_mqttclient->state() == QMqttClient::Disconnected) logLine("status: disconnected");
        else if (m_mqttclient->state() == QMqttClient::Connecting) logLine("status: connecting");
        else if (m_mqttclient->state() == QMqttClient::Connected) logLine("status: connected");
    }

#endif
}

void MqttManager::brokerConnected()
{
#if defined(ENABLE_MQTT)

    //qDebug() << "MqttManager::brokerConnected()" << m_mqttclient->state();

    if (m_mqttclient)
    {
        // Clear drop bookkeeping: the banner/notification hides automatically
        // once droppedSinceDisconnect == 0 and disconnectedSince is invalid.
        if (m_droppedSinceDisconnect != 0)
        {
            m_droppedSinceDisconnect = 0;
            Q_EMIT droppedChanged();
        }
        if (m_disconnectedSince.isValid())
        {
            m_disconnectedSince = QDateTime();
            // statusChanged already emitted by updateStateChange().
        }

        Q_EMIT connected();

        SettingsManager *sm = SettingsManager::getInstance();
        if (!sm || (sm && (sm->getMqttTopicA().isEmpty() || sm->getMqttTopicB().isEmpty()))) return;

        QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/version";

        QMqttTopicName t(topic);
        QByteArray v("v" + QString::fromLatin1(APP_VERSION).toUtf8());

        m_mqttclient->publish(t, v);

        // current broker preset
        {
            Broker *mine = new Broker(1, "Current broker",
                                      sm->getMqttHost(), sm->getMqttPort(),
                                      sm->getMqttUser(), sm->getMqttPassword(),
                                      sm->getMqttTopicA(), sm->getMqttTopicB(),
                                      this);

            if (m_brokersAvailable.first()->getType() == 1)
            {
                Broker *previous = qobject_cast<Broker *>(m_brokersAvailable.first());
                m_brokersAvailable.removeFirst();
                delete previous;
            }

            m_brokersAvailable.push_front(mine);

            Q_EMIT brokersUpdated();
        }
    }

#endif
}

void MqttManager::brokerDisconnected()
{
    //qDebug() << "MqttManager::brokerDisconnected()";

    // Stamp the moment the link went down so UI can show "since HH:MM".
    // updateStateChange() will fire statusChanged separately.
    if (!m_disconnectedSince.isValid())
    {
        m_disconnectedSince = QDateTime::currentDateTime();
    }
}

void MqttManager::handleMessage(/*const QMqttMessage &qmsg*/)
{
    //qDebug() << "MqttManager::handleMessage()" << qmsg.topic();
}

void MqttManager::logLine(const QString &msg)
{
    m_mqttLog.prepend("[" + QTime::currentTime().toString("HH:mm:ss") + "] " + msg + "\n");

    // Cap at 100 most-recent lines (newest are at the start). Long-running
    // sessions otherwise grow the buffer unbounded.
    QStringList lines = m_mqttLog.split('\n', Qt::SkipEmptyParts);
    if (lines.size() > 100)
    {
        lines = lines.mid(0, 100);
        m_mqttLog = lines.join('\n') + '\n';
    }

    Q_EMIT logChanged();
}

/* ************************************************************************** */
