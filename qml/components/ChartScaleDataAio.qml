import QtQuick 2.15
import QtCharts 2.15

import ThemeEngine 1.0
import "qrc:/js/UtilsNumber.js" as UtilsNumber

Item {
    id: chartScaleDataAio
    anchors.fill: parent

    property bool useOpenGL: true
    property bool showGraphDots: settingsManager.graphShowDots
    property color legendColor: Theme.colorSubText

    function loadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartScaleDataAio // loadGraph() >> " + currentDevice)

        weightData.visible = currentDevice.hasWeight
        impedanceData.visible = currentDevice.hasImpedance

        dateIndicator.visible = false
        dataIndicator.visible = false
        verticalIndicator.visible = false

        legendColor = Qt.rgba(legendColor.r, legendColor.g, legendColor.b, 0.8)
    }

    function updateGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartScaleDataAio // updateGraph() >> " + currentDevice)

        var days = 14
        var count = 0 // currentDevice.countDataNamed("weight", days)

        //// DATA
        weightData.clear()
        impedanceData.clear()

        currentDevice.getChartData_scaleAIO(days, axisTime, weightData, impedanceData)

        //// AXIS
        axisWeight.min = 0
        axisWeight.max = 150
        axisImpedance.min = 0
        axisImpedance.max = 1000

        // min/max axis
        axisWeight.min = currentDevice.weightMin*0.85
        axisWeight.max = currentDevice.weightMax*1.15
        axisImpedance.min = currentDevice.impedanceMin*0.85
        axisImpedance.max = currentDevice.impedanceMax*1.15

        // Graph visibility
        count = weightData.count
        aioGraph.visible = (count > 1)
        noDataIndicator.visible = (count <= 0)
        showGraphDots = (settingsManager.graphShowDots && count < 16)

        // Update indicator (only works if data are changed in database though...)
        //if (dateIndicator.visible) updateIndicator()
    }

    function qpoint_lerp(p0, p1, x) { return (p0.y + (x - p0.x) * ((p1.y - p0.y) / (p1.x - p0.x))) }

    ////////////////////////////////////////////////////////////////////////////

    ChartView {
        id: aioGraph
        anchors.fill: parent
        anchors.topMargin: -28
        anchors.leftMargin: -24
        anchors.rightMargin: -24
        anchors.bottomMargin: -24

        antialiasing: true
        legend.visible: false
        backgroundRoundness: 0
        backgroundColor: Theme.colorBackground
        animationOptions: ChartView.NoAnimation

        ValueAxis { id: axisWeight; visible: false; gridVisible: false; }
        ValueAxis { id: axisImpedance; visible: false; gridVisible: false; }

        DateTimeAxis { id: axisTime; visible: true;
                       labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                       color: legendColor;
                       gridLineColor: Theme.colorSeparator; }

        LineSeries {
            id: weightData
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorGreen; width: 2;
            axisY: axisWeight; axisX: axisTime;
        }
        LineSeries {
            id: impedanceData
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorYellow; width: 2;
            axisY: axisImpedance; axisX: axisTime;
        }

        MouseArea {
            id: clickableGraphArea
            anchors.fill: aioGraph

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: (mouse) => {
                if (mouse.button === Qt.LeftButton) {
                    aioGraph.moveIndicator(mouse, false)
                    mouse.accepted = true
                }
                else if (mouse.button === Qt.RightButton) {
                    resetIndicator()
                }
            }
        }

        function moveIndicator(mouse, isMoving) {
            var mmm = Qt.point(mouse.x, mouse.y)

            // we adjust coordinates with graph area margins
            var ppp = Qt.point(mouse.x, mouse.y)
            ppp.x = ppp.x + aioGraph.anchors.rightMargin
            ppp.y = ppp.y - aioGraph.anchors.topMargin

            // map mouse position to graph value // mpmp.x is the timestamp
            var mpmp = aioGraph.mapToValue(mmm, weightData)

            //console.log("clicked " + mouse.x + " " + mouse.y)
            //console.log("clicked adjusted " + ppp.x + " " + ppp.y)
            //console.log("clicked mapped " + mpmp.x + " " + mpmp.y)

            if (isMoving) {
                // dragging outside the graph area?
                if (mpmp.x < weightData.at(0).x){
                    ppp.x = aioGraph.mapToPosition(weightData.at(0), weightData).x + aioGraph.anchors.rightMargin
                    mpmp.x = weightData.at(0).x
                }
                if (mpmp.x > weightData.at(weightData.count-1).x){
                    ppp.x = aioGraph.mapToPosition(weightData.at(weightData.count-1), weightData).x + aioGraph.anchors.rightMargin
                    mpmp.x = weightData.at(weightData.count-1).x
                }
            } else {
                // did we clicked outside the graph area?
                if (mpmp.x < weightData.at(0).x || mpmp.x > weightData.at(weightData.count-1).x) {
                    resetIndicator()
                    return
                }
            }

            // indicators is now visible
            dateIndicator.visible = true
            verticalIndicator.visible = true
            verticalIndicator.x = ppp.x
            verticalIndicator.clickedCoordinates = mpmp

            // update the indicator data
            updateIndicator()
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    ItemNoData {
        id: noDataIndicator
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    ////////////////

    Rectangle {
        id: verticalIndicator
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 26

        width: 2
        visible: false
        opacity: 0.66
        color: Theme.colorSubText

        property var clickedCoordinates: null

        Behavior on x { NumberAnimation { id: vanim; duration: 333; easing.type: Easing.InOutCubic; } }

        MouseArea {
            id: verticalIndicatorArea
            anchors.fill: parent
            anchors.margins: isMobile ? -24 : -8

            propagateComposedEvents: true
            hoverEnabled: false

            onReleased: {
                if (typeof (sensorPages) !== "undefined") sensorPages.interactive = isPhone
                vanim.duration = 266
            }
            onPositionChanged: (mouse) => {
                if (typeof (sensorPages) !== "undefined") {
                    // So we don't swipe pages as we drag the indicator
                    sensorPages.interactive = false
                }
                vanim.duration = 16

                var mouseMapped = mapToItem(clickableGraphArea, mouse.x, mouse.y)
                aioGraph.moveIndicator(mouseMapped, true)
                mouse.accepted = true
            }
        }

        onXChanged: {
            if (isPhone) return // verticalIndicator default to middle
            if (isTablet) return // verticalIndicator default to middle

            var direction = "middle"
            if (verticalIndicator.x > dateIndicator.width + 48)
                direction = "right"
            else if (chartScaleDataAio.width - verticalIndicator.x > dateIndicator.width + 48)
                direction = "left"

            if (direction === "middle") {
                // date indicator is too big, center on screen
                indicators.columns = 2
                indicators.rows = 1
                indicators.state = "reanchoredmid"
                indicators.layoutDirection = "LeftToRight"
            } else {
                // date indicator is positioned next to the vertical indicator
                indicators.columns = 1
                indicators.rows = 2
                if (direction === "left") {
                    indicators.state = "reanchoredleft"
                    indicators.layoutDirection = "LeftToRight"
                } else {
                    indicators.state = "reanchoredright"
                    indicators.layoutDirection = "RightToLeft"
                }
            }
        }
    }

    ////////////////

    Grid {
        id: indicators
        anchors.top: parent.top
        anchors.topMargin: isPhone ? 16 : 20
        anchors.leftMargin: isPhone ? 20 : 24
        anchors.rightMargin: isPhone ? 20 : 24
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: 32
        layoutDirection: "LeftToRight"
        columns: 2
        rows: 1

        transitions: Transition { AnchorAnimation { duration: 133; } }
        //move: Transition { NumberAnimation { properties: "x"; duration: 133; } }

        states: [
            State {
                name: "reanchoredmid"
                AnchorChanges {
                    target: indicators
                    anchors.right: undefined
                    anchors.left: undefined
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            },
            State {
                name: "reanchoredleft"
                AnchorChanges {
                    target: indicators
                    anchors.horizontalCenter: undefined
                    anchors.right: undefined
                    anchors.left: verticalIndicator.right
                }
            },
            State {
                name: "reanchoredright"
                AnchorChanges {
                    target: indicators
                    anchors.horizontalCenter: undefined
                    anchors.left: undefined
                    anchors.right: verticalIndicator.right
                }
            }
        ]

        Text {
            id: dateIndicator

            visible: false
            font.pixelSize: 15
            font.bold: true
            color: Theme.colorSubText

            Rectangle {
                anchors.fill: parent
                anchors.topMargin: -8
                anchors.leftMargin: -12
                anchors.rightMargin: -12
                anchors.bottomMargin: -8
                z: -1
                radius: 4
                color: Theme.colorForeground
                border.width: Theme.componentBorderWidth
                border.color: Theme.colorSeparator
            }
        }

        Text {
            id: dataIndicator

            visible: false
            font.pixelSize: 15
            font.bold: true
            color: Theme.colorSubText

            Rectangle {
                anchors.fill: parent
                anchors.topMargin: -8
                anchors.leftMargin: -12
                anchors.rightMargin: -12
                anchors.bottomMargin: -8
                z: -1
                radius: 4
                color: Theme.colorForeground
                border.width: Theme.componentBorderWidth
                border.color: Theme.colorSeparator
            }
        }
    }

    ////////////////

    MouseArea {
        anchors.fill: indicators
        anchors.margins: -8
        onClicked: resetIndicator()
    }

    onWidthChanged: resetIndicator()

    function isIndicator() {
        return verticalIndicator.visible
    }

    function resetIndicator() {
        dateIndicator.visible = false
        dataIndicator.visible = false
        verticalIndicator.visible = false
        verticalIndicator.clickedCoordinates = null
    }

    function updateIndicator() {
        if (!dateIndicator.visible) return

        var wei = -99
        var imp = -99

        // set date & time
        var date = new Date(verticalIndicator.clickedCoordinates.x)
        var date_string = date.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
        //: "at" is used for DATE at HOUR
        var time_string = qsTr("at") + " " + UtilsNumber.padNumber(date.getHours(), 2) + ":" + UtilsNumber.padNumber(date.getMinutes(), 2)
        dateIndicator.text = date_string + " " + time_string

        // search index corresponding to the timestamp
        var x1 = -1
        var x2 = -1
        for (var i = 0; i < weightData.count; i++) {
            var graph_at_x = weightData.at(i).x
            var dist = (graph_at_x - verticalIndicator.clickedCoordinates.x) / 1000000

            if (dist < 0) {
                if (x1 < i) x1 = i
            } else {
                x2 = i
                break
            }
        }
        if (x1 >= 0 && x2 > x1) {
            // linear interpolation
            wei = qpoint_lerp(weightData.at(x1), weightData.at(x2), verticalIndicator.clickedCoordinates.x)
            imp = qpoint_lerp(impedanceData.at(x1), impedanceData.at(x2), verticalIndicator.clickedCoordinates.x)
            if (settingsManager.tempUnit === "F") wei *= 2.20462
        }

        // print data
        if (wei > 0) {
            dataIndicator.visible = true
            dataIndicator.text = wei.toFixed(1) + " " + (settingsManager.tempUnit === "F" ? "lb" : "kg")
            if (currentDevice.hasImpedance && imp > 0) {
                dataIndicator.text += " / " + imp.toFixed(0) + " Ω"
            }
        }
    }
}
