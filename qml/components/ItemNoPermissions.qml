import QtQuick 2.15

import ThemeEngine 1.0

Item {
    anchors.fill: parent

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 32
        anchors.right: parent.right
        anchors.rightMargin: 32
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -20

        IconSvg { // imageLock
            width: (isDesktop || isTablet || (isPhone && appWindow.screenOrientation === Qt.LandscapeOrientation)) ? 256 : (parent.width*0.666)
            height: width
            anchors.horizontalCenter: parent.horizontalCenter

            source: "qrc:/assets/icons_material/outline-lock-24px.svg"
            fillMode: Image.PreserveAspectFit
            color: Theme.colorIcon
        }

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 16

            ////////

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 4

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    text: qsTr("Authorization to use Bluetooth is required to connect to the sensors.")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContentSmall
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //visible: (Qt.platform.os === "android")

                    text: qsTr("On Android 6+, scanning for Bluetooth Low Energy devices requires <b>location permission</b>.")
                    textFormat: Text.StyledText
                    font.pixelSize: Theme.fontSizeContentSmall
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
/*
                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //visible: (Qt.platform.os === "android")

                    text: qsTr("On Android 10+, <b>background location permission</b> is required to use background updates.")
                    textFormat: Text.StyledText
                    font.pixelSize: Theme.fontSizeContentSmall
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
*/
                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //visible: (Qt.platform.os === "android" && !utilsApp.isMobileGpsEnabled())

                    text: qsTr("Some Android devices also require the actual <b>GPS to be turned on</b>.")
                    textFormat: Text.StyledText
                    font.pixelSize: Theme.fontSizeContentSmall
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //visible: (Qt.platform.os === "android")

                    text: qsTr("The application is neither using nor storing your location. Sorry for the inconvenience.")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContentSmall
                    color: Theme.colorSubText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            ////////

            ButtonWireframeIcon {
                anchors.horizontalCenter: parent.horizontalCenter
                //visible: (Qt.platform.os === "android" || Qt.platform.os === "ios")

                text: qsTr("Official information")
                primaryColor: Theme.colorSubText
                sourceSize: 20
                source: "qrc:/assets/icons_material/duotone-launch-24px.svg"

                onClicked: {
                    if (Qt.platform.os === "android") {
                        Qt.openUrlExternally("https://developer.android.com/guide/topics/connectivity/bluetooth/permissions#declare-android11-or-lower")
                    } else if (Qt.platform.os === "ios") {
                        Qt.openUrlExternally("https://support.apple.com/HT210578")
                    }
                }
            }

            ////////
        }
    }
}
