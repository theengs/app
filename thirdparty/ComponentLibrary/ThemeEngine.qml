pragma Singleton

import QtQuick
import QtQuick.Controls.Material

import ComponentLibrary

Item {
    enum ThemeNames {

        THEME_THEENGS = 0,
        THEME_SNOW = 1,
        THEME_NIGHT = 2,

        THEME_LAST
    }
    property int currentTheme: -1

    ////////////////

    property bool isHdpi: (utilsScreen.screenDpi >= 128 || utilsScreen.screenPar >= 2.0)
    property bool isDesktop: (Qt.platform.os !== "ios" && Qt.platform.os !== "android")
    property bool isMobile: (Qt.platform.os === "ios" || Qt.platform.os === "android")
    property bool isPhone: ((Qt.platform.os === "ios" || Qt.platform.os === "android") && (utilsScreen.screenSize < 7.0))
    property bool isTablet: ((Qt.platform.os === "ios" || Qt.platform.os === "android") && (utilsScreen.screenSize >= 7.0))

    ////////////////

    property bool isLight
    property bool isDark

    // Status bar (mobile)
    property int themeStatusbar
    property color colorStatusbar

    // Header
    property color colorHeader
    property color colorHeaderContent
    property color colorHeaderHighlight

    // Action bar (mobile)
    property color colorActionbar
    property color colorActionbarContent
    property color colorActionbarHighlight

    // Side bar (desktop)
    property color colorSidebar
    property color colorSidebarContent
    property color colorSidebarHighlight

    // Tablet bar (mobile)
    property color colorTabletmenu
    property color colorTabletmenuContent
    property color colorTabletmenuHighlight

    // Content
    property color colorBackground
    property color colorForeground

    property color colorPrimary
    property color colorSecondary
    property color colorSuccess
    property color colorWarning
    property color colorError

    property color colorText
    property color colorSubText
    property color colorIcon
    property color colorSeparator

    property color colorLowContrast
    property color colorHighContrast

    ////////////////

    // App specific (Theengs)
    property color colorDeviceWidget

    ////////////////

    // Palette colors
    property color colorRed: "#ff7657"
    property color colorGreen: "#8cd200"
    property color colorBlue: "#4cafe9"
    property color colorPurple: "#b563ff"
    property color colorYellow: "#ffcf00"
    property color colorOrange: "#ffa635"
    property color colorGrey: "#666"

    // Material colors
    readonly property color colorMaterialRed: "#F44336"
    readonly property color colorMaterialPink: "#E91E63"
    readonly property color colorMaterialPurple: "#9C27B0"
    readonly property color colorMaterialDeepPurple: "#673AB7"
    readonly property color colorMaterialIndigo: "#3F51B5"
    readonly property color colorMaterialBlue: "#2196F3"
    readonly property color colorMaterialLightBlue: "#03A9F4"
    readonly property color colorMaterialCyan: "#00BCD4"
    readonly property color colorMaterialTeal: "#009688"
    readonly property color colorMaterialGreen: "#4CAF50"
    readonly property color colorMaterialLightGreen: "#8BC34A"
    readonly property color colorMaterialLime: "#CDDC39"
    readonly property color colorMaterialYellow: "#FFEB3B"
    readonly property color colorMaterialAmber: "#FFC107"
    readonly property color colorMaterialOrange: "#FF9800"
    readonly property color colorMaterialDeepOrange: "#FF5722"
    readonly property color colorMaterialBrown: "#795548"
    readonly property color colorMaterialGrey: "#9E9E9E"
    readonly property color colorMaterialLightGrey: "#f8f8f8"
    readonly property color colorMaterialDarkGrey: "#ececec"

    ////////////////

    // Qt Quick Controls & theming
    property color colorComponent
    property color colorComponentText
    property color colorComponentContent
    property color colorComponentBorder
    property color colorComponentDown
    property color colorComponentBackground
    property color colorComponentShadow: isLight ? "#40000000" : "#88000000"

    property int componentRadius: 4
    property int componentBorderWidth: 2

    property int componentFontSize: isHdpi ? 14 : 15

    property int componentMarginXS: isHdpi ? 4 : 8
    property int componentMarginS: isHdpi ? 8 : 12
    property int componentMargin: isHdpi ? 12 : 16
    property int componentMarginL: isHdpi ? 16 : 20
    property int componentMarginXL: isHdpi ? 20 : 24

    property int componentHeight: {
        if (isDesktop && isHdpi) return 34
        if (isDesktop) return 38
        return 40
    }
    property int componentHeightL: {
        if (isDesktop && isHdpi) return 40
        if (isDesktop) return 44
        return 48
    }
    property int componentHeightXL: {
        if (isDesktop && isHdpi) return 44
        if (isDesktop) return 48
        return 52
    }

    ////////////////

    // Fonts (sizes in pixel)
    readonly property int fontSizeHeader: isMobile ? 22 : 26
    readonly property int fontSizeTitle: isMobile ? 24 : 28
    readonly property int fontSizeContentVeryVerySmall: 10
    readonly property int fontSizeContentVerySmall: 12
    readonly property int fontSizeContentSmall: 14
    readonly property int fontSizeContent: 16
    readonly property int fontSizeContentBig: 18
    readonly property int fontSizeContentVeryBig: 20
    readonly property int fontSizeContentVeryVeryBig: 22

    ////////////////////////////////////////////////////////////////////////////

    function getThemeIndex(name) {
        if (name === "THEME_THEENGS") return Theme.THEME_THEENGS
        if (name === "THEME_SNOW") return Theme.THEME_SNOW
        if (name === "THEME_NIGHT") return Theme.THEME_NIGHT
        return -1
    }
    function getThemeName(index) {
        if (index === Theme.THEME_THEENGS) return "THEME_THEENGS"
        if (index === Theme.THEME_SNOW) return "THEME_SNOW"
        if (index === Theme.THEME_NIGHT) return "THEME_NIGHT"
        return ""
    }

    ////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: loadTheme(settingsManager.appTheme)
    Connections {
        target: settingsManager
        function onAppThemeChanged() { loadTheme(settingsManager.appTheme) }
    }

    function loadTheme(newIndex) {
        //console.log("Theme.loadTheme(" + newIndex + ")")
        var themeIndex = -1

        // Get the theme index
        if ((typeof newIndex === 'string' || newIndex instanceof String)) {
            themeIndex = getThemeIndex(newIndex)
        } else {
            themeIndex = newIndex
        }

        // Validate the result (or set the default)
        if (themeIndex < 0 || themeIndex >= Theme.THEME_LAST) {
            themeIndex = Theme.THEME_THEENGS // default theme
        }

        // Handle day/night themes
        if (settingsManager.appThemeAuto) {
            var rightnow = new Date()
            var hour = Qt.formatDateTime(rightnow, "hh")
            if (hour >= 21 || hour <= 8) {
                themeIndex = Theme.THEME_NIGHT
            }
        }

        // Do not reload the same theme
        if (themeIndex === currentTheme) return

        // Set the theme
        if (themeIndex === Theme.THEME_SNOW) {

            colorGreen = "#85c700"
            colorBlue = "#4cafe9"
            colorYellow = "#facb00"
            colorOrange = "#ffa635"
            colorRed = "#ff7657"

            isLight = true
            isDark = false

            themeStatusbar = Material.Light
            colorStatusbar = "white"

            colorHeader = "white"
            colorHeaderContent = "#444"
            colorHeaderHighlight = "#ececec"

            colorSidebar = "white"
            colorSidebarContent = "#444"
            colorSidebarHighlight = "#ececec"

            colorActionbar = colorGreen
            colorActionbarContent = "white"
            colorActionbarHighlight = "#7ab800"

            colorTabletmenu = "#ffffff"
            colorTabletmenuContent = "#9d9d9d"
            colorTabletmenuHighlight = "#0079fe"

            colorBackground = "white"
            colorForeground = "#f8f8f8"

            colorPrimary = colorYellow
            colorSecondary = "#ffe800"
            colorSuccess = colorGreen
            colorWarning = colorOrange
            colorError = colorRed

            colorText = "#474747"
            colorSubText = "#666666"
            colorIcon = "#474747"
            colorSeparator = "#ececec"
            colorLowContrast = "white"
            colorHighContrast = "#303030"

            colorDeviceWidget = "#fdfdfd"

            componentHeight = 40
            componentRadius = 20
            componentBorderWidth = 2

            colorComponent = "#EFEFEF"
            colorComponentText = "black"
            colorComponentContent = "black"
            colorComponentBorder = "#EAEAEA"
            colorComponentDown = "#DADADA"
            colorComponentBackground = "#FAFAFA"

        } else if (themeIndex === Theme.THEME_THEENGS) {

            colorGreen = "#8cd200"
            colorBlue = "#4cafe9"
            colorYellow = "#ffcf00"
            colorOrange = "#ffa635"
            colorRed = "#ff7657"

            isLight = true
            isDark = false

            themeStatusbar = Material.Dark
            colorStatusbar = "#0f295c"

            colorHeader = "#325da9" // blue
            colorHeaderHighlight = "#0f295c"
            colorHeaderContent = "white"

            colorSidebar = "#ffcf00"
            colorSidebarContent = "white"
            colorSidebarHighlight = "#ffb300"

            colorActionbar = colorGreen
            colorActionbarContent = "white"
            colorActionbarHighlight = "#7ab800"

            colorTabletmenu = "#f3f3f3"
            colorTabletmenuContent = "#9d9d9d"
            colorTabletmenuHighlight = "#0079fe"

            colorBackground = "white"
            colorForeground = "#f8f8f8"

            colorPrimary = "#fc9c2b"
            colorSecondary = "#ffa635"
            colorSuccess = colorGreen
            colorWarning = colorOrange
            colorError = colorRed

            colorText = "#474747"
            colorSubText = "#666666"
            colorIcon = "#474747"
            colorSeparator = "#ececec"
            colorLowContrast = "white"
            colorHighContrast = "#303030"

            colorDeviceWidget = "#fdfdfd"

            componentHeight = 40
            componentRadius = 8
            componentBorderWidth = 2

            colorComponent = "#EFEFEF"
            colorComponentText = "black"
            colorComponentContent = "black"
            colorComponentBorder = "#E8E8E8"
            colorComponentDown = "#DDDDDD"
            colorComponentBackground = "#FAFAFA"

        } else if (themeIndex === Theme.THEME_NIGHT) {

            colorGreen = "#58CF77"
            colorBlue = "#4dceeb"
            colorYellow = "#fcc632"
            colorOrange = "#ff8f35"
            colorRed = "#e8635a"

            isLight = false
            isDark = true

            themeStatusbar = Material.Dark
            colorStatusbar = "#725595"

            colorHeader = "#b16bee"
            colorHeaderContent = "white"
            colorHeaderHighlight = "#725595"

            colorSidebar = "#b16bee"
            colorSidebarContent = "white"
            colorSidebarHighlight = "#725595"

            colorActionbar = colorBlue
            colorActionbarContent = "white"
            colorActionbarHighlight = "#4dabeb"

            colorTabletmenu = "#292929"
            colorTabletmenuContent = "#808080"
            colorTabletmenuHighlight = "#bb86fc"

            colorBackground = "#313236"
            colorForeground = "#292929"

            colorPrimary = "#bb86fc"
            colorSecondary = "#b16bee"
            colorSuccess = colorGreen
            colorWarning = colorOrange
            colorError = colorRed

            colorText = "#EEE"
            colorSubText = "#AAA"
            colorIcon = "#EEE"
            colorSeparator = "#404040"
            colorLowContrast = "#111"
            colorHighContrast = "white"

            colorDeviceWidget = "#333"

            componentHeight = 40
            componentRadius = 4
            componentBorderWidth = 2

            colorComponent = "#757575"
            colorComponentText = "#eee"
            colorComponentContent = "white"
            colorComponentBorder = "#777"
            colorComponentDown = "#595959"
            colorComponentBackground = "#292929"

        }

        // This will emit the signal 'onCurrentThemeChanged'
        currentTheme = themeIndex
    }
}
