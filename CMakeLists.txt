cmake_minimum_required(VERSION 3.20)

project(Theengs VERSION 1.5.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_NAME ${CMAKE_PROJECT_NAME})
set(APP_VERSION ${CMAKE_PROJECT_VERSION})

option(ENABLE_MQTT "MQTT broker support" ON)
option(ENABLE_OPENSSL "OpenSSL for various encryption needs" ON)
option(ENABLE_MBEDTLS "mbedTLS for various encryption needs" OFF)

################################################################################

message(STATUS "[CMAKE]   CMAKE_VERSION             = ${CMAKE_VERSION}")
message(STATUS "[CMAKE]   CMAKE_PREFIX_PATH         = ${CMAKE_PREFIX_PATH}")
message(STATUS "[SYSTEM]  (HOST) SYSTEM NAME        = ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "[SYSTEM]  (HOST) SYSTEM VERSION     = ${CMAKE_HOST_SYSTEM_VERSION}")
message(STATUS "[SYSTEM]  (TARGET) SYSTEM NAME      = ${CMAKE_SYSTEM_NAME}")
message(STATUS "[SYSTEM]  (TARGET) SYSTEM VERSION   = ${CMAKE_SYSTEM_VERSION}")
message(STATUS "[PROJECT] SOURCE_DIR                = ${CMAKE_SOURCE_DIR}")
message(STATUS "[PROJECT] BUILD_TYPE                = ${CMAKE_BUILD_TYPE}")
message(STATUS "[PROJECT] TOOLCHAIN_FILE            = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "[PROJECT] QT_HOST_PATH              = ${QT_HOST_PATH}")

################################################################################

find_package(Qt6 REQUIRED COMPONENTS Core Bluetooth Network Sql)
find_package(Qt6 REQUIRED COMPONENTS Gui Svg Qml Quick QuickControls2 Charts)

qt_standard_project_setup(REQUIRES 6.8)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0002 NEW)
qt_policy(SET QTP0003 NEW)
qt_policy(SET QTP0004 NEW)

set(SOURCES
    src/main.cpp
    src/SettingsManager.cpp src/SettingsManager.h
    src/DatabaseManager.cpp src/DatabaseManager.h
    src/PermissionManager.cpp src/PermissionManager.h
    src/NotificationManager.cpp src/NotificationManager.h
    src/MqttManager.cpp src/MqttManager.h

    src/DeviceManager.cpp src/DeviceManager.h
    src/DeviceManager_advertisement.cpp src/DeviceManager_gateway.cpp src/DeviceManager_nearby.cpp src/DeviceManager_theengs.cpp
    src/DeviceFilter.cpp src/DeviceFilter.h
    src/device.cpp src/device.h src/device_utils.h src/device_utils_theengs.h src/device_utils_switchbot.h src/device_firmwares.h
    src/device_infos.cpp src/device_infos.h
    src/device_sensor.cpp src/device_sensor.h
    src/device_plantsensor.cpp src/device_plantsensor.h
    src/device_thermometer.cpp src/device_thermometer.h
    src/device_environmental.cpp src/device_environmental.h
        src/devices/device_flowercare.cpp src/devices/device_flowercare.h
        src/devices/device_flowercare_tuya.cpp src/devices/device_flowercare_tuya.h
        src/devices/device_ropot.cpp src/devices/device_ropot.h
        src/devices/device_hygrotemp_cgd1.cpp src/devices/device_hygrotemp_cgd1.h
        src/devices/device_hygrotemp_cgdk2.cpp src/devices/device_hygrotemp_cgdk2.h
        src/devices/device_hygrotemp_cgg1.cpp src/devices/device_hygrotemp_cgg1.h
        src/devices/device_hygrotemp_cgp1w.cpp src/devices/device_hygrotemp_cgp1w.h
        src/devices/device_hygrotemp_clock.cpp src/devices/device_hygrotemp_clock.h
        src/devices/device_hygrotemp_square.cpp src/devices/device_hygrotemp_square.h
        src/devices/device_hygrotemp_lywsdcgq.cpp src/devices/device_hygrotemp_lywsdcgq.h
        src/devices/device_thermobeacon.cpp src/devices/device_thermobeacon.h
        src/devices/device_jqjcy01ym.cpp src/devices/device_jqjcy01ym.h
    src/device_theengs.cpp src/device_theengs.h
        src/devices/device_theengs_gateway.cpp src/devices/device_theengs_gateway.h
        src/devices/device_theengs_generic.cpp src/devices/device_theengs_generic.h
        src/devices/device_theengs_actuators.cpp src/devices/device_theengs_actuators.h
        src/devices/device_theengs_windowactuators.cpp src/devices/device_theengs_windowactuators.h
        src/devices/device_theengs_beacons.cpp src/devices/device_theengs_beacons.h
        src/devices/device_theengs_batterymonitors.cpp src/devices/device_theengs_batterymonitors.h
        src/devices/device_theengs_motionsensors.cpp src/devices/device_theengs_motionsensors.h
        src/devices/device_theengs_probes.cpp src/devices/device_theengs_probes.h
        src/devices/device_theengs_scales.cpp src/devices/device_theengs_scales.h
        src/devices/device_theengs_thermometers.cpp src/devices/device_theengs_thermometers.h
        src/devices/device_bm26.cpp src/devices/device_bm26.h
        src/devices/device_sbbt.cpp src/devices/device_sbbt.h
        src/devices/device_sbcu.cpp src/devices/device_sbcu.h
        src/devices/device_sbs1.cpp src/devices/device_sbs1.h

    src/BatteryPresetManager.cpp src/BatteryPresetManager.h
    src/BatteryPreset.cpp src/BatteryPreset.h
    src/TempPresetManager.cpp src/TempPresetManager.h
    src/TempPreset.cpp src/TempPreset.h
    src/rc4/rc4.cpp src/rc4/rc4.h
    src/blufi/BluFiFrame.cpp src/blufi/BluFiFrame.h src/blufi/BluFiUtils.h

    thirdparty/TheengsDecoder/src/decoder.cpp thirdparty/TheengsDecoder/src/decoder.h

    assets/assets.qrc
    assets/devices.qrc
    thirdparty/IconLibrary/IconLibrary_material.qrc
)

if(IOS OR ANDROID)
    set(QAPPLICATION_CLASS QGuiApplication)
    set(SOURCES_MOBILE
        src/AndroidService.cpp src/AndroidService.h
    )
endif()

if(NOT IOS AND NOT ANDROID)
    # Set app icons
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(app_icon_windows "${CMAKE_SOURCE_DIR}/assets/windows/${CMAKE_PROJECT_NAME}.rc")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(app_icon_macos "${CMAKE_SOURCE_DIR}/assets/macos/${CMAKE_PROJECT_NAME}.icns")
    endif()

    # Desktop only source files
    set(SOURCES_DESKTOP
        src/MenubarManager.cpp src/MenubarManager.h
        src/SystrayManager.cpp src/SystrayManager.h
        ${app_icon_windows}
        ${app_icon_macos}
    )
endif()

qt_add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${SOURCES_DESKTOP} ${SOURCES_MOBILE})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE src/)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE thirdparty/TheengsDecoder/src/)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE thirdparty/TheengsDecoder/src/arduino_json/src)

