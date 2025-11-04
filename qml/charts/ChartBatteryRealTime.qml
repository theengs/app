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

    property real valueMin: 0
    property real valueMax: 100

    ////////////////////////////////////////////////////////////////////////////

    function loadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartBatteryRealTime // loadGraph() >> " + currentDevice)

        //// DATA
        battery1Data.clear()

        legendColor = Qt.rgba(legendColor.r, legendColor.g, legendColor.b, 0.8)
    }

    function reloadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        if (appContent.state !== "DeviceBatteryMonitor") return
        //console.log("chartBatteryRealTime // reloadGraph() >> " + currentDevice)

        currentDevice.getChartData_batteryRT(axisTime, battery1Data, true)
    }

    function updateGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        if (appContent.state !== "DeviceBatteryMonitor") return
        //console.log("chartBatteryRealTime // updateGraph() >> " + currentDevice)

        // update
        currentDevice.getChartData_batteryRT(axisTime, battery1Data, false)
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

        ValueAxis { id: axisPercent; visible: true; gridVisible: false;
                    min: 0; max: 100;
                    labelFormat: "%i";
                    labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                    color: legendColor;
                    gridLineColor: Theme.colorSeparator; }

        DateTimeAxis { id: axisTime; visible: true; gridVisible: true;
                       labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                       color: legendColor;
                       gridLineColor: Theme.colorSeparator; }

        LineSeries {
            id: battery1Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorMaterialBlue; width: 2;
            axisY: axisPercent; axisX: axisTime;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
