import QtQuick
import QtCharts

import ComponentLibrary

Item {
    id: chartBatteryHistory
    anchors.fill: parent
    anchors.margins: 12

    property bool useOpenGL: true
    property bool showGraphDots: false
    property color legendColor: Theme.colorSubText

    property real valueMin_p: 0
    property real valueMax_p: 100

    property real valueMin_v: 0
    property real valueMax_v: 15
    property real limitMin_v: 0
    property real limitMax_v: 0

    property int maxDays: 30

    ////////////////////////////////////////////////////////////////////////////

    function loadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartBatteryHistory // loadGraph() >> " + currentDevice)

        //// DATA
        batteryData_p.clear()
        batteryData_v.clear()

        batteryData_p.visible = true
        batteryData_v.visible = true

        //// AXIS
        axisPercents.visible = false
        axisPercents.min = 0
        axisPercents.max = 100

        axisVolts.visible = true
        axisVolts.min = valueMin_v
        axisVolts.max = valueMax_v

        //// LEGEND
        chartBatteryHistory.limitMin_v = (currentPreset) ? currentPreset.voltageMin : 0
        chartBatteryHistory.limitMax_v = (currentPreset) ? currentPreset.voltageMax : 0
        chartBatteryHistory.legendColor = Qt.rgba(legendColor.r, legendColor.g, legendColor.b, 0.8)
    }

    function updateGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartBatteryHistory // updateGraph() >> " + currentDevice)

        var days = chartBatteryHistory.maxDays
        var count = 0 // currentDevice.countDataNamed("battery2", days)

        // update
        currentDevice.getChartData_batteryHistory(axisTime, batteryData_p, batteryData_v, false, days)
        count = batteryData_v.count

        // series
        axisPercents.visible = (count <= 0)
        batteryData_p.visible = (count <= 0)
        axisVolts.visible = (count > 0)
        batteryData_v.visible = (count > 0)

        /// graph visibility
        historyGraph.visible = (count > 1)
        noDataIndicator.visible = (count <= 0)
        //showGraphDots = (settingsManager.graphShowDots && count < 16)
    }

    function updatePreset() {
        if (typeof currentDevice === "undefined" || !currentDevice) return

        if (currentPreset) {
            //console.log("DeviceBatteryMonitor // onPresetUpdated() >> " + currentPreset.name)
            chartBatteryHistory.limitMin_v = currentPreset.voltageMin
            chartBatteryHistory.limitMax_v = currentPreset.voltageMax
        } else {
            //console.log("DeviceBatteryMonitor // onPresetUpdated() >> empty preset")
            chartBatteryHistory.limitMin_v = 0
            chartBatteryHistory.limitMax_v = 0
        }
    }

    function updateMaxDays(maxDays) {
        if (chartBatteryHistory.maxDays !== maxDays) {
            chartBatteryHistory.maxDays = maxDays
            chartBatteryHistory.updateGraph()
        }
    }

    function isIndicator() { return false }
    function resetIndicator() { }

    ////////////////////////////////////////////////////////////////////////////

    ChartView {
        id: historyGraph

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

        ValueAxis { id: axisPercents; visible: false; gridVisible: false;
                    min: 0; max: 100;
                    labelFormat: "%i";
                    labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                    color: legendColor;
                    gridLineColor: Theme.colorSeparator; }

        ValueAxis { id: axisVolts; visible: true; gridVisible: false;
                    labelFormat: "%i";
                    labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                    min: 0; max: 20; }

        DateTimeAxis { id: axisTime; visible: true; gridVisible: true;
                       labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                       color: legendColor;
                       gridLineColor: Theme.colorSeparator; }

        LineSeries {
            id: batteryData_p
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorBlue; width: 2;
            axisY: axisPercents; axisX: axisTime;
        }
        LineSeries {
            id: batteryData_v
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorGreen; width: 2;
            axisY: axisVolts; axisX: axisTime;
        }

        MouseArea {
            id: clickableGraphArea
            anchors.fill: historyGraph

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: (mouse) => {
                if (mouse.button === Qt.LeftButton) {
                    historyGraph.moveIndicator(mouse, false)
                    mouse.accepted = true
                }
                else if (mouse.button === Qt.RightButton) {
                    resetIndicator()
                }
            }
        }

        function moveIndicator(mouse, isMoving) {
            //
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Item {
        id: legend_area

        width: historyGraph.plotArea.width
        height: historyGraph.plotArea.height
        x: historyGraph.plotArea.x + historyGraph.anchors.leftMargin
        y: historyGraph.plotArea.y + historyGraph.anchors.topMargin

        visible: historyGraph.visible
        clip: true

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right

            y: UtilsNumber.mapNumber(Math.min(limitMax_v, valueMax_v), // value
                                     valueMin_p, valueMax_v, // from
                                     historyGraph.plotArea.height, 0) // to
            height: ((Math.min(limitMax_v, valueMax_v) - limitMin_v) / (valueMax_v - valueMin_v)) * historyGraph.plotArea.height

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
