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

#include "BluFiFrame.h"
#include "BluFiUtils.h"

#include <QDebug>

/* ************************************************************************** */

BluFiFrame::BluFiFrame()
{
    //
}

BluFiFrame::BluFiFrame(const QByteArray &data)
{
    fromByeArray(data);
}

BluFiFrame::~BluFiFrame()
{
    //
}

/* ************************************************************************** */

void BluFiFrame::getFrameType(uint8_t &type, uint8_t &subtype)
{
    type = (m_type & 0x03);
    subtype = ((m_type & 0xFC) >> 2);
}

void BluFiFrame::setFrameType(const uint8_t type, const uint8_t subtype)
{
    if ( (type == BluFiUtils::CTRL_FRAME && (subtype < 0 || subtype > BluFiUtils::SUBTYPE_GET_WIFI_LIST)) ||
        (type == BluFiUtils::DATA_FRAME && (subtype < 0 || subtype > BluFiUtils::SUBTYPE_WIFI_STA_CONN_RSSI)) )
    {
        qWarning() << "BluFiFrame::setFrameType(" << type << subtype << ") INVALID";
        m_isFrameValid = false;
    }

    m_type_type = type;
    m_type_subtype = subtype;

    m_type = (type & 0x03) + ((subtype << 2) & 0xFC);
}

void BluFiFrame::getFrameCtrl(bool &encrypted, bool &checksum,
                              int &direction, bool &requireAck, bool &frag)
{
    encrypted = (m_ctrl & 0x01);
    checksum = (m_ctrl & 0x02);
    direction = ((m_ctrl & 0x04) >> 2);
    requireAck = (m_ctrl & 0x08);
    frag = (m_ctrl & 0x10);
}

void BluFiFrame::setFrameCtrl(const bool encrypted, const bool checksum,
                              const int direction, const bool requireAck,
                              const bool frag)
{
    if (direction < 0 || direction > 0)
    {
        qWarning() << "BluFiFrame::setFrameCtrl() INVALID";
        m_isFrameValid = false;
    }

    m_ctrl_encrypted = encrypted;
    m_ctrl_checksum = checksum;
    m_ctrl_direction = direction;
    m_ctrl_requireAck = requireAck;
    m_ctrl_frag = frag;

    m_ctrl = 0; // reset
    if (encrypted) m_ctrl += 0x01;
    if (checksum) m_ctrl += 0x02;
    if (direction == BluFiUtils::DIRECTION_INPUT) m_ctrl += 0x04;
    if (requireAck) m_ctrl += 0x08;
    if (frag) m_ctrl += 0x10;
}

int BluFiFrame::getSequenceNumber()
{
    return m_seq_nb;
}

void BluFiFrame::setSequenceNumber(const int seq_nb)
{
    m_seq_nb = static_cast<uint8_t>(seq_nb);
}

QByteArray BluFiFrame::getData()
{
    return m_data;
}

void BluFiFrame::setData(const QByteArray &data)
{
    m_data = data;
    m_data_size = static_cast<uint8_t>(data.size());
}

void BluFiFrame::generateChecksum()
{
    QByteArray datatocheck;
    datatocheck.push_back(m_seq_nb);
    datatocheck.push_back(m_data_size);
    if (m_data_size) datatocheck.push_back(m_data);

    m_checksum = qChecksum(datatocheck, Qt::ChecksumIso3309);
    uint8_t c1 = ((m_checksum & 0xFF00) >> 8);
    uint8_t c2 = ((m_checksum & 0x00FF) >> 0);

    m_checksum_data.clear();
    m_checksum_data.push_back(c1);
    m_checksum_data.push_back(c2);
}

bool BluFiFrame::checkChecksum()
{
    QByteArray datatocheck;
    datatocheck.push_back(m_seq_nb);
    datatocheck.push_back(m_data_size);
    datatocheck.push_back(m_data);

    return (m_checksum == qChecksum(datatocheck, Qt::ChecksumIso3309));
}

/* ************************************************************************** */

