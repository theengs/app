import QtQuick
import QtCharts

import ComponentLibrary
import "qrc:/ComponentLibrary/UtilsNumber.js" as UtilsNumber

Item {
    id: chartBatteryHistory
    anchors.fill: parent
    anchors.margins: 12

    property bool useOpenGL: true
    property bool showGraphDots: settingsManager.graphShowDots
    property color legendColor: Theme.colorSubText

    property real limitMin: 0
    property real limitMax: 0

    property real valueMin: 0
    property real valueMax: 20

    ////////////////////////////////////////////////////////////////////////////

    function loadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartBatteryHistory // loadGraph() >> " + currentDevice)

        battery2Data.visible = true // currentDevice.hasBatteryVoltage
        battery2Data.clear()

        axisPercents.min = 0
        axisPercents.max = 100
        axisVolts.min = valueMin
        axisVolts.max = valueMax

        chartBatteryHistory.limitMin = (currentPreset) ? currentPreset.voltageMin : 0
        chartBatteryHistory.limitMax = (currentPreset) ? currentPreset.voltageMax : 0
        chartBatteryHistory.legendColor = Qt.rgba(legendColor.r, legendColor.g, legendColor.b, 0.8)
    }

    function updateGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartBatteryHistory // updateGraph() >> " + currentDevice)

        var days = 30
        var count = 0 // currentDevice.countDataNamed("battery2", days)

        //// DATA
        battery2Data.clear()

        currentDevice.getChartData_batteryHistory(axisTime, battery2Data, false, days)

        //// AXIS
        //axisVolts.min = currentDevice.voltMin*0.85
        //axisVolts.max = currentDevice.voltMax*1.15

        /// Graph visibility
        count = battery2Data.count
        aioGraph.visible = (count > 1)
        noDataIndicator.visible = (count <= 0)
        showGraphDots = (settingsManager.graphShowDots && count < 16)
    }

    function updatePreset() {
        if (typeof currentDevice === "undefined" || !currentDevice) return

        if (currentPreset) {
            //console.log("DeviceBatteryMonitor // onPresetUpdated() >> " + currentPreset.name)
            chartBatteryHistory.limitMin = currentPreset.voltageMin
            chartBatteryHistory.limitMax = currentPreset.voltageMax
        } else {
            //console.log("DeviceBatteryMonitor // onPresetUpdated() >> empty preset")
            chartBatteryHistory.limitMin = 0
            chartBatteryHistory.limitMax = 0
        }
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

        ValueAxis { id: axisPercents; visible: true; gridVisible: false; }
        ValueAxis { id: axisVolts; visible: true; gridVisible: false; }

        DateTimeAxis { id: axisTime; visible: true;
                       labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                       color: legendColor;
                       gridLineColor: Theme.colorSeparator; }

        LineSeries {
            id: battery2Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorBlue; width: 2;
            axisY: axisVolts; axisX: axisTime;
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
/*
            // we adjust coordinates with graph area margins
            var ppp = Qt.point(mouse.x, mouse.y)
            ppp.x = ppp.x + aioGraph.anchors.rightMargin
            ppp.y = ppp.y - aioGraph.anchors.topMargin

            // map mouse position to graph value // mpmp.x is the timestamp
            var mpmp = aioGraph.mapToValue(mmm, batteryData)

            //console.log("clicked " + mouse.x + " " + mouse.y)
            //console.log("clicked adjusted " + ppp.x + " " + ppp.y)
            //console.log("clicked mapped " + mpmp.x + " " + mpmp.y)

            if (isMoving) {
                // dragging outside the graph area?
                if (mpmp.x < batteryData.at(0).x){
                    ppp.x = aioGraph.mapToPosition(batteryData.at(0), batteryData).x + aioGraph.anchors.rightMargin
                    mpmp.x = batteryData.at(0).x
                }
                if (mpmp.x > batteryData.at(batteryData.count-1).x){
                    ppp.x = aioGraph.mapToPosition(batteryData.at(batteryData.count-1), batteryData).x + aioGraph.anchors.rightMargin
                    mpmp.x = batteryData.at(batteryData.count-1).x
                }
            } else {
                // did we clicked outside the graph area?
                if (mpmp.x < batteryData.at(0).x || mpmp.x > batteryData.at(batteryData.count-1).x) {
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
*/
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Item {
        id: legend_area

        width: aioGraph.plotArea.width
        height: aioGraph.plotArea.height
        x: aioGraph.plotArea.x + aioGraph.anchors.leftMargin
        y: aioGraph.plotArea.y + aioGraph.anchors.topMargin

        visible: aioGraph.visible
        clip: true

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right

            y: UtilsNumber.mapNumber(Math.min(limitMax, valueMax), // value
                                     valueMin, valueMax, // from
                                     aioGraph.plotArea.height, 0) // to
            height: ((Math.min(limitMax, valueMax) - limitMin) / (valueMax - valueMin)) * aioGraph.plotArea.height

            color: Theme.colorGreen
            opacity: 0.20
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    ItemNoData {
        id: noDataIndicator
        anchors.centerIn: parent
    }

    ////////////////////////////////////////////////////////////////////////////
}