qt_add_qml_module(${CMAKE_PROJECT_NAME}
    URI ${CMAKE_PROJECT_NAME}
    QML_FILES
        qml/MobileApplication.qml
        qml/MobileHeader.qml
        qml/MobileDrawer.qml
        qml/MobileMenu.qml
        qml/MobilePermissions.qml
        qml/DesktopApplication.qml
        qml/DesktopHeader.qml
        qml/About.qml
        qml/SettingsMqtt.qml
        qml/Settings.qml
        qml/GatewayList.qml
        qml/GatewayDevice.qml
        qml/DeviceList.qml
        qml/DeviceBrowser.qml
        qml/DevicePlantSensor.qml
        qml/DevicePlantSensorData.qml
        qml/DevicePlantSensorHistory.qml
        qml/DevicePlantSensorSettings.qml
        qml/DeviceThermometer.qml
        qml/DeviceEnvironmental.qml
        qml/DeviceGeneric.qml
        qml/DeviceActuator.qml
        qml/DeviceActuatorWindow.qml
        qml/DeviceBatteryMonitor.qml
        qml/DeviceProbe.qml
        qml/DeviceScale.qml
        qml/DeviceMotionSensor.qml
        qml/TemperaturePresetList.qml
        qml/TemperaturePreset.qml

        qml/onboarding/GatewayAuthentication.qml
        qml/onboarding/GatewayOnboarding.qml
        qml/onboarding/GatewayWiFi.qml
        qml/onboarding/GatewayMqtt.qml

        qml/charts/AirQualityIndicator.qml
        qml/charts/ChartEnvironmentalVoc.qml
        qml/charts/ChartHistory.qml
        qml/charts/ChartPlantDataAio.qml
        qml/charts/ChartScaleDataAio.qml
        qml/charts/ChartProbeDataAio.qml
        qml/charts/ChartProbeRealTime.qml
        qml/charts/ChartBatteryRealTime.qml
        qml/charts/ChartBatteryHistory.qml
        qml/charts/ChartThermometerMinMax.qml
        qml/charts/ChartThermometerMinMaxBar.qml
        qml/charts/IndicatorsCompact.qml
        qml/charts/IndicatorsSolid.qml

        qml/components/ActionMenuFixed.qml
        qml/components/BannerButton.qml
        qml/components/TextInputThemed.qml
        qml/components/ItemBannerSync.qml
        qml/components/ItemEnvBox.qml
        qml/components/ItemWeatherBox.qml
        qml/components/ItemLoadData.qml
        qml/components/ItemBroker.qml
        qml/components/ItemNetwork.qml
        qml/components/ItemNetworkInfo.qml
        qml/components/ItemNoBluetooth.qml
        qml/components/ItemNoPermissions.qml
        qml/components/ItemNoData.qml
        qml/components/ItemNoDevice.qml
        qml/components/ItemNoDeviceNearby.qml
        qml/components/ItemNoGateway.qml

        qml/popups/PopupBackgroundUpdates.qml
        qml/popups/PopupBlacklistDevice.qml
        qml/popups/PopupCalibration.qml
        qml/popups/PopupDeleteData.qml
        qml/popups/PopupDeleteDevice.qml
        qml/popups/PopupGatewayConfirm.qml
        qml/popups/PopupMacAddress.qml
        qml/popups/PopupBatteryPresetSelection.qml
        qml/popups/PopupTempPresetAdd.qml
        qml/popups/PopupTempPresetCopy.qml
        qml/popups/PopupTempPresetDelete.qml
        qml/popups/PopupTempPresetRangeAdd.qml
        qml/popups/PopupTempPresetRangeDelete.qml
        qml/popups/PopupTempPresetSelection.qml

        qml/widgets/DebugWidget.qml
        qml/widgets/DeviceWidget.qml
        qml/widgets/DeviceNearbyWidget.qml
        qml/widgets/GatewayWidget.qml
        qml/widgets/BatteryPresetSearchWidget.qml
        qml/widgets/TemperaturePresetWidget.qml
        qml/widgets/TemperaturePresetSearchWidget.qml
        qml/widgets/TemperatureRangeWidget.qml

        qml/components_js/UtilsDeviceSensors.js
        qml/components_js/UtilsDeviceTheengs.js
        qml/components_js/UtilsPresets.js
)

