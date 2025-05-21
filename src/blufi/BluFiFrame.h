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

#ifndef BLUFI_FRAME_H
#define BLUFI_FRAME_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QByteArray>

/* ************************************************************************** */

/*!
 * \brief The BluFiFrame class
 *
 * - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/blufi.html
 * - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/blufi.html#the-frame-formats-defined-in-blufi
 */
class BluFiFrame {
    /*!
     * Type field takes 1 byte and is divided into Type and Subtype.
     * - Type uses the lower two bits, indicating whether the frame is a data frame or a control frame.
     * - Subtype uses the upper six bits, indicating the specific meaning of this data frame or control frame.
     * The control frame is not encrypted for the time being and supports to be verified.
     * The data frame supports to be encrypted and verified.
     */
    uint8_t m_type;
        uint8_t m_type_type = 0;
        uint8_t m_type_subtype = 0;

    //! The Frame Control field takes one byte and each bit has a different meaning.
    uint8_t m_ctrl;
        bool m_ctrl_encrypted = false;
        bool m_ctrl_checksum = false;
        uint8_t m_ctrl_direction = 0;
        bool m_ctrl_requireAck = false;
        bool m_ctrl_frag = false;

    /*!
     * The Sequence Number field is the field for sequence control.
     * When a frame is sent, the value of this field is automatically incremented
     * by 1 regardless of the type of frame, which prevents Replay Attack.
     * The sequence would be cleared after each reconnection.
     */
    uint8_t m_seq_nb = 0;

    //! The Data Length field indicates the length of the data field, which does not include CheckSum.
    uint8_t m_data_size = 0;

    //! Content of the Data field can be different according to various values of Type or Subtype.
    QByteArray m_data;

    //! The CheckSum field takes two bytes, which is used to check "sequence + data length + clear text data".
    uint16_t m_checksum = 0;
    QByteArray m_checksum_data;

    bool m_isFrameValid = true;

public:
    BluFiFrame();
    BluFiFrame(const QByteArray &data);
    ~BluFiFrame();

    void getFrameTypeStr(QString &type_str, QString &subtype_str, QString &direction_str);
    void getFrameType(uint8_t &type, uint8_t &subtype);
    void setFrameType(const uint8_t type, const uint8_t subtype);

    void getFrameCtrl(bool &encrypted, bool &checksum, int &direction, bool &requireAck, bool &frag);
    void setFrameCtrl(const bool encrypted, const bool checksum, const int direction, const bool requireAck, const bool frag);

    int getSequenceNumber();
    void setSequenceNumber(const int seq_nb);

    QByteArray getData();
    void setData(const QByteArray &data);

    void generateChecksum();
    bool checkChecksum();

    QByteArray toByeArray();
    void fromByeArray(const QByteArray &data);

    /// DEBUG
    void printHeader();
    void printDetails();
    void printParsedData();
    QString getParsedData();
};

/* ************************************************************************** */
#endif // BLUFI_FRAME_H
