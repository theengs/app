/*
    Theengs - Decode things and devices
    Copyright: (c) Florian ROBERT

    This file is part of Theengs.

    Theengs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Theengs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SettingsManager.h"
#include "PermissionManager.h"
#include "DeviceManager.h"
#include "SystrayManager.h"
#include "NotificationManager.h"
#include "MenubarManager.h"

#include "MqttManager.h"
#include "BatteryPresetManager.h"
#include "BatteryPreset.h"
#include "TempPresetManager.h"
#include "TempPreset.h"
#include "device_utils_theengs.h"
#include "device_utils_switchbot.h"

#include "utils_app.h"
#include "utils_screen.h"
#include "utils_language.h"
#include "utils_wifi.h"
#if defined(Q_OS_MACOS)
#include "utils_os_macos_dock.h"
#endif

#include <MobileUI>
#include <MobileSharing>
#include <SingleApplication>

#include <QtGlobal>
#include <QLibraryInfo>
#include <QVersionNumber>

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSurfaceFormat>

#if defined(Q_OS_ANDROID)
#include <unistd.h> // _exit()
#include "AndroidService.h"
#include "private/qandroidextras_p.h" // for QAndroidService
#endif

/* ************************************************************************** */

int main(int argc, char *argv[])
{
    // Arguments parsing ///////////////////////////////////////////////////////

    bool start_minimized = false;
    bool background_service = false;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i])
        {
            //qDebug() << "> arg >" << argv[i];

            if (QString::fromLocal8Bit(argv[i]) == "--start-minimized")
                start_minimized = true;
            if (QString::fromLocal8Bit(argv[i]) == "--service")
                background_service = true;
        }
    }

    // Background service application //////////////////////////////////////////

    // Android daemon
    if (background_service)
    {
#if defined(Q_OS_ANDROID)
        QAndroidService app(argc, argv);
        app.setApplicationName("Theengs");
        app.setOrganizationName("Theengs");
        app.setOrganizationDomain("Theengs");

        SettingsManager *sm = SettingsManager::getInstance();
        if (sm && sm->getSysTray())
        {
            AndroidService *as = new AndroidService();
            if (!as) _exit(EXIT_FAILURE);

            const int rc = app.exec();
            // Terminate this Qt-hosted service process with _exit() rather than
            // a normal return (which calls exit()). On Android, exiting via
            // exit() runs __cxa_finalize, which tears down the ART runtime and
            // aborts ("destroying mutex with owner or contenders") whenever
            // another runtime thread (JIT/GC/Binder/Qt) still holds a lock at
            // teardown — a ~8% race per service exit, observed crashing the
            // ":qt_service" process repeatedly in the field. _exit() ends the
            // process via exit_group without running static destructors; the
            // process is terminating regardless, so the skipped C++/Qt cleanup
            // is moot.
            _exit(rc);
        }

        // Background updates disabled: this service has nothing to do. Same
        // _exit() rationale as above — the service is frequently (re)spawned by
        // the boot receiver / OS service-restart even when idle, so avoiding
        // the exit()/__cxa_finalize ART-teardown abort here removes the bulk of
        // the crashes.
        _exit(EXIT_SUCCESS);
#endif
    }

    // Hacks ///////////////////////////////////////////////////////////////////

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    // Qt 6.6+ mouse wheel hack
    qputenv("QT_QUICK_FLICKABLE_WHEEL_DECELERATION", "2500");
#endif

#if defined(Q_OS_ANDROID)
    // The threaded Qt Quick render loop (Android default) races the SurfaceView
    // lifecycle on background/foreground transitions: when eglCreateWindowSurface
    // fails mid-transition, QAndroidPlatformOpenGLWindow::ensureEglSurfaceCreated()
    // calls qFatal() and the app aborts. Still unfixed upstream (QTBUG-142195;
    // qFatal present in qtbase v6.10.3 / 6.10 / dev), so forcing the basic
    // (GUI-thread) render loop serializes rendering with surface events and avoids
    // the race. Respect an explicit override if the environment already sets it.
    if (qEnvironmentVariableIsEmpty("QSG_RENDER_LOOP"))
        qputenv("QSG_RENDER_LOOP", "basic");
#endif

    // GUI application /////////////////////////////////////////////////////////

    SingleApplication app(argc, argv, true);

    // Application name
    app.setApplicationName("Theengs");
    app.setApplicationDisplayName("Theengs");
    app.setOrganizationName("Theengs");
    app.setOrganizationDomain("Theengs");

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    ShareUtils *utilsShare = new ShareUtils();
#else
    // Application icon
    QIcon appIcon(":/assets/logos/logo.png");
    app.setWindowIcon(appIcon);

    SystrayManager *st = SystrayManager::getInstance();
    MenubarManager *mb = MenubarManager::getInstance();
#endif

    // Init components
    PermissionManager *pm = PermissionManager::getInstance();
    SettingsManager *sm = SettingsManager::getInstance();
    MqttManager *mq = MqttManager::getInstance();
    BatteryPresetManager *bpm = BatteryPresetManager::getInstance();
    TempPresetManager *tpm = TempPresetManager::getInstance();
    NotificationManager *nm = NotificationManager::getInstance();
    DeviceManager *dm = new DeviceManager;
    if (!sm || !mq || !nm || !dm)
    {
        qWarning() << "Cannot init Theengs components!";
        return EXIT_FAILURE;
    }

#if defined(Q_OS_ANDROID)
    // Android 13 (API 33) reclassified POST_NOTIFICATIONS as a runtime
    // permission. Without it the foreground-service notification is
    // silently dropped (dumpsys reports numPostedByApp=0 despite the FGS
    // running), so the user can't see / dismiss the App. The helper
    // no-ops on pre-API-33 phones via Build.VERSION.SDK_INT, so this is
    // a single unconditional call here. Fire-and-forget — the dialog is
    // asynchronous and QML re-reads the property on activity resume.
    pm->requestNotificationPermission();
