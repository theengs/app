import QtQuick
import QtQuick.Controls

import ComponentLibrary

// Small footer caption shown under in-app history charts. Reminds the user
// that what the chart shows is sampled at the per-category interval, which
// can diverge from the broker timeseries (every advert).
//
// Required property: intervalMin (uint, minutes). Pass the relevant
// settingsManager.updateInterval{Thermo,Plant,Env} from the parent screen.
//
// NB: the interval setters exist in C++ (SettingsManager::setUpdateInterval*)
// but no QML Settings control is wired to them today, so the caption is
// purely informational — no "change in Settings" cue, which would mislead.

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

        text: qsTr("1 sample / %1 min").arg(intervalMin)
    }
}
