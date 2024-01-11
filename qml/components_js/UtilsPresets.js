// UtilsPresets.js
// Version 0

.import PresetUtils 1.0 as PresetUtils
.import ThemeEngine 1.0 as ThemeEngine

/* ************************************************************************** */

function getPresetIcon(presetType) {
    var src = ""

    if (presetType === PresetUtils.PresetUtils.PRESET_UNKNOWN) {
        src = "qrc:/assets/icons_fontawesome/question-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_BEEF) {
        src = "qrc:/assets/icons_fontawesome/cow-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_PORK) {
        src = "qrc:/assets/icons_fontawesome/piggy-bank-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_CHICKEN) {
        src = "qrc:/assets/icons_fontawesome/kiwi-bird-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FISH) {
        src = "qrc:/assets/icons_fontawesome/fish-fins-solid.svg"
    }

    return src
}

/* ************************************************************************** */

function getPresetType(presetType) {
    var type = ""

    if (presetType === PresetUtils.PresetUtils.PRESET_UNKNOWN) {
        type = qsTr("Unknown")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_BEEF) {
        type = qsTr("Beef")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_PORK) {
        type = qsTr("Pork")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_CHICKEN) {
        type = qsTr("Chicken")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FISH) {
        type = qsTr("Fish")
    }

    return type
}

/* ************************************************************************** */