#endif

    // Runtime-only CLI overrides for MQTT TLS. ``--mqtt-ca <path>`` forces
    // TLS on and sets the CA cert path; ``--mqtt-port <int>`` overrides
    // the port; ``--mqtt-tls-insecure`` skips peer verification (needed
    // when the bench broker's cert doesn't match the connected hostname).
    // All bypass QSettings write so the user's persisted broker config
    // is preserved across launches. Used by
    // hil-bench/tests/test_theengs_ios/test_tls_publish.py.
    {
        const QStringList args = QCoreApplication::arguments();
        const int caIdx = args.indexOf("--mqtt-ca");
        if (caIdx >= 0 && caIdx + 1 < args.size())
        {
            const QString caPath = args.at(caIdx + 1);
            int port = -1;
            const int portIdx = args.indexOf("--mqtt-port");
            if (portIdx >= 0 && portIdx + 1 < args.size())
                port = args.at(portIdx + 1).toInt();
            const bool insecure = args.contains("--mqtt-tls-insecure");
            sm->applyMqttCliOverrides(/*tls=*/true, caPath, port, insecure);
        }
    }

    // Start MQTT
    if (sm->getMQTT())
    {
        QObject::connect(mq, &MqttManager::connected, dm, &DeviceManager::discoverTheengsDevices);
        mq->connect();
    }

    // Init generic utils
    UtilsApp *utilsApp = UtilsApp::getInstance();
    UtilsScreen *utilsScreen = UtilsScreen::getInstance();
    UtilsWiFi *utilsWifi = UtilsWiFi::getInstance();
    UtilsLanguage *utilsLanguage = UtilsLanguage::getInstance();
    if (!utilsScreen || !utilsApp || !utilsLanguage)
    {
        qWarning() << "Cannot init Theengs utils!";
        return EXIT_FAILURE;
    }

    // Translate the application
    utilsLanguage->loadLanguage(sm->getAppLanguage());

    //
    MobileUI::registerQML();
    DeviceUtils::registerQML();
    DeviceUtilsTheengs::registerQML();
    DeviceUtilsSwitchBot::registerQML();
    BatteryPresetUtils::registerQML();
    TempPresetUtils::registerQML();

    // Then we start the UI
    QQmlApplicationEngine engine;
    QQmlContext *engine_context = engine.rootContext();

    engine_context->setContextProperty("settingsManager", sm);
    engine_context->setContextProperty("permissionManager", pm);
    engine_context->setContextProperty("deviceManager", dm);
    engine_context->setContextProperty("mqttManager", mq);
    engine_context->setContextProperty("notificationManager", nm);
    engine_context->setContextProperty("batteryPresetsManager", bpm);
    engine_context->setContextProperty("tempPresetsManager", tpm);

    engine_context->setContextProperty("utilsApp", utilsApp);
    engine_context->setContextProperty("utilsWifi", utilsWifi);
    engine_context->setContextProperty("utilsScreen", utilsScreen);
    engine_context->setContextProperty("utilsLanguage", utilsLanguage);

    engine_context->setContextProperty("startMinimized", (start_minimized || sm->getMinimized()));
#if defined(QT_CONNECTIVITY_PATCHED)
    engine_context->setContextProperty("qtConnectivityPatched", true);
#else
    engine_context->setContextProperty("qtConnectivityPatched", false);
#endif
#if defined(ENABLE_MBEDTLS)
    engine_context->setContextProperty("mbedtlsEnabled", true);
#else
    engine_context->setContextProperty("mbedtlsEnabled", false);
#endif
#if defined(ENABLE_MQTT)
    engine_context->setContextProperty("mqttEnabled", true);
#else
    engine_context->setContextProperty("mqttEnabled", false);
#endif


    // Load the main view
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(FORCE_MOBILE_UI)
    engine_context->setContextProperty("utilsShare", utilsShare);
    engine.loadFromModule("Theengs", "MobileApplication");
#else
    engine_context->setContextProperty("systrayManager", st);
    engine_context->setContextProperty("menubarManager", mb);
    engine.loadFromModule("Theengs", "DesktopApplication");
#endif
    if (engine.rootObjects().isEmpty())
    {
        qWarning() << "Cannot init QmlApplicationEngine!";
        return EXIT_FAILURE;
    }

    // For i18n retranslate
    utilsLanguage->setQmlEngine(&engine);

    // Notch handling // QQuickWindow must be valid at this point
    QQuickWindow *window = qobject_cast<QQuickWindow *>(engine.rootObjects().value(0));
    engine_context->setContextProperty("quickWindow", window);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) // desktop section

    // React to secondary instances
    QObject::connect(&app, &SingleApplication::instanceStarted, window, &QQuickWindow::show);
    QObject::connect(&app, &SingleApplication::instanceStarted, window, &QQuickWindow::raise);

    // Systray?
    st->setupSystray(&app, window);
    if (sm->getSysTray()) st->installSystray();

    // Menu bar
    mb->setupMenubar(window, dm);

#if defined(Q_OS_MACOS)
    // dock
    MacOSDockHandler *dockIconHandler = MacOSDockHandler::getInstance();
    dockIconHandler->setupDock(window);
    engine_context->setContextProperty("utilsDock", dockIconHandler);
#endif

#endif // desktop section

#if defined(Q_OS_ANDROID)
    QNativeInterface::QAndroidApplication::hideSplashScreen(333);
    if (sm->getSysTray()) AndroidService::service_start();
#endif

    return app.exec();
}

/* ************************************************************************** */
