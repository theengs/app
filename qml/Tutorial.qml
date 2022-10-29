import QtQuick 2.15
import QtQuick.Controls 2.15

import ThemeEngine 1.0

Rectangle {
    width: 480
    height: 720
    anchors.fill: parent

    color: Theme.colorHeader

    property string entryPoint: "DeviceList"

    ////////////////////////////////////////////////////////////////////////////

    function loadScreen() {
        // Load the tutorial
        if (!tutorialLoader.sourceComponent) {
            tutorialLoader.sourceComponent = componentTutorial
        } else {
            tutorialLoader.item.reset()
        }

        // Change screen
        appContent.state = "Tutorial"
    }

    function loadScreenFrom(screenname) {
        entryPoint = screenname
        loadScreen()
    }

    ////////////////////////////////////////////////////////////////////////////

    Loader {
        id: tutorialLoader
        anchors.fill: parent

        sourceComponent: null
        asynchronous: false
    }

    ////////////////////////////////////////////////////////////////////////////

    Component {
        id: componentTutorial

        Item {
            id: itemTutorial

            function reset() {
                tutorialPages.disableAnimation()
                tutorialPages.currentIndex = 0
                tutorialPages.enableAnimation()
            }

            SwipeView {
                id: tutorialPages
                anchors.fill: parent
                anchors.leftMargin: screenPaddingLeft
                anchors.rightMargin: screenPaddingRight
                anchors.bottomMargin: 56

                currentIndex: 0
                onCurrentIndexChanged: {
                    if (currentIndex < 0) currentIndex = 0
                    if (currentIndex > count-1) {
                        currentIndex = 0 // reset
                        appContent.state = entryPoint
                    }
                }

                function enableAnimation() {
                    contentItem.highlightMoveDuration = 333
                }
                function disableAnimation() {
                    contentItem.highlightMoveDuration = 0
                }

                ////////

                Item {
                    id: page1

                    Column {
                        id: column
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 32

                        Text {
                            anchors.right: parent.right
                            anchors.rightMargin: 32
                            anchors.left: parent.left
                            anchors.leftMargin: 32

                            text: qsTr("<b>Theengs</b> theengs theengs.")
                            textFormat: Text.StyledText
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHeaderContent
                            horizontalAlignment: Text.AlignHCenter
                        }
                        IconSvg {
                            width: tutorialPages.width * (tutorialPages.height > tutorialPages.width ? 0.8 : 0.4)
                            height: width*0.229
                            anchors.horizontalCenter: parent.horizontalCenter

                            smooth: true
                            source: "qrc:/assets/logos/logo-greyscale.svg"
                            color: Theme.colorHeaderContent
                            fillMode: Image.PreserveAspectFit
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.rightMargin: 32
                            anchors.left: parent.left
                            anchors.leftMargin: 32

                            text: qsTr("…theengs…")
                            textFormat: Text.StyledText
                            color: Theme.colorHeaderContent
                            font.pixelSize: Theme.fontSizeContentBig
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }
                    }
                }

                Item {
                    id: page2

                    Column {
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 32

                        Text {
                            anchors.right: parent.right
                            anchors.rightMargin: 32
                            anchors.left: parent.left
                            anchors.leftMargin: 32

                            text: qsTr("theengs")
                            textFormat: Text.StyledText
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHeaderContent
                            horizontalAlignment: Text.AlignHCenter
                        }
                        IconSvg {
                            width: tutorialPages.width * (tutorialPages.height > tutorialPages.width ? 0.8 : 0.4)
                            height: width*0.229
                            anchors.horizontalCenter: parent.horizontalCenter

                            smooth: true
                            source: "qrc:/assets/logos/logo-greyscale.svg"
                            color: Theme.colorHeaderContent
                            fillMode: Image.PreserveAspectFit
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.rightMargin: 32
                            anchors.left: parent.left
                            anchors.leftMargin: 32

                            text: qsTr("theengs")
                            textFormat: Text.StyledText
                            color: Theme.colorHeaderContent
                            font.pixelSize: Theme.fontSizeContentBig
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }
                    }
                }

                Item {
                    id: page3

                    Column {
                        anchors.right: parent.right
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 32

                        Text {
                            anchors.right: parent.right
                            anchors.rightMargin: 32
                            anchors.left: parent.left
                            anchors.leftMargin: 32

                            text: qsTr("theengs")
                            textFormat: Text.StyledText
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pixelSize: Theme.fontSizeContentBig
                            color: Theme.colorHeaderContent
                            horizontalAlignment: Text.AlignHCenter
                        }
                        IconSvg {
                            width: tutorialPages.width * (tutorialPages.height > tutorialPages.width ? 0.8 : 0.4)
                            height: width*0.229
                            anchors.horizontalCenter: parent.horizontalCenter

                            smooth: true
                            source: "qrc:/assets/logos/logo-greyscale.svg"
                            color: Theme.colorHeaderContent
                            fillMode: Image.PreserveAspectFit
                        }

                        Text {
                            anchors.right: parent.right
                            anchors.rightMargin: 32
                            anchors.left: parent.left
                            anchors.leftMargin: 32

                            text: qsTr("theengs")
                            textFormat: Text.StyledText
                            color: Theme.colorHeaderContent
                            font.pixelSize: Theme.fontSizeContentBig
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }
                    }
                }
            }

            ////////

            Text {
                id: pagePrevious
                anchors.left: parent.left
                anchors.leftMargin: 32
                anchors.verticalCenter: pageIndicator.verticalCenter

                visible: (tutorialPages.currentIndex !== 0)

                text: qsTr("Previous")
                textFormat: Text.PlainText
                color: Theme.colorHeaderContent
                font.bold: true
                font.pixelSize: Theme.fontSizeContent

                opacity: 0.8
                Behavior on opacity { OpacityAnimator { duration: 133 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.opacity = 1
                    onExited: parent.opacity = 0.8
                    onClicked: tutorialPages.currentIndex--
                }
            }

            PageIndicatorThemed {
                id: pageIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16

                count: tutorialPages.count
                currentIndex: tutorialPages.currentIndex
            }

            Text {
                id: pageNext
                anchors.right: parent.right
                anchors.rightMargin: 32
                anchors.verticalCenter: pageIndicator.verticalCenter

                text: (tutorialPages.currentIndex === tutorialPages.count-1) ? qsTr("All right!") : qsTr("Next")
                textFormat: Text.PlainText
                color: Theme.colorHeaderContent
                font.bold: true
                font.pixelSize: Theme.fontSizeContent

                opacity: 0.8
                Behavior on opacity { OpacityAnimator { duration: 133 } }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.opacity = 1
                    onExited: parent.opacity = 0.8
                    onClicked: tutorialPages.currentIndex++
                }
            }
        }
    }
}
