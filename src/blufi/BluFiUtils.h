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

#ifndef BLUFI_UTILS_H
#define BLUFI_UTILS_H
/* ************************************************************************** */

#include <QObject>
#include <QString>

/* ************************************************************************** */

class BluFiUtils: public QObject
{
    Q_OBJECT

public:
    static void registerQML()
    {
        // TODO
    }

    enum CommunicationDirection {
        DIRECTION_OUTPUT    = 0,    //!< mobile phone to the ESP device
        DIRECTION_INPUT     = 1,    //!< ESP device to the mobile phone
    };
    Q_ENUM(CommunicationDirection)

    enum FrameType {
        CTRL_FRAME = 0x0,
        DATA_FRAME = 0x1
    };
    Q_ENUM(FrameType)

    enum FrameSubType {
        // 1.1 Control Frame
        SUBTYPE_ACK = 0x00,
        SUBTYPE_SET_SEC_MODE = 0x01,
        SUBTYPE_SET_OP_MODE = 0x02,
        SUBTYPE_CONNECT_WIFI = 0x03,
        SUBTYPE_DISCONNECT_WIFI = 0x04,
        SUBTYPE_GET_WIFI_STATUS = 0x05,
        SUBTYPE_DEAUTHENTICATE = 0x06,
        SUBTYPE_GET_VERSION = 0x07,
        SUBTYPE_CLOSE_CONNECTION = 0x08,
        SUBTYPE_GET_WIFI_LIST = 0x09,
        // 1.2 Data Frame
        SUBTYPE_NEG = 0x00,
        SUBTYPE_STA_WIFI_BSSID = 0x01,
        SUBTYPE_STA_WIFI_SSID = 0x02,
        SUBTYPE_STA_WIFI_PASSWORD = 0x03,
        SUBTYPE_SOFTAP_WIFI_SSID = 0x04,
        SUBTYPE_SOFTAP_WIFI_PASSWORD = 0x05,
        SUBTYPE_SOFTAP_MAX_CONNECTION_COUNT = 0x06,
        SUBTYPE_SOFTAP_AUTH_MODE = 0x07,
        SUBTYPE_SOFTAP_CHANNEL = 0x08,
        SUBTYPE_USERNAME = 0x09,
        SUBTYPE_CA_CERTIFICATION = 0x0a,
        SUBTYPE_CLIENT_CERTIFICATION = 0x0b,
        SUBTYPE_SERVER_CERTIFICATION = 0x0c,
        SUBTYPE_CLIENT_PRIVATE_KEY = 0x0d,
        SUBTYPE_SERVER_PRIVATE_KEY = 0x0e,
        SUBTYPE_WIFI_CONNECTION_STATE = 0x0f,
        SUBTYPE_VERSION = 0x10,
        SUBTYPE_WIFI_LIST = 0x11,
        SUBTYPE_ERROR = 0x12,
        SUBTYPE_CUSTOM_DATA = 0x13,
        SUBTYPE_WIFI_STA_MAX_CONN_RETRY = 0x14,
        SUBTYPE_WIFI_STA_CONN_END_REASON = 0x15,
        SUBTYPE_WIFI_STA_CONN_RSSI = 0x16
    };
    Q_ENUM(FrameSubType)

    enum FrameCtrlFieldsPos {
        POSITION_ENCRYPTED      = (1 << 0),
        POSITION_CHECKSUM       = (1 << 1),
        POSITION_DATA_DIRECTION = (1 << 2),
        POSITION_REQUIRE_ACK    = (1 << 3),
        POSITION_FRAG           = (1 << 4)
    };
    Q_ENUM(FrameCtrlFieldsPos)

    ///

    enum SeqMode {
        SEQ_MODE_NULL = 0x00,
        SEQ_MODE_CHECK = 0x01,
        SEQ_MODE_ENC = 0x02,
        SEQ_MODE_FULL = 0x03
    };
    Q_ENUM(SeqMode)

    enum BluFiErrors {
        ERR_sequence = 0x00,
        ERR_checksum = 0x01,
        ERR_decrypt = 0x02,
        ERR_encrypt = 0x03,
        ERR_init_security = 0x04,
        ERR_dh_malloc = 0x05,
        ERR_dh_param = 0x06,
        ERR_read_param = 0x07,
        ERR_make_public = 0x08,
        ERR_data_format = 0x09,
        ERR_calculate_MD5 = 0x0a,
        ERR_wifi_scan = 0x0b
    };
    Q_ENUM(BluFiErrors)

    ///

    enum WifiOpMode {
        WIFI_OP_MODE_NULL = 0x00,
        WIFI_OP_MODE_STA = 0x01,
        WIFI_OP_MODE_SOFTAP = 0x02,
        WIFI_OP_MODE_STASOFTAP = 0x03,
    };
    Q_ENUM(WifiOpMode)

    enum WifiOpState {
        WIFI_OP_STATE_CONNECTED = 0x00,
        WIFI_OP_STATE_DISCONNECTED = 0x01,
        WIFI_OP_STATE_CONNECTING = 0x02,
        WIFI_OP_STATE_CONNECTED_NO_IP = 0x03,
    };
    Q_ENUM(WifiOpState)

    enum WifiSeqMode {
        WIFI_SEQ_MODE_OPEN = 0x00,
        WIFI_SEQ_MODE_WEP = 0x01,
        WIFI_SEQ_MODE_WPA = 0x02,
        WIFI_SEQ_MODE_WPA2 = 0x03,
        WIFI_SEQ_MODE_WPA_WPA2 = 0x04,
    };
    Q_ENUM(WifiSeqMode)

    enum WifiConnStatus {
        WIFI_CONN_SUCCESS = 0x00,
        WIFI_CONN_FAILURE = 0x01,
        WIFI_CONN_CONNECTING = 0x02,
        WIFI_CONN_NO_IP = 0x03,
    };
    Q_ENUM(WifiConnStatus)

    enum WifiConnError {
        WIFI_ERROR_4WAY_HANDSHAKE_TIMEOUT = 15,
        WIFI_ERROR_NO_AP_FOUND = 201,
        WIFI_ERROR_HANDSHAKE_TIMEOUT = 204,
        WIFI_ERROR_CONNECTION_FAIL = 205,
    };
    Q_ENUM(WifiConnError)

    ///
};

/* ************************************************************************** */
#endif // BLUFI_UTILS_H
