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

#include "DatabaseManager.h"
#include "SettingsManager.h"
#include "MqttManager.h"
#include "DeviceManager.h"
#include "SystrayManager.h"
#include "NotificationManager.h"
#include "device_utils_theengs.h"
#include "utils/utils_app.h"
#include "utils/utils_screen.h"
#include "utils/utils_language.h"
#include "utils/utils_os_macosdock.h"

#include <MobileUI.h>
#include <SharingUtils.h>
#include <singleapplication.h>

#include <QtGlobal>
#include <QLibraryInfo>
#include <QVersionNumber>

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSurfaceFormat>

#if defined(Q_OS_ANDROID)
#include "AndroidService.h"
#include "private/qandroidextras_p.h" // for QAndroidService
#endif

/* ************************************************************************** */

int main(int argc, char *argv[])
{
    // Arguments parsing ///////////////////////////////////////////////////////

    bool background_service = false;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i])
        {
            //qDebug() << "> arg >" << argv[i];

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
            if (!as) return EXIT_FAILURE;

            return app.exec();
        }

        return EXIT_SUCCESS;
#endif
    }

    // GUI application /////////////////////////////////////////////////////////

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // NVIDIA suspend&resume hack
    auto format = QSurfaceFormat::defaultFormat();
    format.setOption(QSurfaceFormat::ResetNotification);
    QSurfaceFormat::setDefaultFormat(format);
#endif

    SingleApplication app(argc, argv);

    // Application name
    app.setApplicationName("Theengs");
    app.setApplicationDisplayName("Theengs");
    app.setOrganizationName("Theengs");
    app.setOrganizationDomain("Theengs");

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    // Application icon
    QIcon appIcon(":/assets/logos/logo.png");
    app.setWindowIcon(appIcon);
#endif

    // Init components
    SettingsManager *sm = SettingsManager::getInstance();
    SystrayManager *st = SystrayManager::getInstance();
    NotificationManager *nm = NotificationManager::getInstance();
    MqttManager *mq = MqttManager::getInstance();
    DeviceManager *dm = new DeviceManager;
    if (!sm || !st || !nm || !mq || !dm)
    {
        qWarning() << "Cannot init Theengs components!";
        return EXIT_FAILURE;
    }

    // Start MQTT
    if (sm->getMQTT()) mq->connect();

    // Init generic utils
    UtilsScreen *utilsScreen = UtilsScreen::getInstance();
    UtilsApp *utilsApp = UtilsApp::getInstance();
    UtilsLanguage *utilsLanguage = UtilsLanguage::getInstance();
    if (!utilsScreen || !utilsApp || !utilsLanguage)
    {
        qWarning() << "Cannot init Theengs utils!";
        return EXIT_FAILURE;
    }

    // Translate the application
    utilsLanguage->loadLanguage(sm->getAppLanguage());

    // ThemeEngine
    qmlRegisterSingletonType(QUrl("qrc:/qml/ThemeEngine.qml"), "ThemeEngine", 1, 0, "Theme");

    MobileUI::registerQML();
    DeviceUtils::registerQML();
    DeviceUtilsTheengs::registerQML();

    // Then we start the UI
    QQmlApplicationEngine engine;
    QQmlContext *engine_context = engine.rootContext();

    engine_context->setContextProperty("deviceManager", dm);
    engine_context->setContextProperty("settingsManager", sm);
    engine_context->setContextProperty("systrayManager", st);
    engine_context->setContextProperty("mqttManager", mq);
    engine_context->setContextProperty("utilsApp", utilsApp);
    engine_context->setContextProperty("utilsLanguage", utilsLanguage);
    engine_context->setContextProperty("utilsScreen", utilsScreen);

    // Load the main view
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(FORCE_MOBILE_UI)
    ShareUtils *mShareUtils = new ShareUtils();
    engine_context->setContextProperty("utilsShare", mShareUtils);
    engine.load(QUrl(QStringLiteral("qrc:/qml/MobileApplication.qml")));
#else
    engine.load(QUrl(QStringLiteral("qrc:/qml/DesktopApplication.qml")));
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

    // Set systray?
    st->initSettings(&app, window);
    if (sm->getSysTray()) st->installSystray();

#if defined(Q_OS_MACOS)
    MacOSDockHandler *dockIconHandler = MacOSDockHandler::getInstance();
    QObject::connect(dockIconHandler, &MacOSDockHandler::dockIconClicked, window, &QQuickWindow::show);
    QObject::connect(dockIconHandler, &MacOSDockHandler::dockIconClicked, window, &QQuickWindow::raise);
#endif

#endif // desktop section

#if defined(Q_OS_ANDROID)
    QNativeInterface::QAndroidApplication::hideSplashScreen(333);
    if (sm->getSysTray()) AndroidService::service_start();
#endif

    return app.exec();
}

/* ************************************************************************** */