################################################################################

qt_standard_project_setup(
    I18N_SOURCE_LANGUAGE en
)

################################################################################

# Qt libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Bluetooth
    Qt6::Network
    Qt6::Sql
    Qt6::Gui
    Qt6::Svg
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Charts
)

if(IOS OR ANDROID)
    #
endif()

if(NOT IOS AND NOT ANDROID)
    # for proper systray and menubar support
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Widgets)
endif()

if(ENABLE_MQTT)
    add_definitions(-DENABLE_MQTT)
    find_package(Qt6 REQUIRED COMPONENTS Mqtt)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Mqtt)
endif()
if(ENABLE_MBEDTLS)
    add_definitions(-DENABLE_MBEDTLS)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "-lmbedtls -lmbedx509 -lmbedcrypto")
endif()

################################################################################

# SingleApplication
set(QAPPLICATION_CLASS QApplication)
add_subdirectory(thirdparty/SingleApplication)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SingleApplication)

# AppUtils
set(UTILS_DOCK_ENABLED true)
set(UTILS_WIFI_ENABLED false)
set(UTILS_NOTIFICATIONS_ENABLED true)
add_subdirectory(thirdparty/AppUtils)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE AppUtils)

# MobileUI
add_subdirectory(thirdparty/MobileUI)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE MobileUI)

