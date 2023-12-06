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

function getPresetType(presetType) {
    var type = ""

    if (presetType === PresetUtils.PresetUtils.PRESET_UNKNOWN) {
        type = qsTr("Unknown")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_MEAT) {
        type = qsTr("Meat")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FISH) {
        type = qsTr("Fish")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_EGG) {
        type = qsTr("Egg")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_POULTRY) {
        type = qsTr("Poultry")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_SEAFOOD) {
        type = qsTr("Seafood")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_VEGETABLE) {
        type = qsTr("Vegetable")
    } else if (presetType === PresetUtils.PresetUtils.PRESET_FRUIT) {
        type = qsTr("Fruit")
    }

    return type
}

/* ************************************************************************** */