QByteArray BluFiFrame::toByeArray()
{
    QByteArray data;

    data.push_back(m_type);
    data.push_back(m_ctrl);
    data.push_back(m_seq_nb);

    m_data_size = static_cast<uint8_t>(m_data.size());
    data.push_back(m_data_size);
    data.push_back(m_data);

    if (m_ctrl_checksum) // checksum flag
    {
        generateChecksum();
        data.push_back(m_checksum_data);
    }

    return data;
}

void BluFiFrame::fromByeArray(const QByteArray &data)
{
    if (data.size() < 4)
    {
        qWarning() << "BluFiFrame::fromByteArray() invalid size" << data.size();
        m_isFrameValid = false;
        return;
    }

    m_type = data.at(0);
    m_type_type = (m_type & 0x03);
    m_type_subtype = ((m_type & 0xFC) >> 2);

    m_ctrl = data.at(1);
    m_ctrl_encrypted = (m_ctrl & 0x01);
    m_ctrl_checksum = (m_ctrl & 0x02);
    m_ctrl_direction = ((m_ctrl & 0x04) >> 2);
    m_ctrl_requireAck = (m_ctrl & 0x08);
    m_ctrl_frag = (m_ctrl & 0x10);

    m_seq_nb = data.at(2);
    m_data_size = data.at(3);

    if ((m_data_size > 0) && (m_data_size >= data.size() - 4))
    {
        m_data = data.sliced(4, m_data_size);
    }
    else
    {
        qWarning() << "!!!! data size error : " << m_data_size << data.size() -4;
    }

    if (m_ctrl_checksum) // checksum flag
    {
        m_checksum = data.last(2).toUShort();
        m_checksum_data = data.last(2);
    }
}

/* ************************************************************************** */

void BluFiFrame::getFrameTypeStr(QString &type_str, QString &subtype_str, QString &direction_str)
{
    if (m_type_type == BluFiUtils::CTRL_FRAME)
    {
        type_str = "Control Frame";

        if (m_type_subtype == BluFiUtils::SUBTYPE_ACK) subtype_str = "ACK";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SET_SEC_MODE) subtype_str = "SET_SEC_MODE";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SET_OP_MODE) subtype_str = "SET_OP_MODE";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_CONNECT_WIFI) subtype_str = "CONNECT_WIFI";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_DISCONNECT_WIFI) subtype_str = "DISCONNECT_WIFI";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_GET_WIFI_STATUS) subtype_str = "GET_WIFI_STATUS";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_DEAUTHENTICATE) subtype_str = "DEAUTHENTICATE";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_GET_VERSION) subtype_str = "GET_VERSION";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_CLOSE_CONNECTION) subtype_str = "CLOSE_CONNECTION";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_GET_WIFI_LIST) subtype_str = "GET_WIFI_LIST";
    }
    else if (m_type_type == BluFiUtils::DATA_FRAME)
    {
        type_str = "Data Frame";

        if (m_type_subtype == BluFiUtils::SUBTYPE_NEG) subtype_str = "NEG";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_STA_WIFI_BSSID) subtype_str = "STA_WIFI_BSSID";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_STA_WIFI_SSID) subtype_str = "STA_WIFI_SSID";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_STA_WIFI_PASSWORD) subtype_str = "STA_WIFI_PASSWORD";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SOFTAP_WIFI_SSID) subtype_str = "SOFTAP_WIFI_SSID";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SOFTAP_WIFI_PASSWORD) subtype_str = "SOFTAP_WIFI_PASSWORD";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SOFTAP_MAX_CONNECTION_COUNT) subtype_str = "SOFTAP_MAX_CONNECTION_COUNT";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SOFTAP_AUTH_MODE) subtype_str = "SOFTAP_AUTH_MODE";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SOFTAP_CHANNEL) subtype_str = "SOFTAP_CHANNEL";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_USERNAME) subtype_str = "USERNAME";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_CA_CERTIFICATION) subtype_str = "CA_CERTIFICATION";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_CLIENT_CERTIFICATION) subtype_str = "CLIENT_CERTIFICATION";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SERVER_CERTIFICATION) subtype_str = "SERVER_CERTIFICATION";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_CLIENT_PRIVATE_KEY) subtype_str = "CLIENT_PRIVATE_KEY";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_SERVER_PRIVATE_KEY) subtype_str = "SERVER_PRIVATE_KEY";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_WIFI_CONNECTION_STATE) subtype_str = "WIFI_CONNECTION_STATE";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_VERSION) subtype_str = "VERSION";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_WIFI_LIST) subtype_str = "WIFI_LIST";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_ERROR) subtype_str = "ERROR";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_CUSTOM_DATA) subtype_str = "CUSTOM_DATA";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_WIFI_STA_MAX_CONN_RETRY) subtype_str = "WIFI_STA_MAX_CONN_RETRY";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_WIFI_STA_CONN_END_REASON) subtype_str = "WIFI_STA_CONN_END_REASON";
        else if (m_type_subtype == BluFiUtils::SUBTYPE_WIFI_STA_CONN_RSSI) subtype_str = "WIFI_STA_CONN_RSSI";
    }

    if (m_ctrl_direction == BluFiUtils::DIRECTION_OUTPUT) direction_str = "output (mobile phone to the ESP device)";
    else if (m_ctrl_direction == BluFiUtils::DIRECTION_INPUT) direction_str = "input (ESP device to the mobile phone)";
}