# MobileSharing
add_subdirectory(thirdparty/MobileSharing)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE MobileSharing)

# ComponentLibrary
add_subdirectory(thirdparty/ComponentLibrary)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ComponentLibraryplugin)

# TheengsDecoder
#add_subdirectory(thirdparty/TheengsDecoder/)
#target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE TheengsDecoder)

################################################################################

## Android
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        #QT_ANDROID_ABIS "armeabi-v7a;arm64-v8a;x86;x86_64"
        #QT_ANDROID_BUILD_ALL_ABIS ON
        QT_ANDROID_MIN_SDK_VERSION 28
        QT_ANDROID_TARGET_SDK_VERSION 36
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets/android/"
    )

    # Download & ibclude SSL Libraries
    include(FetchContent)
    FetchContent_Declare(
        android_openssl
        DOWNLOAD_EXTRACT_TIMESTAMP true
        URL https://github.com/KDAB/android_openssl/archive/refs/heads/master.zip
    )
    FetchContent_MakeAvailable(android_openssl)
    include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)
    add_android_openssl_libraries(${CMAKE_PROJECT_NAME})

    # Patched QtConnectivity
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QT_CONNECTIVITY_PATCHED)

    # Android background service
    find_package(Qt6 REQUIRED COMPONENTS CorePrivate)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt::CorePrivate)
endif()

## macOS
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    #set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/assets/macos/Info.plist"
        # New Xcode attributes (???)
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER           "com.theengs.app"
        XCODE_ATTRIBUTE_PRODUCT_NAME                        ${CMAKE_PROJECT_NAME}
        XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION             ${CMAKE_PROJECT_VERSION}
        XCODE_ATTRIBUTE_MARKETING_VERSION                   ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS             "${CMAKE_SOURCE_DIR}/assets/macos/${CMAKE_PROJECT_NAME}.entitlements"
    )
endif()

## iOS
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(app_icon_ios "${CMAKE_SOURCE_DIR}/assets/ios/Images.xcassets")
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${app_icon_ios})
    set_source_files_properties(${app_icon_ios} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/assets/ios/Info.plist"
        # Qt
        QT_IOS_LAUNCH_SCREEN "${CMAKE_SOURCE_DIR}/assets/ios/LaunchScreen.storyboard"
        # New Xcode attributes (???)
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER           "com.theengs.app"
        XCODE_ATTRIBUTE_PRODUCT_NAME                        ${CMAKE_PROJECT_NAME}
        XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION             ${CMAKE_PROJECT_VERSION}
        XCODE_ATTRIBUTE_MARKETING_VERSION                   ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET          "16.0"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY              "1,2" # iPhone,iPad
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME  "AppIcon"
        XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS      "YES"
    )
endif()

## Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

## Release build? Set "no debug" macros
if(CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        NDEBUG
        QT_NO_DEBUG
        QT_NO_DEBUG_OUTPUT
    )
endif()

################################################################################

# Output (build)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    #ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    #LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Output (install)
install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

################################################################################
