TARGET  = Theengs

VERSION = 1.1
DEFINES+= APP_NAME=\\\"$$TARGET\\\"
DEFINES+= APP_VERSION=\\\"$$VERSION\\\"

CONFIG += c++17
QT     += core bluetooth sql
QT     += qml quick quickcontrols2 svg widgets charts
QT     += mqtt

# Validate Qt version
!versionAtLeast(QT_VERSION, 6.3) : error("You need at least Qt version 6.3 for $${TARGET}")

# Project features #############################################################

# Use Qt Quick compiler
ios | android { CONFIG += qtquickcompiler }

win32 { DEFINES += _USE_MATH_DEFINES }

# MobileUI and MobileSharing for mobile OS
include(src/thirdparty/MobileUI/MobileUI.pri)
include(src/thirdparty/MobileSharing/MobileSharing.pri)

# SingleApplication for desktop OS
include(src/thirdparty/SingleApplication/SingleApplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

# Various utils
include(src/thirdparty/AppUtils/AppUtils.pri)

# Theengs submodule
SOURCES         += $${PWD}/src/thirdparty/TheengsDecoder/src/decoder.cpp
INCLUDEPATH     += $${PWD}/src/thirdparty/TheengsDecoder/src/
INCLUDEPATH     += $${PWD}/src/thirdparty/TheengsDecoder/src/arduino_json/src/

# Project files ################################################################

SOURCES  += src/main.cpp \
            src/SettingsManager.cpp \
            src/DatabaseManager.cpp \
            src/MenubarManager.cpp \
            src/SystrayManager.cpp \
            src/NotificationManager.cpp \
            src/AndroidService.cpp \
            src/MqttManager.cpp \
            src/DeviceManager.cpp \
            src/DeviceManager_advertisement.cpp \
            src/DeviceManager_nearby.cpp \
            src/DeviceManager_theengs.cpp \
            src/DeviceFilter.cpp \
            src/device.cpp \
            src/device_infos.cpp \
            src/device_sensor.cpp \
            src/device_plantsensor.cpp \
            src/device_thermometer.cpp \
            src/device_environmental.cpp \
            src/devices/device_flowercare.cpp \
            src/devices/device_flowercare_tuya.cpp \
            src/devices/device_ropot.cpp \
            src/devices/device_hygrotemp_cgd1.cpp \
            src/devices/device_hygrotemp_cgdk2.cpp \
            src/devices/device_hygrotemp_cgg1.cpp \
            src/devices/device_hygrotemp_cgp1w.cpp \
            src/devices/device_hygrotemp_clock.cpp \
            src/devices/device_hygrotemp_square.cpp \
            src/devices/device_hygrotemp_lywsdcgq.cpp \
            src/devices/device_thermobeacon.cpp \
            src/devices/device_jqjcy01ym.cpp \
            src/device_theengs.cpp \
            src/devices/device_theengs_generic.cpp \
            src/devices/device_theengs_beacons.cpp \
            src/devices/device_theengs_motionsensors.cpp \
            src/devices/device_theengs_probes.cpp \
            src/devices/device_theengs_scales.cpp \
            src/thirdparty/RC4/rc4.cpp

HEADERS  += src/SettingsManager.h \
            src/DatabaseManager.h \
            src/MenubarManager.h \
            src/SystrayManager.h \
            src/NotificationManager.h \
            src/AndroidService.h \
            src/MqttManager.h \
            src/DeviceManager.h \
            src/DeviceFilter.h \
            src/device.h \
            src/device_firmwares.h \
            src/device_utils.h \
            src/device_utils_theengs.h \
            src/device_infos.h \
            src/device_sensor.h \
            src/device_plantsensor.h \
            src/device_thermometer.h \
            src/device_environmental.h \
            src/devices/device_flowercare.h \
            src/devices/device_flowercare_tuya.h \
            src/devices/device_ropot.h \
            src/devices/device_hygrotemp_cgd1.h \
            src/devices/device_hygrotemp_cgdk2.h \
            src/devices/device_hygrotemp_cgg1.h \
            src/devices/device_hygrotemp_cgp1w.h \
            src/devices/device_hygrotemp_clock.h \
            src/devices/device_hygrotemp_square.h \
            src/devices/device_hygrotemp_lywsdcgq.h \
            src/devices/device_thermobeacon.h \
            src/devices/device_jqjcy01ym.h \
            src/device_theengs.h \
            src/devices/device_theengs_generic.h \
            src/devices/device_theengs_beacons.h \
            src/devices/device_theengs_motionsensors.h \
            src/devices/device_theengs_probes.h \
            src/devices/device_theengs_scales.h \
            src/thirdparty/RC4/rc4.h

INCLUDEPATH += src/ src/thirdparty/

RESOURCES   += qml/qml.qrc \
               assets/assets.qrc \
               assets/devices.qrc

OTHER_FILES += README.md \
               deploy_linux.sh \
               deploy_macos.sh \
               deploy_windows.sh \
               contribs/contribs.py \
               .github/workflows/builds_desktop.yml \
               .github/workflows/builds_mobile.yml \
               .github/workflows/flatpak.yml \
               .gitignore

lupdate_only {
    SOURCES += qml/*.qml qml/*.js \
               qml/components/*.qml qml/components_generic/*.qml qml/components_js/*.js
}

# Dependencies #################################################################

ARCH = "x86_64"
linux { PLATFORM = "linux" }
macx { PLATFORM = "macOS" }
win32 { PLATFORM = "windows" }

android { # ANDROID_TARGET_ARCH available: x86 x86_64 armeabi-v7a arm64-v8a
    PLATFORM = "android"
    equals(ANDROID_TARGET_ARCH, "x86") { ARCH = "x86" }
    equals(ANDROID_TARGET_ARCH, "x86_64") { ARCH = "x86_64" }
    equals(ANDROID_TARGET_ARCH, "armeabi-v7a") { ARCH = "armv7" }
    equals(ANDROID_TARGET_ARCH, "arm64-v8a") { ARCH = "armv8" }
}
ios { # QMAKE_APPLE_DEVICE_ARCHS available: armv7 arm64
    PLATFORM = "iOS"
    ARCH = "armv8" # can be simulator, armv7 and armv8
    QMAKE_APPLE_DEVICE_ARCHS = "arm64" # force 'arm64'
}

CONTRIBS_DIR = $${PWD}/contribs/env/$${PLATFORM}_$${ARCH}/usr

INCLUDEPATH     += $${CONTRIBS_DIR}/include/
QMAKE_LIBDIR    += $${CONTRIBS_DIR}/lib/
QMAKE_RPATHDIR  += $${CONTRIBS_DIR}/lib/

# Build settings ###############################################################

unix {
    # Enables AddressSanitizer
    #QMAKE_CXXFLAGS += -fsanitize=address,undefined
    #QMAKE_CXXFLAGS += -Wno-nullability-completeness
    #QMAKE_LFLAGS += -fsanitize=address,undefined
}

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(release, debug|release) : DEFINES += NDEBUG QT_NO_DEBUG QT_NO_DEBUG_OUTPUT

# Build artifacts ##############################################################

OBJECTS_DIR = build/$${QT_ARCH}/
MOC_DIR     = build/$${QT_ARCH}/
RCC_DIR     = build/$${QT_ARCH}/
UI_DIR      = build/$${QT_ARCH}/
QMLCACHE_DIR= build/$${QT_ARCH}/

DESTDIR     = bin/

################################################################################
# Application deployment and installation steps

linux:!android {
    TARGET = $$lower($${TARGET})

    # Automatic application packaging # Needs linuxdeployqt installed
    #system(linuxdeployqt $${OUT_PWD}/$${DESTDIR}/ -qmldir=qml/)

    # Application packaging # Needs linuxdeployqt installed
    #deploy.commands = $${OUT_PWD}/$${DESTDIR}/ -qmldir=qml/
    #install.depends = deploy
    #QMAKE_EXTRA_TARGETS += install deploy

    # Installation steps
    isEmpty(PREFIX) { PREFIX = /usr/local }
    target_app.files       += $${OUT_PWD}/$${DESTDIR}/$$lower($${TARGET})
    target_app.path         = $${PREFIX}/bin/
    target_appentry.files  += $${OUT_PWD}/assets/linux/$$lower($${TARGET}).desktop
    target_appentry.path    = $${PREFIX}/share/applications
    target_appdata.files   += $${OUT_PWD}/assets/linux/$$lower($${TARGET}).appdata.xml
    target_appdata.path     = $${PREFIX}/share/appdata
    target_icon_appimage.files += $${OUT_PWD}/assets/linux/$$lower($${TARGET}).svg
    target_icon_appimage.path   = $${PREFIX}/share/pixmaps/
    target_icon_flatpak.files  += $${OUT_PWD}/assets/linux/$$lower($${TARGET}).svg
    target_icon_flatpak.path    = $${PREFIX}/share/icons/hicolor/scalable/apps/
    INSTALLS += target_app target_appentry target_appdata target_icon_appimage target_icon_flatpak

    # Clean appdir/ and bin/ directories
    #QMAKE_CLEAN += $${OUT_PWD}/$${DESTDIR}/$$lower($${TARGET})
    #QMAKE_CLEAN += $${OUT_PWD}/appdir/
}

win32 {
    # OS icon
    RC_ICONS = $${PWD}/assets/windows/$$lower($${TARGET}).ico

    # Deploy step
    deploy.commands = $$quote(windeployqt $${OUT_PWD}/$${DESTDIR}/ --qmldir qml/)
    install.depends = deploy
    QMAKE_EXTRA_TARGETS += install deploy

    # Installation step
    # TODO

    # Clean step
    # TODO
}

macx {
    # Bundle name
    QMAKE_TARGET_BUNDLE_PREFIX = com.theengs
    QMAKE_BUNDLE = app

    # OS
    ICON = $${PWD}/assets/macos/theengs.icns

    # OS infos
    QMAKE_INFO_PLIST = $${PWD}/assets/macos/Info.plist

    # OS entitlement (sandbox and stuff)
    ENTITLEMENTS.name = CODE_SIGN_ENTITLEMENTS
    ENTITLEMENTS.value = $${PWD}/assets/macos/$$lower($${TARGET}).entitlements
    QMAKE_MAC_XCODE_SETTINGS += ENTITLEMENTS

    # Target architecture(s)
    QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64

    # Target OS
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
}

android {
    # ANDROID_TARGET_ARCH: [x86_64, armeabi-v7a, arm64-v8a]
    #message("ANDROID_TARGET_ARCH: $$ANDROID_TARGET_ARCH")

    # Bundle name
    QMAKE_TARGET_BUNDLE_PREFIX = com.theengs
    QMAKE_BUNDLE = app

    ANDROID_PACKAGE_SOURCE_DIR = $${PWD}/assets/android

    OTHER_FILES += $${PWD}/assets/android/src/com/theengs/app/TheengsBootServiceBroadcastReceiver.java \
                   $${PWD}/assets/android/src/com/theengs/app/TheengsAndroidService.java \
                   $${PWD}/assets/android/src/com/theengs/app/TheengsAndroidNotifier.java \
                   $${PWD}/assets/android/src/com/theengs/utils/QShareUtils.java \
                   $${PWD}/assets/android/src/com/theengs/utils/QSharePathResolver.java

    DISTFILES += $${PWD}/assets/android/AndroidManifest.xml \
                 $${PWD}/assets/android/gradle.properties \
                 $${PWD}/assets/android/build.gradle

    include($${PWD}/contribs/env/android_openssl-master/openssl.pri)

    #DEFINES += LIBS_SUFFIX='\\"_$${QT_ARCH}.so\\"'
    #ANDROID_EXTRA_LIBS += \
    #    $${PWD}/contribs/env/android_armv7/usr/lib/libdecoder.so \
    #    $${PWD}/contribs/env/android_armv8/usr/lib/libdecoder.so \
    #    $${PWD}/contribs/env/android_x86/usr/lib/libdecoder.so \
    #    $${PWD}/contribs/env/android_x86_64/usr/lib/libdecoder.so
}

ios {
    #QMAKE_IOS_DEPLOYMENT_TARGET = 11.0
    #message("QMAKE_IOS_DEPLOYMENT_TARGET: $$QMAKE_IOS_DEPLOYMENT_TARGET")

    CONFIG += no_autoqmake

    # Bundle name
    QMAKE_TARGET_BUNDLE_PREFIX = com.theengs
    QMAKE_BUNDLE = app

    # OS infos
    QMAKE_INFO_PLIST = $${PWD}/assets/ios/Info.plist
    QMAKE_APPLE_TARGETED_DEVICE_FAMILY = 1,2 # 1: iPhone / 2: iPad / 1,2: Universal

    # OS icons
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"
    QMAKE_ASSET_CATALOGS = $${PWD}/assets/ios/Images.xcassets

    # iOS launch screen
    AppLaunchScreen.files += $${PWD}/assets/ios/AppLaunchScreen.storyboard
    QMAKE_BUNDLE_DATA += AppLaunchScreen

    # iOS developer settings
    exists($${PWD}/assets/ios/ios_signature.pri) {
        # Must contain values for:
        # QMAKE_DEVELOPMENT_TEAM
        # QMAKE_PROVISIONING_PROFILE
        include($${PWD}/assets/ios/ios_signature.pri)
    }
}
