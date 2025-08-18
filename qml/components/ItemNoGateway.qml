import QtQuick
import QtQuick.Layouts

import ComponentLibrary

Item {
    id: itemNoDevice
    anchors.fill: parent

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -Theme.componentMarginXL

        IconSvg { // magnifying glass icon
            anchors.horizontalCenter: parent.horizontalCenter
            width: (isDesktop || isTablet || (isPhone && screenOrientation === Qt.LandscapeOrientation)) ? 320 : (parent.width*0.66)
            height: width

            source: "qrc:/IconLibrary/material-symbols/search.svg"
            fillMode: Image.PreserveAspectFit
            color: Theme.colorIcon

            SequentialAnimation on opacity {
                id: scanAnimation
                loops: Animation.Infinite
                running: deviceManager.scanning
                alwaysRunToEnd: true

                PropertyAnimation { to: 0.33; duration: 750; }
                PropertyAnimation { to: 1; duration: 750; }
            }
        }
    }
}
