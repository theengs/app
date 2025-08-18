import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls.impl
import QtQuick.Templates as T

import ComponentLibrary

T.Button {
    id: broker

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: 12
    rightPadding: 12

    property bool selected: false

    // broker
    property string name: ""
    property string host: "192.168.1.x"
    property int port: 1883

    // colors
    property color colorContent: Theme.colorComponentText
    property color colorHighlight: Theme.colorComponentDown

    ////////////////

    background: Rectangle {
        implicitWidth: 512
        implicitHeight: Theme.componentHeightXL

        color: broker.selected ? broker.colorHighlight : "transparent"

        RippleThemed {
            anchors.fill: parent
            anchor: broker

            clip: visible
            pressed: broker.pressed
            active: enabled && (broker.down || broker.visualFocus || broker.hovered)
            color: Qt.rgba(broker.colorHighlight.r, broker.colorHighlight.g, broker.colorHighlight.b, 0.66)
        }
    }

    ////////////////

    contentItem: RowLayout {
        spacing: 16

        IconSvg {
            Layout.preferredWidth: 32
            Layout.preferredHeight: 32

            source: {
                return "qrc:/IconLibrary/material-symbols/signal_wifi_0_bar.svg"
            }
            color: Theme.colorIcon
        }

        Column {
            Layout.fillWidth: true

            Text {
                text: broker.name
                color: Theme.colorText
                font.bold: true
            }

            Row {
                Text {
                    text: broker.host + " @ " + broker.port
                    color: Theme.colorSubText
                }
            }
        }

        ButtonClear {
            Layout.preferredHeight: 28
            text: {
                if (modelData.type === 1) return "current"
                if (modelData.type === 2) return "preset"
                if (modelData.type === 3) return "scan"
                return "?"
            }
            color:{
                if (modelData.type === 1) return Theme.colorGreen
                if (modelData.type === 2) return Theme.colorBlue
                if (modelData.type === 3) return Theme.colorPrimary
                return Theme.colorWarning
            }
        }
    }

    ////////////////
}
