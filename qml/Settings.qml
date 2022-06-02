import QtQuick 2.15
import QtQuick.Controls 2.15

import ThemeEngine 1.0
import "qrc:/js/UtilsNumber.js" as UtilsNumber

Item {
    id: settingsScreen
    width: 480
    height: 720
    anchors.fill: parent

    ////////////////////////////////////////////////////////////////////////////

    Flickable {
        anchors.fill: parent

        contentWidth: -1
        contentHeight: column.height

        boundsBehavior: isDesktop ? Flickable.OvershootBounds : Flickable.DragAndOvershootBounds
        ScrollBar.vertical: ScrollBar { visible: false }

        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right

            topPadding: 12
            bottomPadding: 12
            spacing: 8

            ////////////////

            Rectangle {
                height: 48
                anchors.left: parent.left
                anchors.right: parent.right

                color: Theme.colorForeground

                IconSvg {
                    id: image_appsettings
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: screenPaddingLeft + 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/baseline-settings-20px.svg"
                }

                Text {
                    id: text_appsettings
                    anchors.left: image_appsettings.right
                    anchors.leftMargin: 24
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Application")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    font.bold: false
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }
            }

            ////////////////

            Item {
                id: element_appTheme
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                IconSvg {
                    id: image_appTheme
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/duotone-style-24px.svg"
                }

                Text {
                    id: text_appTheme
                    height: 40
                    anchors.left: image_appTheme.right
                    anchors.leftMargin: 24
                    anchors.right: appTheme_selector.left
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Theme")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }

                Row {
                    id: appTheme_selector
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    z: 1
                    spacing: 10

                    Rectangle {
                        id: rectangleSnow
                        width: wideWideMode ? 80 : 32
                        height: 32
                        anchors.verticalCenter: parent.verticalCenter

                        radius: 2
                        color: "white"
                        border.color: (settingsManager.appTheme === "THEME_SNOW") ? Theme.colorSubText : "#ccc"
                        border.width: 2

                        Text {
                            anchors.centerIn: parent
                            visible: wideWideMode
                            text: qsTr("snow")
                            textFormat: Text.PlainText
                            color: (settingsManager.appTheme === "THEME_SNOW") ? Theme.colorSubText : "#ccc"
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentSmall
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: settingsManager.appTheme = "THEME_SNOW"
                        }
                    }
                    Rectangle {
                        id: rectangleTheengs
                        width: wideWideMode ? 80 : 32
                        height: 32
                        anchors.verticalCenter: parent.verticalCenter

                        radius: 2
                        color: "#325da9"
                        border.color: "#f7931e"
                        border.width: (settingsManager.appTheme === "THEME_THEENGS") ? 2 : 0

                        Text {
                            anchors.centerIn: parent
                            visible: wideWideMode
                            text: qsTr("theengs")
                            textFormat: Text.PlainText
                            color: "white"
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentSmall
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: settingsManager.appTheme = "THEME_THEENGS"
                        }
                    }
                    Rectangle {
                        id: rectangleNight
                        width: wideWideMode ? 80 : 32
                        height: 32
                        anchors.verticalCenter: parent.verticalCenter

                        radius: 2
                        color: "#555151"
                        border.color: Theme.colorPrimary
                        border.width: (settingsManager.appTheme === "THEME_NIGHT") ? 2 : 0

                        Text {
                            anchors.centerIn: parent
                            visible: wideWideMode
                            text: qsTr("night")
                            textFormat: Text.PlainText
                            color: (settingsManager.appTheme === "THEME_NIGHT") ? Theme.colorPrimary : "#ececec"
                            font.bold: true
                            font.pixelSize: Theme.fontSizeContentSmall
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: settingsManager.appTheme = "THEME_NIGHT"
                        }
                    }
                }
            }

            ////////

            Item {
                id: element_appThemeAuto
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                IconSvg {
                    id: image_appThemeAuto
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/duotone-brightness_4-24px.svg"
                }

                Text {
                    id: text_appThemeAuto
                    height: 40
                    anchors.left: image_appThemeAuto.right
                    anchors.leftMargin: 24
                    anchors.right: switch_appThemeAuto.left
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Automatic dark mode")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }

                SwitchThemedMobile {
                    id: switch_appThemeAuto
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.verticalCenter: parent.verticalCenter
                    z: 1

                    checked: settingsManager.appThemeAuto
                    onClicked: {
                        settingsManager.appThemeAuto = checked
                        Theme.loadTheme(settingsManager.appTheme)
                    }
                }
            }
            Text {
                id: legend_appThemeAuto
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft + 64
                anchors.right: parent.right
                anchors.rightMargin: 12

                topPadding: -12
                bottomPadding: isMobile ? 12 : 0
                visible: element_appThemeAuto.visible

                text: qsTr("Dark mode will switch on automatically between 9 PM and 9 AM.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            Item { // spacer
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                visible: isDesktop
            }

            ////////
/*
            Rectangle {
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.colorSeparator
            }

            ////////

            Item {
                id: element_language
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                IconSvg {
                    id: image_language
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/duotone-translate-24px.svg"
                }

                Text {
                    id: text_language
                    height: 40
                    anchors.left: image_language.right
                    anchors.leftMargin: 24
                    anchors.right: combobox_language.left
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Language")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }

                ComboBoxThemed {
                    id: combobox_language
                    width: wideMode ? 256 : 160
                    height: 36
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter

                    z: 1
                    enabled: false
                    wheelEnabled: false

                    model: ListModel {
                        id: cbAppLanguage
                        ListElement {
                            text: qsTr("auto", "short for automatic");
                        }
                        ListElement { text: "Chinese (traditional)"; }
                        ListElement { text: "Chinese (simplified)"; }
                        ListElement { text: "Dansk"; }
                        ListElement { text: "Deutsch"; }
                        ListElement { text: "English"; }
                        ListElement { text: "Español"; }
                        ListElement { text: "Français"; }
                        ListElement { text: "Frysk"; }
                        ListElement { text: "Nederlands"; }
                        ListElement { text: "Norsk (Bokmål)"; }
                        ListElement { text: "Norsk (Nynorsk)"; }
                        ListElement { text: "Pусский"; }
                    }

                    Component.onCompleted: {
                        for (var i = 0; i < cbAppLanguage.count; i++) {
                            if (cbAppLanguage.get(i).text === settingsManager.appLanguage)
                                currentIndex = i
                        }
                    }
                    property bool cbinit: false
                    onCurrentIndexChanged: {
                        if (cbinit) {
                            utilsLanguage.loadLanguage(cbAppLanguage.get(currentIndex).text)
                            settingsManager.appLanguage = cbAppLanguage.get(currentIndex).text
                        } else {
                            cbinit = true
                        }
                    }
                }
            }

            ////////

            Rectangle {
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.colorSeparator
            }
*/
            ////////////////

            Rectangle {
                height: 48
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.colorForeground

                visible: (element_bluetoothControl.visible) // || element_bluetoothSimUpdate.visible)

                IconSvg {
                    id: image_ble
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: screenPaddingLeft + 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/baseline-bluetooth-24px.svg"
                }

                Text {
                    id: text_ble
                    anchors.left: image_ble.right
                    anchors.leftMargin: 24
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Bluetooth")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    font.bold: false
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }
            }

            ////////////////

            Item {
                id: element_bluetoothControl
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                // Android only
                visible: (Qt.platform.os === "android")

                IconSvg {
                    id: image_bluetoothControl
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/duotone-settings_bluetooth-24px.svg"
                }

                Text {
                    id: text_bluetoothControl
                    height: 40
                    anchors.left: image_bluetoothControl.right
                    anchors.leftMargin: 24
                    anchors.right: switch_bluetoothControl.left
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Bluetooth control")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }

                SwitchThemedMobile {
                    id: switch_bluetoothControl
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.verticalCenter: parent.verticalCenter
                    z: 1

                    checked: settingsManager.bluetoothControl
                    onClicked: settingsManager.bluetoothControl = checked
                }
            }
            Text {
                id: legend_bluetoothControl
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft + 64
                anchors.right: parent.right
                anchors.rightMargin: 12

                topPadding: -12
                bottomPadding: 0
                visible: element_bluetoothControl.visible

                text: qsTr("Theengs can activate your device's Bluetooth in order to operate.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }
/*
            Rectangle {
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.colorSeparator

                // Android only
                visible: (Qt.platform.os === "android")
            }

            ////////

            Item {
                id: element_bluetoothSimUpdate
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                IconSvg {
                    id: image_bluetoothSimUpdate
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/duotone-settings_bluetooth-24px.svg"
                }

                Text {
                    id: text_bluetoothSimUpdate
                    height: 40
                    anchors.left: image_bluetoothSimUpdate.right
                    anchors.leftMargin: 24
                    anchors.right: isDesktop ? undefined : spinBox_bluetoothSimUpdate.left
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Simultaneous updates")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    color: Theme.colorText
                    font.pixelSize: Theme.fontSizeContent
                    verticalAlignment: Text.AlignVCenter
                }

                SliderValueSolid {
                    id: slider_bluetoothSimUpdate
                    anchors.left: text_bluetoothSimUpdate.right
                    anchors.leftMargin: 16
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter

                    visible: isDesktop
                    z: 1

                    from: 1
                    to: 6
                    stepSize: 1
                    wheelEnabled: false

                    value: settingsManager.bluetoothSimUpdates
                    onMoved: settingsManager.bluetoothSimUpdates = value
                }
                SpinBoxThemed {
                    id: spinBox_bluetoothSimUpdate
                    width: 128
                    height: 36
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter

                    visible: isMobile
                    z: 1

                    from: 1
                    to: 6
                    stepSize: 1
                    editable: false

                    value: settingsManager.bluetoothSimUpdates
                    onValueModified: settingsManager.bluetoothSimUpdates = value
                }
            }
            Text {
                id: legend_bluetoothSimUpdate
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft + 64
                anchors.right: parent.right
                anchors.rightMargin: 12
                topPadding: -12
                bottomPadding: 12

                visible: element_bluetoothSimUpdate.visible

                text: qsTr("How many sensors should be updated at once. A lower number improves Bluetooth synchronization reliability, at the expense of speed.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            Rectangle {
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.colorSeparator
                visible: (Qt.platform.os !== "ios")
            }
*/            
            ////////////////

            Rectangle {
                height: 48
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.colorForeground

                // every platforms except iOS
                visible: (Qt.platform.os !== "ios")

                IconSvg {
                    id: image_androidservice
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: screenPaddingLeft + 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/duotone-tap_and_play_black-24px.svg"
                }

                Text {
                    id: text_androidservice
                    anchors.left: image_androidservice.right
                    anchors.leftMargin: 24
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Background updates")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    font.bold: false
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }

                ButtonExperimental {
                    anchors.right: parent.right
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    height: 32

                    visible: (Qt.platform.os === "android")

                    text: qsTr("experimental")
                    primaryColor: Theme.colorRed
                    borderColor: Theme.colorRed

                    onClicked: popupBackgroundData.open()
                }
            }

            ////////////////

            Item {
                id: element_worker
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                // every platforms except iOS
                visible: (Qt.platform.os !== "ios")

                IconSvg {
                    id: image_worker
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: screenPaddingLeft + 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/baseline-autorenew-24px.svg"
                }

                Text {
                    id: text_worker
                    height: 40
                    anchors.left: image_worker.right
                    anchors.leftMargin: 24
                    anchors.right: switch_worker.left
                    anchors.rightMargin: 0
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Enable background updates")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }

                SwitchThemedMobile {
                    id: switch_worker
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.verticalCenter: parent.verticalCenter
                    z: 1

                    checked: settingsManager.systray
                    onClicked: {
                        settingsManager.systray = checked

                        if (isMobile) {
                            if (settingsManager.systray) {
                                popupBackgroundData.open()
                                utilsApp.getMobileBackgroundLocationPermission()
                            }
                        }
                    }
                }
            }
            Text {
                id: legend_worker_mobile
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft + 64
                anchors.right: parent.right
                anchors.rightMargin: 12
                topPadding: -12
                bottomPadding: 0

                visible: (Qt.platform.os === "android") // && (settingsManager.systray && element_worker.visible)

                text: qsTr("Wake up at a predefined interval to refresh sensor data. Only if Bluetooth (or Bluetooth control) is enabled.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }
            Text {
                id: legend_worker_desktop
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft + 64
                anchors.right: parent.right
                anchors.rightMargin: 12
                topPadding: -12
                bottomPadding: 0

                visible: isDesktop // && (settingsManager.systray && element_worker.visible)

                text: qsTr("Theengs will remain active in the system tray, and will wake up at a regular interval to refresh sensor data.")
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                color: Theme.colorSubText
                font.pixelSize: Theme.fontSizeContentSmall
            }

            ////////

            Item {
                id: element_update_background
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                // every platforms except iOS
                visible: (Qt.platform.os !== "ios")

                IconSvg {
                    id: image_update_background
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/duotone-timer-24px.svg"
                }

                Text {
                    id: text_update_background
                    height: 40
                    anchors.left: image_update_background.right
                    anchors.leftMargin: 24
                    anchors.right: spinBox_update_background.left
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Update interval")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }

                SpinBoxThemed {
                    id: spinBox_update_background
                    width: 128
                    height: 36
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    z: 1

                    enabled: (settingsManager.systray && element_worker.visible)

                    legend: " " + qsTr("m.", "short for minutes")
                    from: 5
                    to: 360
                    stepSize: 5
                    editable: false
                    wheelEnabled: isDesktop

                    value: (settingsManager.updateIntervalBackground)
                    onValueModified: settingsManager.updateIntervalBackground = value
                }
            }

            ////////////////

            Rectangle {
                height: 48
                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.colorForeground

                IconSvg {
                    id: image_plantsensor
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: screenPaddingLeft + 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/logos/logo-greyscale.svg"
                }

                Text {
                    id: text_plantsensor
                    anchors.left: image_plantsensor.right
                    anchors.leftMargin: 24
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Sensors")
                    textFormat: Text.PlainText
                    font.pixelSize: Theme.fontSizeContent
                    font.bold: false
                    color: Theme.colorText
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }
            }

            ////////////////

            Item {
                id: element_units
                height: 48
                anchors.left: parent.left
                anchors.leftMargin: screenPaddingLeft
                anchors.right: parent.right
                anchors.rightMargin: screenPaddingRight

                IconSvg {
                    id: image_units
                    width: 24
                    height: 24
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    color: Theme.colorIcon
                    source: "qrc:/assets/icons_material/baseline-ac_unit-24px.svg"
                }

                Text {
                    id: text_units
                    height: 40
                    anchors.left: image_units.right
                    anchors.leftMargin: 24
                    anchors.right: row_units.left
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Unit system")
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeContent
                    color: Theme.colorText
                    verticalAlignment: Text.AlignVCenter
                }

                Row {
                    id: row_units
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: text_units.verticalCenter
                    spacing: 12

                    RadioButtonThemed {
                        text: qsTr("Metric")
                        checked: (settingsManager.appUnits === 0)
                        onClicked: settingsManager.appUnits = 0
                    }

                    RadioButtonThemed {
                        text: qsTr("Imperial")
                        checked: (settingsManager.appUnits === 1)
                        onClicked: settingsManager.appUnits = 1
                    }
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