void BluFiFrame::printHeader()
{
    QString frametype_str;
    QString framesubtype_str;
    QString direction_str;
    getFrameTypeStr(frametype_str, framesubtype_str, direction_str);

    qDebug() << (m_ctrl_direction ? "<<<<<<" : ">>>>>>") << "BluFiFrame::printHeader()"
             << toByeArray().toHex() << " / " << toByeArray().size() << "bytes";
    qDebug() << "      " << frametype_str << " > " << framesubtype_str;
}

void BluFiFrame::printDetails()
{
    qDebug() << (m_ctrl_direction ? "<<<<<<" : ">>>>>>") << "BluFiFrame::printDetails()"
             << toByeArray().toHex() << " / " << toByeArray().size() << "bytes";

    QString frametype_str;
    QString framesubtype_str;
    QString direction_str;
    getFrameTypeStr(frametype_str, framesubtype_str, direction_str);

    qDebug() << "- type: " << m_type;
    qDebug() << "  - frame type: " << m_type_type << " > " << frametype_str;
    qDebug() << "  - frame sub type: " << m_type_type << " > " << framesubtype_str;
    qDebug() << "- ctrl: " << m_ctrl;
    qDebug() << "  - encrypted: " << m_ctrl_encrypted;
    qDebug() << "  - checksum: " << m_ctrl_checksum;
    qDebug() << "  - direction: " << m_ctrl_direction << " > " << direction_str;
    qDebug() << "  - requireAck: " << m_ctrl_requireAck;
    qDebug() << "  - frag: " << m_ctrl_frag;
    qDebug() << "- seq : " << m_seq_nb;
    qDebug() << "- data: " << m_data.toHex() << " / " << m_data.size() << "bytes";
    if (m_ctrl_checksum) // checksum flag
    {
        qDebug() << "- checksum: " << m_checksum << " > " << checkChecksum();
    }
}

