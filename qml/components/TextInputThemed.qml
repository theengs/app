import QtQuick 2.15

import ThemeEngine 1.0

Item {
    id: control

    implicitWidth: 256
    implicitHeight: 48

    property string text

    property int padding: Theme.componentMargin
    property int fontsize: Theme.fontSizeContentBig
    property var color: Theme.colorPrimary
    property bool readOnly: false

    property alias focusalias: textInput.focus

    signal editingFinished()
    signal displayTextChanged()

    ////////

    TextInput {
        id: textInput
        anchors.left: parent.left
        anchors.leftMargin: control.padding
        anchors.right: parent.right
        anchors.rightMargin: control.padding
        anchors.verticalCenter: parent.verticalCenter

        text: control.text
        font.pixelSize: control.fontsize

        color: Theme.colorText
        selectByMouse: true
        selectedTextColor: "white"
        selectionColor: control.color
        readOnly: control.readOnly

        Keys.onBackPressed: focus = false
        onEditingFinished: {
            focus = false
            control.text = text
            control.editingFinished()
        }
        onDisplayTextChanged: {
            control.text = displayText
            control.displayTextChanged()
        }
    }

    ////////

    IconSvg {
        anchors.right: parent.right
        anchors.rightMargin: control.padding
        anchors.verticalCenter: parent.verticalCenter
        width: 24; height: 24;

        source: "qrc:/assets/icons_material/duotone-edit-24px.svg"
        color: Theme.colorSubText

        opacity: !readOnly && (isMobile || !textInput.text || textInput.focus || textInput.containsMouse) ? 0.9 : 0
        Behavior on opacity { OpacityAnimator { duration: 133 } }
    }

    ////////

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 2
        color: control.color
        opacity: textInput.focus
        Behavior on opacity { OpacityAnimator { duration: 133 } }
    }

    ////////
}
