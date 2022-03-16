TARGET  = Theengs

VERSION = 0.1
DEFINES+= APP_VERSION=\\\"$$VERSION\\\"

CONFIG += c++17
QT     += core bluetooth sql
QT     += mqtt
QT     += qml quick quickcontrols2 svg widgets charts

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
include(src/thirdparty/SingleApplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

# Project files ################################################################

SOURCES  += src/main.cpp \
            src/SettingsManager.cpp \
            src/DatabaseManager.cpp \
            src/SystrayManager.cpp \
            src/NotificationManager.cpp \
            src/AndroidService.cpp \
            src/DeviceManager.cpp \
            src/device.cpp \
            src/device_infos.cpp \
            src/device_filter.cpp \
            src/device_sensor.cpp \
            src/devices/device_flowercare.cpp \
            src/devices/device_flowerpower.cpp \
            src/devices/device_hygrotemp_lcd.cpp \
            src/devices/device_hygrotemp_cgg1.cpp \
            src/devices/device_hygrotemp_clock.cpp \
            src/devices/device_hygrotemp_square.cpp \
            src/devices/device_hygrotemp_cgdk2.cpp \
            src/devices/device_thermobeacon.cpp \
            src/devices/device_ropot.cpp \
            src/devices/device_parrotpot.cpp \
            src/devices/device_esp32_airqualitymonitor.cpp \
            src/devices/device_esp32_higrow.cpp \
            src/devices/device_esp32_geigercounter.cpp \
            src/devices/device_ess_generic.cpp \
            src/devices/device_wp6003.cpp \
            src/utils/utils_app.cpp \
            src/utils/utils_language.cpp \
            src/utils/utils_maths.cpp \
            src/utils/utils_screen.cpp \
            src/thirdparty/RC4/rc4.cpp

HEADERS  += src/SettingsManager.h \
            src/DatabaseManager.h \
            src/SystrayManager.h \
            src/NotificationManager.h \
            src/AndroidService.h \
            src/DeviceManager.h \
            src/device.h \
            src/device_utils.h \
            src/device_infos.h \
            src/device_filter.h \
            src/device_sensor.h \
            src/devices/device_flowercare.h \
            src/devices/device_flowerpower.h \
            src/devices/device_hygrotemp_lcd.h \
            src/devices/device_hygrotemp_cgg1.h \
            src/devices/device_hygrotemp_clock.h \
            src/devices/device_hygrotemp_square.h \
            src/devices/device_hygrotemp_cgdk2.h \
            src/devices/device_thermobeacon.h \
            src/devices/device_ropot.h \
            src/devices/device_parrotpot.h \
            src/devices/device_esp32_airqualitymonitor.h \
            src/devices/device_esp32_higrow.h \
            src/devices/device_esp32_geigercounter.h \
            src/devices/device_ess_generic.h \
            src/devices/device_wp6003.h \
            src/utils/utils_app.h \
            src/utils/utils_language.h \
            src/utils/utils_maths.h \
            src/utils/utils_screen.h \
            src/utils/utils_versionchecker.h \
            src/thirdparty/RC4/rc4.h

INCLUDEPATH += src/

RESOURCES   += qml/qml.qrc \
               assets/assets.qrc \
               assets/devices.qrc

OTHER_FILES += .gitignore

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
LIBS            += -L$${CONTRIBS_DIR}/lib/
LIBS            += -ldecoder

# Build settings ###############################################################

unix {
    # Enables AddressSanitizer
    #QMAKE_CXXFLAGS += -fsanitize=address,undefined
    #QMAKE_CXXFLAGS += -Wno-nullability-completeness
    #QMAKE_LFLAGS += -fsanitize=address,undefined
}

DEFINES += QT_DEPRECATED_WARNINGS
QT_LOGGING_RULES="qt.qml.connections=false"

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

    # Linux utils
    SOURCES += src/utils/utils_os_linux.cpp
    HEADERS += src/utils/utils_os_linux.h
    QT += dbus

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

android {
    # ANDROID_TARGET_ARCH: [x86_64, armeabi-v7a, arm64-v8a]
    #message("ANDROID_TARGET_ARCH: $$ANDROID_TARGET_ARCH")

    # Bundle name
    QMAKE_TARGET_BUNDLE_PREFIX = com.theengs
    QMAKE_BUNDLE = app

    # android utils
    QT += core-private
    SOURCES += src/utils/utils_os_android_qt6.cpp
    HEADERS += src/utils/utils_os_android.h

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

    DEFINES += LIBS_SUFFIX='\\"_$${QT_ARCH}.so\\"'
    ANDROID_EXTRA_LIBS += \
        $${PWD}/contribs/env/android_armv7/usr/lib/libdecoder.so \
        $${PWD}/contribs/env/android_armv8/usr/lib/libdecoder.so \
        $${PWD}/contribs/env/android_x86/usr/lib/libdecoder.so \
        $${PWD}/contribs/env/android_x86_64/usr/lib/libdecoder.so
}

ios {
    #QMAKE_IOS_DEPLOYMENT_TARGET = 11.0
    #message("QMAKE_IOS_DEPLOYMENT_TARGET: $$QMAKE_IOS_DEPLOYMENT_TARGET")

    CONFIG += no_autoqmake

    # iOS utils
    SOURCES += src/utils/utils_os_ios.mm
    HEADERS += src/utils/utils_os_ios.h
    LIBS    += -framework UIKit

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
