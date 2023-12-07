import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import ThemeEngine

Rectangle {
    id: temperatureRangeWidget

    implicitWidth: 256
    implicitHeight: 80

    height: columnContent.height + 8
    radius: Theme.componentRadius
    color: Theme.colorForeground

    ////////////////////////////////////////////////////////////////////////////

    Loader {
        id: popupDeleteLoader

        active: false
        asynchronous: false

        sourceComponent: PopupPresetRangeDelete {
            id: popupDelete
            onConfirmed: currentPreset.removeRange(modelData.name)
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Column {
        id: columnContent
        anchors.left: parent.left
        anchors.right: parent.right

        TextInputThemed { // range name
            anchors.left: parent.left
            anchors.right: parent.right
            height: 44

            readOnly: currentPreset.readOnly
            text: modelData.name

            onDisplayTextChanged: {
                if (currentPreset.isRangeNameValid(text)) {
                    modelData.name = text
                }
            }
            onEditingFinished: {
                if (currentPreset.isRangeNameValid(text)) {
                    modelData.name = text
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 2
                z: -1
                color: Theme.colorSeparator
            }
        }

        Item {
            width: 12
            height: 6
        }

        RowLayout {
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            height: 44

            Text {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                text: qsTr("Minimum temp.")
                color: Theme.colorSubText
            }
            SpinBoxThemed {
                Layout.preferredHeight: 34
                Layout.alignment: Qt.AlignVCenter

                enabled: !currentPreset.readOnly
                editable: !currentPreset.readOnly

                from: 0
                to: 200
                value: modelData.tempMin
                onValueModified: modelData.tempMin = value
            }
        }

        RowLayout {
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            height: 44

            Text {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                text: modelData.tempMaxEnabled ? qsTr("Maximum temp.") : qsTr("No maximum")
                color: Theme.colorSubText
            }

            ButtonWireframe {
                Layout.preferredHeight: 34
                Layout.alignment: Qt.AlignVCenter

                visible: (!currentPreset.readOnly && modelData.tempMaxDisabled)
                text: qsTr("set")

                onClicked: {
                    modelData.tempMaxDisabled = false
                    modelData.tempMax = modelData.tempMin + 4
                }
            }

            SpinBoxThemed {
                Layout.preferredHeight: 34
                Layout.alignment: Qt.AlignVCenter

                visible: modelData.tempMaxEnabled
                enabled: !currentPreset.readOnly
                editable: !currentPreset.readOnly

                from: 0
                to: 200
                value: modelData.tempMax
                onValueModified: modelData.tempMax = value
            }
        }

        Item {
            anchors.left: parent.left
            anchors.leftMargin: Theme.componentMargin
            anchors.right: parent.right
            anchors.rightMargin: Theme.componentMargin
            height: 44

            visible: !currentPreset.readOnly

            ButtonWireframeIcon {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                height: 34
                text: qsTr("Delete")

                onClicked: {
                    popupDeleteLoader.active = true
                    popupDeleteLoader.item.open()
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
