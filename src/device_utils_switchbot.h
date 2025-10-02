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

#ifndef DEVICE_UTILS_SWITCHBOT_H
#define DEVICE_UTILS_SWITCHBOT_H
/* ************************************************************************** */

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QQmlContext>
#include <QQmlApplicationEngine>

/* ************************************************************************** */

class DeviceUtilsSwitchBot: public QObject
{
    Q_OBJECT

public:
    static void registerQML()
    {
        qRegisterMetaType<DeviceUtilsSwitchBot::SwitchBotUserModes>("DeviceUtilsSwitchBot::SwitchBotUserModes");
        qRegisterMetaType<DeviceUtilsSwitchBot::SwitchBotUserActions>("DeviceUtilsSwitchBot::SwitchBotUserActions");

        qmlRegisterType<DeviceUtilsSwitchBot>("DeviceUtilsSwitchBot", 1, 0, "DeviceUtilsSwitchBot");
    }

    ///

    enum SwitchBotCommands {
        CMD_ACTION          = 0x01, //!< Execute an Action
        CMD_GET_INFO        = 0x02, //!< Get Device Basic Info
        CMD_SET_INFO        = 0x03, //!< Set Device Basic Info
        CMD_GET_TIMEMANAG   = 0x08, //!< Get Device Time Management Info
        CMD_SET_TIMEMANAG   = 0x09, //!< Set Device Time Management Info
        CMD_ACTION_EXTENDED = 0x0f, //!< Execute an ExtendedAction
    };

    enum SwitchBotResponses {
        RSP_OK              = 0x01, //!< OK, Action executed
        RSP_ERROR           = 0x02, //!< ERROR, Error while executing an Action
        RSP_BUSY            = 0x03, //!< BUSY, Device is busy now, please try later
        RSP_PROTOCOL        = 0x04, //!< Communication protocol version incompatible
        RSP_UNSUPPORTED     = 0x05, //!< Device does not support this Command
        RSP_LOWBATT         = 0x06, //!< Device's battery is low
        RSP_UNSUPPORTED2    = 0x0D, //!< This command is not supported in the current mode
        RSP_DISCONNECTED    = 0x0E, //!< Disconnected from the device that needs to stay connected
    };

    ///

    enum BotActions {
        BOT_ACT_PUSHPULL    = 0x00, //!< push and pull back
        BOT_ACT_ON          = 0x01, //!< light switch on
        BOT_ACT_OFF         = 0x02, //!< light switch off
        BOT_ACT_STOP        = 0x03, //!< push stop
        BOT_ACT_BACK        = 0x04, //!< back
    };

    enum BotModes {
        BOT_MODE_PRESS       = 0x00, //!<
        BOT_MODE_SWITCH      = 0x01, //!<
    };

    ///

    enum CurtainSpeeds {
        SPEED_HIGH          = 0x00, //!< High / standard speed
        SPEED_LOW           = 0x01, //!< Low / quiet / silent speed
    };
    Q_ENUM(CurtainSpeeds)

    enum CurtainActions {
        CURTAIN_ACT_STOP    = 0, //!< stop movement
        CURTAIN_ACT_MOVE    = 1, //!<
        CURTAIN_ACT_OPEN    = 2, //!< position 0
        CURTAIN_ACT_CLOSE   = 3, //!< position 100
    };
    Q_ENUM(CurtainActions)

    enum BlindActions {
        BLIND_ACT_STOP      = 0, //!< stop movement
        BLIND_ACT_MOVE      = 1, //!< move to given position
        BLIND_ACT_OPEN      = 2, //!< position 50
        BLIND_ACT_CLOSE_DOWN= 3, //!< position 0
        BLIND_ACT_CLOSE_UP  = 4, //!< position 100
    };
    Q_ENUM(BlindActions)

    ///

    enum SwitchBotUserModes {
        MODE_PRESS          = 0,
        MODE_SWITCH         = 1,
    };
    Q_ENUM(SwitchBotUserModes)

    enum SwitchBotUserActions {
        ACTION_STOP         = 0, //!< stop movement
        ACTION_MOVE         = 1, //!< move to given position
        ACTION_OPEN         = 2, //!<
        ACTION_CLOSE_DOWN   = 3, //!<
        ACTION_CLOSE_UP     = 4, //!<
        ACTION_CLOSE        = 5, //!<

        ACTION_ON           = 10, //!< switch on
        ACTION_OFF          = 11, //!< switch off
        ACTION_PUSH_PULL    = 12, //!< push and pull back
        ACTION_PUSH_STOP    = 13, //!< push stop
        ACTION_BACK         = 14, //!< back
    };
    Q_ENUM(SwitchBotUserActions)
};

/* ************************************************************************** */
#endif // DEVICE_UTILS_SWITCHBOT_H
