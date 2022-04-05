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

#include <QObject>
#include <QtMqtt/QtMqtt>

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
    //
}

MqttManager::~MqttManager()
{
    //
}

/* ************************************************************************** */
/* ************************************************************************** */

bool MqttManager::connect()
{
    if (!m_mqttclient)
    {
        m_mqttclient = new QMqttClient(this);
        QObject::connect(m_mqttclient, &QMqttClient::stateChanged, this, &MqttManager::updateStateChange);
        QObject::connect(m_mqttclient, &QMqttClient::connected, this, &MqttManager::brokerConnected);
        QObject::connect(m_mqttclient, &QMqttClient::disconnected, this, &MqttManager::brokerDisconnected);
    }

    if (m_mqttclient)
    {
        qDebug() << "MqttManager::connect()";

        SettingsManager *sm = SettingsManager::getInstance();
        m_mqttclient->setHostname(sm->getMqttHost());
        m_mqttclient->setPort(sm->getMqttPort());
        m_mqttclient->setUsername(sm->getMqttUser());
        m_mqttclient->setPassword(sm->getMqttPassword());

        m_mqttclient->connectToHost();
        //m_mqttclient->connectToHostEncrypted();
    }

    return false;
}

void MqttManager::disconnect()
{
    if (m_mqttclient)
    {
        qDebug() << "MqttManager::disconnect()";

        m_mqttclient->disconnectFromHost();
    }
}

void MqttManager::reconnect()
{
    SettingsManager *sm = SettingsManager::getInstance();
    if (sm && sm->getMQTT())
    {
        disconnect();
        connect();
    }
}

void MqttManager::reconnect2()
{
    SettingsManager *sm = SettingsManager::getInstance();
    if (sm && sm->getMQTT())
    {
        if (!m_mqttclient || m_mqttclient->state() != QMqttClient::Connected)
            connect();
    }
    else
    {
        disconnect();
    }
}

/* ************************************************************************** */

bool MqttManager::publish(QString topic, QString str)
{
    if (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected)
    {
        if (topic.isEmpty())
        {
            SettingsManager *sm = SettingsManager::getInstance();
            if (!sm || (sm && (sm->getMqttTopicA().isEmpty() || sm->getMqttTopicB().isEmpty()))) return false;

            topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/BTtoMQTT";
        }

        qDebug() << "MqttManager::publish(" << topic << " : " << str << ")";

        QMqttTopicName t(topic);
        QByteArray m(str.toLocal8Bit());

        QString l = "publish: " + topic + " / " + str + "\n";
        m_mqttLog.push_front(l);
        Q_EMIT logChanged();

        m_mqttclient->publish(t, m);

        return true;
    }

    return false;
}

bool MqttManager::subscribe(QString topic)
{
    if (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected)
    {
        Q_UNUSED(topic)
    }

    return false;
}

/* ************************************************************************** */

void MqttManager::updateStateChange()
{
    if (m_mqttclient)
    {
        qDebug() << "MqttManager::updateStateChange()" << m_mqttclient->state();

        if (m_mqttclient->state() == 0) m_mqttLog.push_front("status: disconnected \n");
        if (m_mqttclient->state() == 1) m_mqttLog.push_front("status: connecting \n");
        if (m_mqttclient->state() == 2) m_mqttLog.push_front("status: connected \n");
        Q_EMIT logChanged();

        Q_EMIT statusChanged();
    }
}

void MqttManager::brokerConnected()
{
    qDebug() << "MqttManager::brokerConnected()" << m_mqttclient->state();

    if (m_mqttclient)
    {
        SettingsManager *sm = SettingsManager::getInstance();
        if (!sm || (sm && (sm->getMqttTopicA().isEmpty() || sm->getMqttTopicB().isEmpty()))) return;

        QString topic = sm->getMqttTopicA() + "/" + sm->getMqttTopicB() + "/version";

        QMqttTopicName t(topic);
        QByteArray v("v" + QString::fromLatin1(APP_VERSION).toLocal8Bit());

        m_mqttclient->publish(t, v);
    }
}

void MqttManager::brokerDisconnected()
{
    qDebug() << "MqttManager::brokerDisconnected()";
}

void MqttManager::handleMessage(const QMqttMessage &qmsg)
{
    qDebug() << "MqttManager::handleMessage()" << qmsg.topic();
}

/* ************************************************************************** */