void BluFiFrame::printParsedData()
{
    uint8_t frametype = 0;
    uint8_t framesubtype = 0;
    getFrameType(frametype, framesubtype);

    if (frametype == BluFiUtils::DATA_FRAME && framesubtype == BluFiUtils::SUBTYPE_VERSION)
    {
        if (m_data.size() == 2)
        {
            qDebug() << "> VERSION >" << QString::number(m_data.at(0)) << "." << QString::number(m_data.at(1));
        }
    }

    if (frametype == BluFiUtils::DATA_FRAME && framesubtype == BluFiUtils::SUBTYPE_WIFI_LIST)
    {
        qDebug() << "> WIFI_LIST >" << m_data.size() << m_data;

        int pos = 0;
        while (pos < m_data.size())
        {
            int length = m_data.at(pos);
            pos++;

            int rssi = m_data.at(pos);
            pos++;

            QString ssid = m_data.mid(pos, length-1);
            pos += length-1;

            qDebug() << " > WIFI (sz" << length << ") SSID" << ssid << "/ RSSI" << rssi;
        }
    }

    if (frametype == BluFiUtils::DATA_FRAME && framesubtype == BluFiUtils::SUBTYPE_WIFI_CONNECTION_STATE)
    {
        qDebug() << "> WIFI_CONNECTION_STATE >";

        if (m_data.size() >= 2)
        {
            //
            if (m_data.at(0) == BluFiUtils::WIFI_OP_MODE_NULL) qDebug() << "wifi mode: NULL";
            if (m_data.at(0) == BluFiUtils::WIFI_OP_MODE_STA) qDebug() << "wifi mode: STA";
            if (m_data.at(0) == BluFiUtils::WIFI_OP_MODE_SOFTAP) qDebug() << "wifi mode: SoftAP";
            if (m_data.at(0) == BluFiUtils::WIFI_OP_MODE_STASOFTAP) qDebug() << "wifi mode: SoftAP & STA";

            // connection state of the STA device
            if (m_data.at(1) == BluFiUtils::WIFI_OP_STATE_DISCONNECTED) qDebug() << "wifi state: disconnected";
            if (m_data.at(1) == BluFiUtils::WIFI_OP_STATE_CONNECTING) qDebug() << "wifi state: connecting";
            if (m_data.at(1) == BluFiUtils::WIFI_OP_STATE_CONNECTED) qDebug() << "wifi state: connected (with IP address)";
            if (m_data.at(1) == BluFiUtils::WIFI_OP_STATE_CONNECTED_NO_IP) qDebug() << "wifi state: connected (no IP address)";
        }
        if (m_data.size() >= 3)
        {
            // connection state of SoftAP
            qDebug() << "SoftAP connection count:" << m_data.at(2);
        }
        if (m_data.size() >= 4)
        {
            //data[3] and the subsequent is in accordance with the format of SSID/BSSID information.
            //If device is in connecting state, maximum Wi-Fi reconnecting time would be included here.
            //If device is in disconnected state, Wi-Fi connection end reason and RSSI would be included here.
        }
    }

    if (frametype == BluFiUtils::DATA_FRAME && framesubtype == BluFiUtils::SUBTYPE_ERROR)
    {
        if (m_data.size() >= 1)
        {
            if (m_data.at(0) == BluFiUtils::ERR_sequence) qDebug() << "BluFi error: sequence";
            if (m_data.at(0) == BluFiUtils::ERR_checksum) qDebug() << "BluFi error: checksum";
            if (m_data.at(0) == BluFiUtils::ERR_decrypt) qDebug() << "BluFi error: decrypt";
            if (m_data.at(0) == BluFiUtils::ERR_encrypt) qDebug() << "BluFi error: encrypt";
            if (m_data.at(0) == BluFiUtils::ERR_init_security) qDebug() << "BluFi error: init_security";
            if (m_data.at(0) == BluFiUtils::ERR_dh_malloc) qDebug() << "BluFi error: dh_malloc";
            if (m_data.at(0) == BluFiUtils::ERR_dh_param) qDebug() << "BluFi error: dh_param";
            if (m_data.at(0) == BluFiUtils::ERR_read_param) qDebug() << "BluFi error: read_param";
            if (m_data.at(0) == BluFiUtils::ERR_make_public) qDebug() << "BluFi error: make_public";
            if (m_data.at(0) == BluFiUtils::ERR_data_format) qDebug() << "BluFi error: data_format";
            if (m_data.at(0) == BluFiUtils::ERR_calculate_MD5) qDebug() << "BluFi error: calculate_MD5";
            if (m_data.at(0) == BluFiUtils::ERR_wifi_scan) qDebug() << "BluFi error: wifi_scan";
        }
    }
}

QString BluFiFrame::getParsedData()
{
    return QString();
}

/* ************************************************************************** */
