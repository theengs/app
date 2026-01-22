import QtQuick
import QtCharts

import ComponentLibrary

Item {
    id: chartBatteryRealTime
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

    ////////////////////////////////////////////////////////////////////////////

    function loadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartBatteryRealTime // loadGraph() >> " + currentDevice)

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
        chartBatteryRealTime.limitMin_v = (currentPreset) ? currentPreset.voltageMin : 0
        chartBatteryRealTime.limitMax_v = (currentPreset) ? currentPreset.voltageMax : 0
        chartBatteryRealTime.legendColor = Qt.rgba(legendColor.r, legendColor.g, legendColor.b, 0.8)
    }

    function reloadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        if (appContent.state !== "DeviceBatteryMonitor") return
        //console.log("chartBatteryRealTime // reloadGraph() >> " + currentDevice)

        currentDevice.getChartData_batteryRT(axisTime, batteryData_p, batteryData_v, true)
    }

    function updateGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        if (appContent.state !== "DeviceBatteryMonitor") return
        //console.log("chartBatteryRealTime // updateGraph() >> " + currentDevice)

        // update
        currentDevice.getChartData_batteryRT(axisTime, batteryData_p, batteryData_v, false)

        // Graph visibility
        batteryData_p.visible = !batteryData_v.count
        batteryData_v.visible = batteryData_v.count
    }

    function updatePreset() {
        if (typeof currentDevice === "undefined" || !currentDevice) return

        if (currentPreset) {
            console.log("DeviceBatteryMonitor // onPresetUpdated() >> " + currentPreset.name)
            chartBatteryRealTime.limitMin_v = currentPreset.voltageMin
            chartBatteryRealTime.limitMax_v = currentPreset.voltageMax
        } else {
            console.log("DeviceBatteryMonitor // onPresetUpdated() >> empty preset")
            chartBatteryRealTime.limitMin_v = 0
            chartBatteryRealTime.limitMax_v = 0
        }
    }

    function updateMaxDays(maxDays) {
        //
    }

    function isIndicator() { return false }
    function resetIndicator() { }

    ////////////////////////////////////////////////////////////////////////////

    Connections {
        target: currentDevice

        function onRtGraphUpdated() {
            //console.log("onRtGraphUpdated")
            updateGraph()
        }
        function onRtGraphCleaned() {
            //console.log("onRtGraphCleaned")
            updateGraph()
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    ChartView {
        id: rtGraph

        anchors.fill: parent
        anchors.topMargin: -40
        anchors.leftMargin: -32
        anchors.rightMargin: -32
        anchors.bottomMargin: -40

        antialiasing: true
        legend.visible: false
        backgroundRoundness: 0
        backgroundColor: "transparent"
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
            color: Theme.colorMaterialBlue; width: 2;
            axisY: axisPercents; axisX: axisTime;
        }
        LineSeries {
            id: batteryData_v
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorGreen; width: 2;
            axisY: axisVolts; axisX: axisTime;
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Item {
        id: legend_area

        width: rtGraph.plotArea.width
        height: rtGraph.plotArea.height
        x: rtGraph.plotArea.x + rtGraph.anchors.leftMargin
        y: rtGraph.plotArea.y + rtGraph.anchors.topMargin

        visible: rtGraph.visible
        clip: true

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right

            y: UtilsNumber.mapNumber(Math.min(limitMax_v, valueMax_v), // value
                                     valueMin_p, valueMax_v, // from
                                     rtGraph.plotArea.height, 0) // to
            height: ((Math.min(limitMax_v, valueMax_v) - limitMin_v) / (valueMax_v - valueMin_v)) * rtGraph.plotArea.height

            color: Theme.colorGreen
            opacity: 0.20
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
