import QtQuick 2.15

import ThemeEngine 1.0

Item {
    id: control

    implicitWidth: 256
    implicitHeight: 48

    property string text: ""

    property int fontsize: Theme.fontSizeContentBig
    property int padding: Theme.componentMargin
    property var color: Theme.colorPrimary

    signal editingFinished

    TextInput {
        id: textInput
        anchors.left: parent.left
        anchors.leftMargin: control.padding
        anchors.right: parent.right
        anchors.rightMargin: control.padding
        anchors.verticalCenter: parent.verticalCenter

        color: Theme.colorText
        selectByMouse: true
        selectedTextColor: "white"
        selectionColor: control.color
        font.pixelSize: control.fontsize

        text: control.text
        onEditingFinished: {
            focus = false
            control.text = text
            control.editingFinished()
        }
    }

    IconSvg {
        anchors.right: parent.right
        anchors.rightMargin: control.padding
        anchors.verticalCenter: parent.verticalCenter
        width: 24; height: 24;

        source: "qrc:/assets/icons_material/duotone-edit-24px.svg"
        color: Theme.colorSubText

        opacity: (isMobile || !textInput.text || textInput.focus || textInput.containsMouse) ? 0.9 : 0
        Behavior on opacity { OpacityAnimator { duration: 133 } }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 2
        color: control.color
        opacity: textInput.focus
        Behavior on opacity { OpacityAnimator { duration: 133 } }
    }
}
