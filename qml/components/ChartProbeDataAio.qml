import QtQuick 2.15
import QtCharts 2.15

import ThemeEngine 1.0
import "qrc:/js/UtilsNumber.js" as UtilsNumber

Item {
    id: chartProbeDataAio
    anchors.fill: parent

    property bool useOpenGL: true
    property bool showGraphDots: settingsManager.graphShowDots
    property color legendColor: Theme.colorSubText

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
        var count = 20 // currentDevice.countDataNamed("temperature", days)

        showGraphDots = (settingsManager.graphShowDots && count < 16)

        if (count > 1) {
            aioGraph.visible = true
            noDataIndicator.visible = false
        } else {
            aioGraph.visible = false
            noDataIndicator.visible = true
        }

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
                                            temp5Data, temp6Data);

        //// AXIS
        axisTemp.min = 0
        axisTemp.max = 100

        // Max axis for temperature
        axisTemp.max = currentDevice.tempMax*1.15;
        axisTemp.min = currentDevice.tempMin*0.85;

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

        // set date & time
        var date = new Date(verticalIndicator.clickedCoordinates.x)
        var date_string = date.toLocaleDateString()
        //: "at" is used for DATE at HOUR
        var time_string = qsTr("at") + " " + UtilsNumber.padNumber(date.getHours(), 2) + ":" + UtilsNumber.padNumber(date.getMinutes(), 2)
        dateIndicator.text = date_string + " " + time_string

        // search index corresponding to the timestamp
        var x1 = -1
        var x2 = -1
        for (var i = 0; i < temp1Data.count; i++) {
            var graph_at_x = temp1Data.at(i).x
            var dist = (graph_at_x - verticalIndicator.clickedCoordinates.x) / 1000000

            if (Math.abs(dist) < 1) {
/*
                // nearest neighbor
                if (appContent.state === "DevicePlantSensor") {
                    dataIndicators.updateDataBars(hygroData.at(i).y, conduData.at(i).y, -99,
                                                  temp1Data.at(i).y, -99, lumiData.at(i).y)
                } else if (appContent.state === "DeviceThermometer") {
                    dataIndicator.visible = true
                    dataIndicator.text = (settingsManager.tempUnit === "F") ? UtilsNumber.tempCelsiusToFahrenheit(temp1Data.at(i).y).toFixed(1) + qsTr("°F") : temp1Data.at(i).y.toFixed(1) + qsTr("°C")
                    dataIndicator.text += " " + hygroData.at(i).y.toFixed(0) + "%"
                }
*/
                break;
            } else {
                if (dist < 0) {
                    if (x1 < i) x1 = i
                } else {
                    x2 = i
                    break
                }
            }
        }

        if (x1 >= 0 && x2 > x1) {
            // linear interpolation
/*
            if (appContent.state === "DevicePlantSensor") {
                dataIndicators.updateDataBars(qpoint_lerp(hygroData.at(x1), hygroData.at(x2), verticalIndicator.clickedCoordinates.x),
                                              qpoint_lerp(conduData.at(x1), conduData.at(x2), verticalIndicator.clickedCoordinates.x),
                                              -99,
                                              qpoint_lerp(temp1Data.at(x1), temp1Data.at(x2), verticalIndicator.clickedCoordinates.x),
                                              -99,
                                              qpoint_lerp(lumiData.at(x1), lumiData.at(x2), verticalIndicator.clickedCoordinates.x))
            } else if (appContent.state === "DeviceThermometer") {
                dataIndicator.visible = true
                var temmp = qpoint_lerp(temp1Data.at(x1), temp1Data.at(x2), verticalIndicator.clickedCoordinates.x)
                dataIndicator.text = (settingsManager.tempUnit === "F") ? UtilsNumber.tempCelsiusToFahrenheit(temmp).toFixed(1) + "°F" : temmp.toFixed(1) + "°C"
                dataIndicator.text += " " + qpoint_lerp(hygroData.at(x1), hygroData.at(x2), verticalIndicator.clickedCoordinates.x).toFixed(0) + "%"
            }
*/
        }
    }
}
