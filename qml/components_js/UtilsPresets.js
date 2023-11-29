// UtilsPresets.js
// Version 0

.import PresetUtils 1.0 as PresetUtils
.import ThemeEngine 1.0 as ThemeEngine

/* ************************************************************************** */

function getPresetIcon(presetType) {
    var src = ""

    if (presetType === PresetUtils.PresetUtils.PRESET_UNKNOWN) {
        src = "qrc:/assets/icons_fontawesome/question-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_MEAT) {
        src = "qrc:/assets/icons_fontawesome/cow-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FISH) {
        src = "qrc:/assets/icons_fontawesome/fish-fins-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_EGG) {
        src = "qrc:/assets/icons_fontawesome/egg-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_POULTRY) {
        src = "qrc:/assets/icons_fontawesome/kiwi-bird-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_SEAFOOD) {
        src = "qrc:/assets/icons_fontawesome/shrimp-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_VEGETABLE) {
        src = "qrc:/assets/icons_fontawesome/pepper-hot-solid.svg"
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FRUIT) {
        src = "qrc:/assets/icons_fontawesome/apple-whole-solid.svg"
    }

    return src
}

/* ************************************************************************** */

function getPresetName(presetType) {
    var name = ""

    if (presetType === PresetUtils.PresetUtils.PRESET_UNKNOWN) {
        name = qsTr("Unknown")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_MEAT) {
        name = qsTr("Meat")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FISH) {
        name = qsTr("Fish")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_EGG) {
        name = qsTr("Egg")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_POULTRY) {
        name = qsTr("Poultry")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_SEAFOOD) {
        name = qsTr("Seafood")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_VEGETABLE) {
        name = qsTr("Vegetable")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FRUIT) {
        name = qsTr("Fruit")
    }

    return name
}

/* ************************************************************************** */
