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
    if (m_mqttclient)
    {
        // ?
        publish("theengs", "rzttmkhkofghm");
    }

    if (!m_mqttclient)
    {
        qDebug() << "MqttManager::connect()";
        SettingsManager *sm = SettingsManager::getInstance();

        m_mqttclient = new QMqttClient(this);
        m_mqttclient->setHostname(sm->getMqttHost());
        m_mqttclient->setPort(sm->getMqttPort());

        m_mqttclient->setUsername(sm->getMqttUser());
        m_mqttclient->setPassword(sm->getMqttPassword());

        QObject::connect(m_mqttclient, &QMqttClient::stateChanged, this, &MqttManager::updateStateChange);
        QObject::connect(m_mqttclient, &QMqttClient::disconnected, this, &MqttManager::brokerDisconnected);

        m_mqttclient->connectToHost();
        //m_mqttclient->connectToHostEncrypted();

        qDebug() << "yes ?";
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

/* ************************************************************************** */

bool MqttManager::publish(const QString &topic, const QString &str)
{
    if (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected)
    {
        qDebug() << "MqttManager::publish(" << topic << " : " << str << ")";

        QMqttTopicName t(topic);
        QByteArray m(str.toLocal8Bit());

        QString l = "publish: " + topic + " / " + str + "\n";
        m_mqttLog += l;
        Q_EMIT logChanged();

        m_mqttclient->publish(t, m);

        return true;
    }

    return false;
}

bool MqttManager::subscribe(const QString &topic)
{
    if (m_mqttclient && m_mqttclient->state() == QMqttClient::Connected)
    {
        //
    }
}

/* ************************************************************************** */

void MqttManager::updateStateChange()
{
    qDebug() << "MqttManager::updateStateChange()" << m_mqttclient->state();

    if (m_mqttclient->state() == 0) m_mqttLog += "status: disconnected \n";
    if (m_mqttclient->state() == 1) m_mqttLog += "status: connecting \n";
    if (m_mqttclient->state() == 2) m_mqttLog += "status: connected \n";
    Q_EMIT logChanged();

    Q_EMIT statusChanged();
}

void MqttManager::brokerDisconnected()
{
    qDebug() << "MqttManager::brokerDisconnected()" << m_mqttclient->state();
}

void MqttManager::handleMessage(const QMqttMessage &qmsg)
{
    qDebug() << "MqttManager::handleMessage()" << qmsg.topic();
}

/* ************************************************************************** */
