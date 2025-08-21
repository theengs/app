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

    // network
    property bool scanning: false
    property bool empty: false

    // colors
    property color colorContent: Theme.colorComponentText
    property color colorHighlight: Theme.colorComponentDown

    ////////////////

    background: Rectangle {
        implicitWidth: 512
        implicitHeight: Theme.componentHeight

        color: "transparent"

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

            text: {
                if (network.scanning) return "Scanning..."
                if (network.empty) return "No network available"
                return ""
            }
            color: Theme.colorText
            font.bold: true
        }

        IconSvg {
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24

            source: network.scanning ? "qrc:/IconLibrary/material-symbols/autorenew.svg"
                                      : "qrc:/IconLibrary/material-symbols/signal_wifi_off.svg"
            color: Theme.colorIcon

            NumberAnimation on rotation {
                running: network.scanning
                alwaysRunToEnd: true
                loops: Animation.Infinite

                duration: 1500
                from: 0
                to: 360
                easing.type: Easing.Linear
            }
        }
    }

    ////////////////
}
