// UtilsPresets.js
// Version 0

.pragma library

.import BatteryPresetUtils as BatteryPresetUtils
.import TempPresetUtils as TempPresetUtils
.import ComponentLibrary as ThemeEngine

/* ************************************************************************** */

function getBatteryPresetIcon(presetType) {
    var src = ""

    if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_UNKNOWN) {
        src = "qrc:/assets/icons_custom/battery_car.svg"
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_LEADACID) {
        src = "qrc:/assets/icons_custom/battery_car.svg"
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_AGM) {
        src = "qrc:/assets/icons_custom/battery_car.svg"
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_EFB) {
        src = "qrc:/assets/icons_custom/battery_car.svg"
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_GEL) {
        src = "qrc:/assets/icons_custom/battery_car.svg"
    }

    return src
}

/* ************************************************************************** */

function getBatteryPresetType(presetType) {
    var type = ""

    if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_UNKNOWN) {
        type = qsTr("Unknown")
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_LEADACID) {
        type = qsTr("Lead Acid")
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_AGM) {
        type = qsTr("Absorbent Glass Mat")
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_EFB) {
        type = qsTr("Enhanced Flooded Battery")
    } else if (presetType === BatteryPresetUtils.BatteryPresetUtils.PRESET_GEL) {
        type = qsTr("Gel")
    }

    return type
}

/* ************************************************************************** */
/* ************************************************************************** */

function getPresetIcon(presetType) {
    var src = ""

    if (presetType === TempPresetUtils.TempPresetUtils.PRESET_UNKNOWN) {
        src = "qrc:/assets/icons_fontawesome/question-solid.svg"
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_BEEF) {
        src = "qrc:/assets/icons_fontawesome/cow-solid.svg"
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_PORK) {
        src = "qrc:/assets/icons_fontawesome/piggy-bank-solid.svg"
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_CHICKEN) {
        src = "qrc:/assets/icons_fontawesome/kiwi-bird-solid.svg"
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_FISH) {
        src = "qrc:/assets/icons_fontawesome/fish-fins-solid.svg"
    }

    return src
}

/* ************************************************************************** */

function getPresetType(presetType) {
    var type = ""

    if (presetType === TempPresetUtils.TempPresetUtils.PRESET_UNKNOWN) {
        type = qsTr("Unknown")
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_BEEF) {
        type = qsTr("Beef")
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_PORK) {
        type = qsTr("Pork")
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_CHICKEN) {
        type = qsTr("Chicken")
    } else if (presetType === TempPresetUtils.TempPresetUtils.PRESET_FISH) {
        type = qsTr("Fish")
    }

    return type
}

/* ************************************************************************** */
