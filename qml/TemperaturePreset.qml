import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Qt5Compat.GraphicalEffects
import "qrc:/js/UtilsPresets.js" as UtilsPresets
import ThemeEngine

Item {
    id: temperaturePreset
    anchors.fill: parent

    property var currentPreset: null

    function loadPreset(p) {
        currentPreset = p
    }

    function backAction() {
        if (presetName.focusalias) {
            presetName.focusalias = false
            return false
        }
        return true
    }

    ////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: presetHeader
        anchors.left: parent.left
        anchors.right: parent.right
        z: 4
        height: 96
        color: Theme.colorForeground

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: presetHeader.height
            color: Qt.darker(Theme.colorForeground, 1.03)

            IconSvg {
                anchors.centerIn: parent
                width: 40
                height: 40
                smooth: true
                color: Theme.colorSubText
                source: UtilsPresets.getPresetIcon(currentPreset && currentPreset.type)
            }

            MouseArea {
                anchors.fill: parent
                enabled: currentPreset && !currentPreset.readOnly
                onClicked: typeChooser.isOpen = !typeChooser.isOpen
            }
        }

        TextInputThemed { // preset name
            id: presetName
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: presetHeader.height
            anchors.right: parent.right
            //anchors.bottom: parent.bottom
            height: 48

            //placeholderText: qsTr("Preset name")
            fontsize: Theme.fontSizeHeader
            readOnly: currentPreset && currentPreset.readOnly

            text: currentPreset && currentPreset.name
            onDisplayTextChanged: {
                if (presetsManager.isPresetNameValid(text)) {
                    currentPreset.name = text
                }
            }
            onEditingFinished: {
                if (presetsManager.isPresetNameValid(text)) {
                    currentPreset.name = text
                }
            }
        }

        Row {
            anchors.left: parent.left
            anchors.leftMargin: presetHeader.height + Theme.componentMargin
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 48

            ItemTag {
                anchors.verticalCenter: parent.verticalCenter
                visible: currentPreset && currentPreset.readOnly

                text: qsTr("Read only")
                color: Qt.darker(Theme.colorForeground, 1.04)
            }
        }
    }

    Rectangle {
        id: typeChooser
        anchors.top: presetHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        z: 3

        property bool isOpen: false

        clip: true
        height: isOpen ? presetType.height + Theme.componentMargin*2 : 0
        Behavior on height { NumberAnimation { duration: 333 } }

        color: Qt.darker(Theme.colorForeground, 1.03)

        SelectorGrid {
            id: presetType
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Theme.componentMargin

            btnCols: 4
            btnRows: 2
            btnHeight: 96

            model: ListModel {
                ListElement { idx:  0; txt: ""; src: "qrc:/assets/icons_fontawesome/question-solid.svg"; }
                ListElement { idx:  1; txt: ""; src: "qrc:/assets/icons_fontawesome/cow-solid.svg"; }
                ListElement { idx:  2; txt: ""; src: "qrc:/assets/icons_fontawesome/fish-fins-solid.svg"; }
                ListElement { idx:  3; txt: ""; src: "qrc:/assets/icons_fontawesome/egg-solid.svg"; }
                ListElement { idx:  4; txt: ""; src: "qrc:/assets/icons_fontawesome/kiwi-bird-solid.svg"; }
                ListElement { idx:  5; txt: ""; src: "qrc:/assets/icons_fontawesome/shrimp-solid.svg"; }
                ListElement { idx:  6; txt: ""; src: "qrc:/assets/icons_fontawesome/pepper-hot-solid.svg"; }
                ListElement { idx:  7; txt: ""; src: "qrc:/assets/icons_fontawesome/apple-whole-solid.svg"; }
            }
            currentSelection: 0
            onMenuSelected: (index) => {
                //console.log("SelectorMenu clicked #" + index)
                currentSelection = index
                typeChooser.isOpen = false
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Flickable {
        anchors.top: typeChooser.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: -1
        contentHeight: columnContent.height

        topMargin: Theme.componentMargin
        bottomMargin: Theme.componentMargin

        Column {
            id: columnContent
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            spacing: Theme.componentMargin

            ////////

            Repeater {
                id: presetRepeater

                model: currentPreset && currentPreset.ranges
                TemperatureRangeWidget {
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
            }

            ////////
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    PopupPresetCopy {
        id: popupPresetCopy
    }
    PopupPresetRangeAdd {
        id: popupPresetRangeAdd
    }

    Column {
        anchors.right: parent.right
        anchors.rightMargin: 24
        anchors.bottom: parent.bottom
        anchors.bottomMargin: (Qt.platform.os === "android" && screenOrientation === Qt.PortraitOrientation) ? 12 : 24
        spacing: 24

        RoundButtonIconShadow { // copy
            width: 56
            height: 56

            source: "qrc:/assets/icons_material/duotone-library_copy-24px.svg"
            iconColor: "white"
            background: true
            backgroundColor: Theme.colorPrimary

            onClicked: {
                popupPresetCopy.open()
            }
        }

        RoundButtonIconShadow { // add
            width: 56
            height: 56

            visible: currentPreset && !currentPreset.readOnly

            source: "qrc:/assets/icons_material/baseline-add-24px.svg"
            iconColor: "white"
            background: true
            backgroundColor: Theme.colorPrimary

            onClicked: {
                popupPresetRangeAdd.open()
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
