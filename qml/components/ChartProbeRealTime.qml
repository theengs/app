import QtQuick
import QtCharts

import ThemeEngine
import "qrc:/js/UtilsNumber.js" as UtilsNumber

Item {
    id: chartProbeRealTime
    anchors.fill: parent
    anchors.margins: 12

    property bool useOpenGL: false
    property bool showGraphDots: false
    property color legendColor: Theme.colorSubText

    property real valueMin: 0
    property real valueMax: 100

    ////////////////////////////////////////////////////////////////////////////

    Connections {
        target: currentDevice
        function onRtGraphUpdated() {
            //console.log("onRtgraphUpdated")
            updateGraph()
        }
        function onRtGraphCleaned() {
            //console.log("onRtgraphCleaned")
            updateGraph()
        }
    }

    function loadGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartProbeRealTime // loadGraph() >> " + currentDevice)

        temp1Data.visible = currentDevice.hasTemperature1
        temp2Data.visible = currentDevice.hasTemperature2
        temp3Data.visible = currentDevice.hasTemperature3
        temp4Data.visible = currentDevice.hasTemperature4
        temp5Data.visible = currentDevice.hasTemperature5
        temp6Data.visible = currentDevice.hasTemperature6

        //// DATA
        temp1Data.clear()
        temp2Data.clear()
        temp3Data.clear()
        temp4Data.clear()
        temp5Data.clear()
        temp6Data.clear()

        axisTemp.min = valueMin
        axisTemp.max = valueMax
        axisTempF.min = UtilsNumber.tempCelsiusToFahrenheit(valueMin)
        axisTempF.max = UtilsNumber.tempCelsiusToFahrenheit(valueMax)

        legendColor = Qt.rgba(legendColor.r, legendColor.g, legendColor.b, 0.8)
    }

    function updateGraph() {
        if (typeof currentDevice === "undefined" || !currentDevice) return
        //console.log("chartProbeRealTime // updateGraph() >> " + currentDevice)

        currentDevice.updateRtGraph(axisTime, currentDevice.realtimeWindow,
                                    temp1Data, temp2Data, temp3Data,
                                    temp4Data, temp5Data, temp6Data)
    }

    function isIndicator() { return false }
    function resetIndicator() { }

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

        ValueAxis { id: axisTemp; visible: (settingsManager.appUnits == 0); gridVisible: false;
                    labelFormat: "%i";
                    labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                    color: legendColor;
                    gridLineColor: Theme.colorSeparator; }
        ValueAxis { id: axisTempF; visible: (settingsManager.appUnits > 0); gridVisible: false;
                    labelFormat: "%i";
                    labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                    color: legendColor;
                    gridLineColor: Theme.colorSeparator; }

        DateTimeAxis { id: axisTime; visible: true; gridVisible: true;
                       labelsFont.pixelSize: Theme.fontSizeContentSmall-1; labelsColor: legendColor;
                       color: legendColor;
                       gridLineColor: Theme.colorSeparator; }

        LineSeries {
            id: fakefahrenheit
            axisY: axisTempF; axisX: axisTime;
        }

        LineSeries {
            id: temp1Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorMaterialBlue; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp2Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorMaterialDeepPurple; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp3Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorMaterialLightGreen; width: 2;
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
            color: Theme.colorOrange; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
        LineSeries {
            id: temp6Data
            useOpenGL: useOpenGL
            pointsVisible: showGraphDots
            color: Theme.colorRed; width: 2;
            axisY: axisTemp; axisX: axisTime;
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    Item {
        id: legend_area

        width: rtGraph.plotArea.width
        height: rtGraph.plotArea.height
        x: rtGraph.plotArea.x + rtGraph.anchors.leftMargin
        y: rtGraph.plotArea.y + rtGraph.anchors.topMargin
        z: -1

        clip: true

        // axisTemp.min // axisTemp.max
        // valueMin // valueMax

        Repeater {
            model: currentPreset && currentPreset.ranges

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right

                y: UtilsNumber.mapNumber(Math.min(modelData.tempMaxGraph, valueMax), // value
                                         valueMin, valueMax, // from
                                         rtGraph.plotArea.height, 0) // to
                height: ((Math.min(modelData.tempMaxGraph, valueMax) - modelData.tempMin) / (valueMax - valueMin)) * rtGraph.plotArea.height

                color: Qt.darker(Theme.colorRed, 1)
                opacity: 0.33 + ((index) * 0.1)
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}
