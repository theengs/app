import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls.impl
import QtQuick.Templates as T

import ComponentLibrary

T.Button {
    id: network

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: 12
    rightPadding: 12

    property bool selected: false

    // network
    property string name: ""
    property bool open: false
    property bool connected: false
    property int strength: 0

    // colors
    property color colorContent: Theme.colorComponentText
    property color colorHighlight: Theme.colorComponentDown

    ////////////////

    background: Rectangle {
        implicitWidth: 512
        implicitHeight: 36

        color: network.selected ? network.colorHighlight : "transparent"

        RippleThemed {
            anchors.fill: parent
            anchor: network

            clip: visible
            pressed: network.pressed
            active: enabled && (network.down || network.visualFocus || network.hovered)
            color: Qt.rgba(network.colorHighlight.r, network.colorHighlight.g, network.colorHighlight.b, 0.66)
        }
    }

    ////////////////

    contentItem: RowLayout {
        spacing: 8

        Text {
            Layout.fillWidth: true

            text: network.name
            color: Theme.colorText
            font.bold: true
        }

        TagClear {
            Layout.preferredHeight: 20
            visible: network.connected

            text: qsTr("connected")
            color: Theme.colorGreen
        }

        IconSvg {
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            visible: !network.open

            source: "qrc:/IconLibrary/material-symbols/lock-fill.svg"
            color: Theme.colorIcon
        }

        IconSvg {
            Layout.preferredWidth: 20
            Layout.preferredHeight: 20

            source: {
                if (network.strength === 4) return "qrc:/IconLibrary/material-symbols/signal_wifi_4_bar.svg"
                if (network.strength === 3) return "qrc:/IconLibrary/material-symbols/signal_wifi_3_bar.svg"
                if (network.strength === 2) return "qrc:/IconLibrary/material-symbols/signal_wifi_2_bar.svg"
                if (network.strength === 1) return "qrc:/IconLibrary/material-symbols/signal_wifi_1_bar.svg"
                return "qrc:/IconLibrary/material-symbols/signal_wifi_0_bar.svg"
            }
            color: Theme.colorIcon
        }
    }

    ////////////////
}
