import QtQuick
import QtQuick.Controls

import ComponentLibrary

// Small footer caption shown under in-app history charts. Reminds the user
// that what the chart shows is sampled at the per-category interval from
// Settings, so the HA/broker timeseries (every advert) may differ. Tapping
// the text takes them to Settings.
//
// Required property: intervalMin (uint, minutes). Pass the relevant
// settingsManager.updateInterval{Thermo,Plant,Env} from the parent screen.

Item {
    id: itemSamplingCaption

    property int intervalMin: 0

    implicitHeight: visible ? captionText.implicitHeight + 8 : 0
    visible: (intervalMin > 0)

    Text {
        id: captionText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        textFormat: Text.PlainText
        color: Theme.colorSubText
        opacity: 0.75
        font.pixelSize: Theme.fontSizeContentVerySmall
        font.italic: true
        horizontalAlignment: Text.AlignHCenter

        text: qsTr("1 sample / %1 min \u00b7 change in Settings").arg(intervalMin)
    }

    // No MouseArea / "tap to open Settings" wiring yet — the screen-routing
    // pattern differs between Desktop and Mobile entry points, and a passive
    // caption beats wiring a half-working tap target.
}
