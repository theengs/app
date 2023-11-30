import QtQuick
import QtCharts

import ThemeEngine
import "qrc:/js/UtilsNumber.js" as UtilsNumber

Item {
    id: chartProbeDataAio
    anchors.fill: parent

    property bool useOpenGL: true
    property bool showGraphDots: settingsManager.graphShowDots
    property color legendColor: Theme.colorSubText

    ////////////////////////////////////////////////////////////////////////////

    function loadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartProbeDataAio // loadGraph() >> " + currentDevice)

        temp1Data.visible = currentDevice.hasTemperature1
        temp2Data.visible = currentDevice.hasTemperature2
        temp3Data.visible = currentDevice.hasTemperature3
        temp4Data.visible = currentDevice.hasTemperature4
        temp5Data.visible = currentDevice.hasTemperature5
        temp6Data.visible = currentDevice.hasTemperature6

        dateIndicator.visible = false
        dataIndicator.visible = false
        verticalIndicator.visible = false

        legendColor = Qt.rgba(legendColor.r, legendColor.g, legendColor.b, 0.8)
    }

    function updateGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartProbeDataAio // updateGraph() >> " + currentDevice)

        var days = 14
        var count = 0 // currentDevice.countDataNamed("temperature", days)

        //// DATA
        temp1Data.clear()
        temp2Data.clear()
        temp3Data.clear()
        temp4Data.clear()
        temp5Data.clear()
        temp6Data.clear()

        currentDevice.getChartData_probeAIO(days, axisTime,
                                            temp1Data, temp2Data,
                                            temp3Data, temp4Data,
                                            temp5Data, temp6Data)

        //// AXIS
        axisTemp.min = currentDevice.tempMin*0.85
        axisTemp.max = currentDevice.tempMax*1.15

        /// Graph visibility
        count = temp1Data.count
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

        ValueAxis { id: axisTemp; visible: false; gridVisible: false; }

        DateTimeAxis { id: axisTime; visible: true;
                       labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                       color: legendColor;
                       gridLineColor: Theme.colorSeparator; }

        LineSeries {
            id: temp1Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorBlue; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp2Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorRed; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp3Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorGreen; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp4Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorYellow; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp5Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorGreen; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp6Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorYellow; width: 2;
            axisY: axisTemp; axisX: axisTime;
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
            var mpmp = aioGraph.mapToValue(mmm, temp1Data)

            //console.log("clicked " + mouse.x + " " + mouse.y)
            //console.log("clicked adjusted " + ppp.x + " " + ppp.y)
            //console.log("clicked mapped " + mpmp.x + " " + mpmp.y)

            if (isMoving) {
                // dragging outside the graph area?
                if (mpmp.x < temp1Data.at(0).x){
                    ppp.x = aioGraph.mapToPosition(temp1Data.at(0), temp1Data).x + aioGraph.anchors.rightMargin
                    mpmp.x = temp1Data.at(0).x
                }
                if (mpmp.x > temp1Data.at(temp1Data.count-1).x){
                    ppp.x = aioGraph.mapToPosition(temp1Data.at(temp1Data.count-1), temp1Data).x + aioGraph.anchors.rightMargin
                    mpmp.x = temp1Data.at(temp1Data.count-1).x
                }
            } else {
                // did we clicked outside the graph area?
                if (mpmp.x < temp1Data.at(0).x || mpmp.x > temp1Data.at(temp1Data.count-1).x) {
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
        anchors.centerIn: parent
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
            else if (chartProbeDataAio.width - verticalIndicator.x > dateIndicator.width + 48)
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
        anchors.topMargin: isPhone ? 8 : 16
        //anchors.left: parent.left
        anchors.leftMargin: isPhone ? 8 : 16
        //anchors.right: parent.right
        anchors.rightMargin: isPhone ? 8 : 16
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: isPhone ? 8 : 16
        layoutDirection: "LeftToRight"
        columns: 1
        rows: 2

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

        Rectangle {
            id: dataIndicator

            width: isPhone ? chartProbeDataAio.width - 16 : dataIndicatorContent.width + 24
            height: 32

            radius: 4
            color: Theme.colorForeground
            border.width: Theme.componentBorderWidth
            border.color: Theme.colorSeparator

            Row {
                id: dataIndicatorContent
                anchors.centerIn: parent
                spacing: 8

                Rectangle { // #1
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20; height: 20; radius: 20;

                    visible: indicatorTop1.visible
                    color: temp1Data.color
                    Text {
                        anchors.centerIn: parent
                        text: "1"
                        color: "white"
                        font.pixelSize: 15
                        font.bold: true
                    }
                }
                Text {
                    id: indicatorTop1
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 15
                    font.bold: true
                    color: Theme.colorSubText
                }

                Rectangle { // #2
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20; height: 20; radius: 20;

                    visible: indicatorTop2.visible
                    color: temp2Data.color
                    Text {
                        anchors.centerIn: parent
                        text: "2"
                        color: "white"
                        font.pixelSize: 15
                        font.bold: true
                    }
                }
                Text {
                    id: indicatorTop2
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 15
                    font.bold: true
                    color: Theme.colorSubText
                }

                Rectangle { // #3
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20; height: 20; radius: 20;

                    visible: indicatorTop3.visible
                    color: temp3Data.color
                    Text {
                        anchors.centerIn: parent
                        text: "3"
                        color: "white"
                        font.pixelSize: 15
                        font.bold: true
                    }
                }
                Text {
                    id: indicatorTop3
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 15
                    font.bold: true
                    color: Theme.colorSubText
                }

                Rectangle { // #4
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20; height: 20; radius: 20;

                    visible: indicatorTop4.visible
                    color: temp4Data.color
                    Text {
                        anchors.centerIn: parent
                        text: "4"
                        color: "white"
                        font.pixelSize: 15
                        font.bold: true
                    }
                }
                Text {
                    id: indicatorTop4
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 15
                    font.bold: true
                    color: Theme.colorSubText
                }

                Rectangle { // #5
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20; height: 20; radius: 20;

                    visible: indicatorTop5.visible
                    color: temp5Data.color
                    Text {
                        anchors.centerIn: parent
                        text: "5"
                        color: "white"
                        font.pixelSize: 15
                        font.bold: true
                    }
                }
                Text {
                    id: indicatorTop5
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 15
                    font.bold: true
                    color: Theme.colorSubText
                }

                Rectangle { // #6
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20; height: 20; radius: 20;

                    visible: indicatorTop6.visible
                    color: temp6Data.color
                    Text {
                        anchors.centerIn: parent
                        text: "6"
                        color: "white"
                        font.pixelSize: 15
                        font.bold: true
                    }
                }
                Text {
                    id: indicatorTop6
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 15
                    font.bold: true
                    color: Theme.colorSubText
                }
            }
        }

        Rectangle {
            id: dateIndicator
            //anchors.horizontalCenter: parent.horizontalCenter

            //Layout.alignment: Qt.AlignHCenter

            width: dateIndicatorContent.width + 24
            height: 32

            radius: 4
            color: Theme.colorForeground
            border.width: Theme.componentBorderWidth
            border.color: Theme.colorSeparator

            Text {
                id: dateIndicatorContent
                anchors.centerIn: parent

                font.pixelSize: 15
                font.bold: true
                color: Theme.colorSubText
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

        var t1 = -99
        var t2 = -99
        var t3 = -99
        var t4 = -99
        var t5 = -99
        var t6 = -99

        // set date & time
        var date = new Date(verticalIndicator.clickedCoordinates.x)
        var date_string = date.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
        //: "at" is used for DATE at HOUR
        var time_string = qsTr("at") + " " + UtilsNumber.padNumber(date.getHours(), 2) + ":" + UtilsNumber.padNumber(date.getMinutes(), 2)
        dateIndicatorContent.text = date_string + " " + time_string

        // search index corresponding to the timestamp
        var x1 = -1
        var x2 = -1
        for (var i = 0; i < temp1Data.count; i++) {
            var graph_at_x = temp1Data.at(i).x
            var dist = (graph_at_x - verticalIndicator.clickedCoordinates.x) / 1000000

            if (dist <= 0) {
                if (x1 == 0) x2 = i
                else if (x1 < i) x1 = i
            } else {
                x2 = i
                break
            }
        }
        if (x1 >= 0 && x2 > x1) {
            // linear interpolation
            if (temp1Data.at(x1).y > -40) t1 = qpoint_lerp(temp1Data.at(x1), temp1Data.at(x2), verticalIndicator.clickedCoordinates.x)
            if (temp2Data.at(x1).y > -40) t2 = qpoint_lerp(temp2Data.at(x1), temp2Data.at(x2), verticalIndicator.clickedCoordinates.x)
            if (temp3Data.at(x1).y > -40) t3 = qpoint_lerp(temp3Data.at(x1), temp3Data.at(x2), verticalIndicator.clickedCoordinates.x)
            if (temp4Data.at(x1).y > -40) t4 = qpoint_lerp(temp4Data.at(x1), temp4Data.at(x2), verticalIndicator.clickedCoordinates.x)
            if (temp5Data.at(x1).y > -40) t5 = qpoint_lerp(temp5Data.at(x1), temp5Data.at(x2), verticalIndicator.clickedCoordinates.x)
            if (temp6Data.at(x1).y > -40) t6 = qpoint_lerp(temp6Data.at(x1), temp6Data.at(x2), verticalIndicator.clickedCoordinates.x)
        }

        // print data
        dataIndicator.visible = true
        indicatorTop1.visible = (t1 > -40)
        indicatorTop1.text = t1.toFixed(0) + "°"
        indicatorTop2.visible = (t2 > -40)
        indicatorTop2.text = t2.toFixed(0) + "°"
        indicatorTop3.visible = (t3 > -40)
        indicatorTop3.text = t3.toFixed(0) + "°"
        indicatorTop4.visible = (t4 > -40)
        indicatorTop4.text = t4.toFixed(0) + "°"
        indicatorTop5.visible = (t5 > -40)
        indicatorTop5.text = t5.toFixed(0) + "°"
        indicatorTop6.visible = (t6 > -40)
        indicatorTop6.text = t6.toFixed(0) + "°"
    }

    ////////////////////////////////////////////////////////////////////////////
}
