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

#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H
/* ************************************************************************** */

#define BACKGROUND_UPDATE_INTERVAL    5 // minutes
#define PLANT_UPDATE_INTERVAL       180 // minutes
#define THERMO_UPDATE_INTERVAL       60 // minutes
#define ERROR_UPDATE_INTERVAL        60 // minutes

#include <QObject>
#include <QString>
#include <QSize>

/* ************************************************************************** */

/*!
 * \brief The SettingsManager class
 */
class SettingsManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool firstLaunch READ isFirstLaunch NOTIFY firstLaunchChanged)

    Q_PROPERTY(QSize initialSize READ getInitialSize NOTIFY initialSizeChanged)
    Q_PROPERTY(QSize initialPosition READ getInitialPosition NOTIFY initialSizeChanged)
    Q_PROPERTY(uint initialVisibility READ getInitialVisibility NOTIFY initialSizeChanged)

    Q_PROPERTY(QString appTheme READ getAppTheme WRITE setAppTheme NOTIFY appThemeChanged)
    Q_PROPERTY(bool appThemeAuto READ getAppThemeAuto WRITE setAppThemeAuto NOTIFY appThemeAutoChanged)
    Q_PROPERTY(bool appThemeCSD READ getAppThemeCSD WRITE setAppThemeCSD NOTIFY appThemeCSDChanged)
    Q_PROPERTY(uint appUnits READ getAppUnits WRITE setAppUnits NOTIFY appUnitsChanged)
    Q_PROPERTY(QString appLanguage READ getAppLanguage WRITE setAppLanguage NOTIFY appLanguageChanged)

    Q_PROPERTY(bool systray READ getSysTray WRITE setSysTray NOTIFY systrayChanged)
    Q_PROPERTY(bool notifications READ getNotifs WRITE setNotifs NOTIFY notifsChanged)
    Q_PROPERTY(bool minimized READ getMinimized WRITE setMinimized NOTIFY minimizedChanged)
    Q_PROPERTY(bool bluetoothControl READ getBluetoothControl WRITE setBluetoothControl NOTIFY bluetoothControlChanged)
    Q_PROPERTY(bool bluetoothLimitScanningRange READ getBluetoothLimitScanningRange WRITE setBluetoothLimitScanningRange NOTIFY bluetoothLimitScanningRangeChanged)
    Q_PROPERTY(uint bluetoothSimUpdates READ getBluetoothSimUpdates WRITE setBluetoothSimUpdates NOTIFY bluetoothSimUpdatesChanged)
    Q_PROPERTY(uint updateIntervalBackground READ getUpdateIntervalBackground WRITE setUpdateIntervalBackground NOTIFY updateIntervalBackgroundChanged)
    Q_PROPERTY(uint updateIntervalPlant READ getUpdateIntervalPlant WRITE setUpdateIntervalPlant NOTIFY updateIntervalPlantChanged)
    Q_PROPERTY(uint updateIntervalThermo READ getUpdateIntervalThermo WRITE setUpdateIntervalThermo NOTIFY updateIntervalThermoChanged)
    Q_PROPERTY(QString orderBy READ getOrderBy WRITE setOrderBy NOTIFY orderByChanged)
    Q_PROPERTY(QString tempUnit READ getTempUnit NOTIFY appUnitsChanged) // READ ONLY
    Q_PROPERTY(QString graphHistory READ getGraphHistogram WRITE setGraphHistogram NOTIFY graphHistogramChanged)
    Q_PROPERTY(QString graphThermometer READ getGraphThermometer WRITE setGraphThermometer NOTIFY graphThermometerChanged)
    Q_PROPERTY(bool graphShowDots READ getGraphShowDots WRITE setGraphShowDots NOTIFY graphShowDotsChanged)
    Q_PROPERTY(bool compactView READ getCompactView WRITE setCompactView NOTIFY compactViewChanged)
    Q_PROPERTY(bool bigIndicator READ getBigIndicator WRITE setBigIndicator NOTIFY bigIndicatorChanged)
    Q_PROPERTY(bool dynaScale READ getDynaScale WRITE setDynaScale NOTIFY dynaScaleChanged)

    Q_PROPERTY(bool mysql READ getMySQL WRITE setMySQL NOTIFY mysqlChanged)
    Q_PROPERTY(QString mysqlHost READ getMysqlHost WRITE setMysqlHost NOTIFY mysqlChanged)
    Q_PROPERTY(uint mysqlPort READ getMysqlPort WRITE setMysqlPort NOTIFY mysqlChanged)
    Q_PROPERTY(QString mysqlUser READ getMysqlUser WRITE setMysqlUser NOTIFY mysqlChanged)
    Q_PROPERTY(QString mysqlPassword READ getMysqlPassword WRITE setMysqlPassword NOTIFY mysqlChanged)

    Q_PROPERTY(bool mqtt READ getMQTT WRITE setMQTT NOTIFY mqttChanged)
    Q_PROPERTY(QString mqttHost READ getMqttHost WRITE setMqttHost NOTIFY mqttChanged)
    Q_PROPERTY(uint mqttPort READ getMqttPort WRITE setMqttPort NOTIFY mqttChanged)
    Q_PROPERTY(QString mqttUser READ getMqttUser WRITE setMqttUser NOTIFY mqttChanged)
    Q_PROPERTY(QString mqttPassword READ getMqttPassword WRITE setMqttPassword NOTIFY mqttChanged)
    Q_PROPERTY(QString mqttTopicA READ getMqttTopicA WRITE setMqttTopicA NOTIFY mqttChanged)
    Q_PROPERTY(QString mqttTopicB READ getMqttTopicB WRITE setMqttTopicB NOTIFY mqttChanged)

    bool m_firstlaunch = true;

    // Application window
    QSize m_appSize;
    QSize m_appPosition;
    unsigned m_appVisibility = 1;               //!< QWindow::Visibility

    // Application generic
    QString m_appTheme = "THEME_THEENGS";
    bool m_appThemeAuto = false;
    bool m_appThemeCSD = false;
    unsigned m_appUnits = 0;                    //!< QLocale::MeasurementSystem
    QString m_appLanguage = "auto";

    // Application specific
    bool m_startMinimized = false;
    bool m_systrayEnabled = false;
    bool m_notificationsEnabled = false;

    bool m_bluetoothControl = false;
    bool m_bluetoothLimitScanningRange = false;
    unsigned m_bluetoothSimUpdates = 3;

    unsigned m_updateIntervalBackground = BACKGROUND_UPDATE_INTERVAL;
    unsigned m_updateIntervalPlant = PLANT_UPDATE_INTERVAL;
    unsigned m_updateIntervalThermo = THERMO_UPDATE_INTERVAL;
    QString m_graphHistogram = "weekly";
    QString m_graphThermometer = "minmax";
    bool m_graphShowDots = true;
    bool m_compactView = true;
    bool m_bigIndicator = false;
    bool m_dynaScale = true;
    QString m_orderBy = "model";

    bool m_mysql = false;
    QString m_mysqlHost;
    int m_mysqlPort = 3306;
    QString m_mysqlName = "theengs";
    QString m_mysqlUser = "theengs";
    QString m_mysqlPassword = "theengs";

    bool m_mqtt = false;
    QString m_mqttHost;
    int m_mqttPort = 1883;
    QString m_mqttName = "theengs";
    QString m_mqttUser = "theengs";
    QString m_mqttPassword = "theengs";
    QString m_mqttTopicA = "home";
    QString m_mqttTopicB = "TheengsApp";

    // Singleton
    static SettingsManager *instance;
    SettingsManager();
    ~SettingsManager();

    bool readSettings();
    bool writeSettings();

Q_SIGNALS:
    void firstLaunchChanged();
    void initialSizeChanged();
    void appThemeChanged();
    void appThemeAutoChanged();
    void appThemeCSDChanged();
    void appUnitsChanged();
    void appLanguageChanged();
    void minimizedChanged();
    void systrayChanged();
    void notifsChanged();
    void bluetoothControlChanged();
    void bluetoothSimUpdatesChanged();
    void bluetoothLimitScanningRangeChanged();
    void updateIntervalBackgroundChanged();
    void updateIntervalPlantChanged();
    void updateIntervalThermoChanged();
    void graphHistogramChanged();
    void graphThermometerChanged();
    void graphShowDotsChanged();
    void compactViewChanged();
    void bigIndicatorChanged();
    void dynaScaleChanged();
    void orderByChanged();
    void mysqlChanged();
    void mqttChanged();

public:
    static SettingsManager *getInstance();

    bool isFirstLaunch() const { return m_firstlaunch; }

    QSize getInitialSize() { return m_appSize; }
    QSize getInitialPosition() { return m_appPosition; }
    unsigned getInitialVisibility() { return m_appVisibility; }

    QString getAppTheme() const { return m_appTheme; }
    void setAppTheme(const QString &value);

    bool getAppThemeAuto() const { return m_appThemeAuto; }
    void setAppThemeAuto(const bool value);

    bool getAppThemeCSD() const { return m_appThemeCSD; }
    void setAppThemeCSD(const bool value);

    unsigned getAppUnits() const { return m_appUnits; }
    void setAppUnits(unsigned value);

    QString getAppLanguage() const { return m_appLanguage; }
    void setAppLanguage(const QString &value);

    bool getMinimized() const { return m_startMinimized; }
    void setMinimized(const bool value);

    bool getSysTray() const { return m_systrayEnabled; }
    void setSysTray(const bool value);

    bool getNotifs() const { return m_notificationsEnabled; }
    void setNotifs(const bool value);

    bool getBluetoothControl() const { return m_bluetoothControl; }
    void setBluetoothControl(const bool value);

    bool getBluetoothLimitScanningRange() const { return m_bluetoothLimitScanningRange; }
    void setBluetoothLimitScanningRange(const bool value);

    unsigned getBluetoothSimUpdates() const { return m_bluetoothSimUpdates; }
    void setBluetoothSimUpdates(const unsigned value);

    unsigned getUpdateIntervalBackground() const { return m_updateIntervalBackground; }
    void setUpdateIntervalBackground(const unsigned value);

    unsigned getUpdateIntervalPlant() const { return m_updateIntervalPlant; }
    void setUpdateIntervalPlant(const unsigned value);

    unsigned getUpdateIntervalThermo() const { return m_updateIntervalThermo; }
    void setUpdateIntervalThermo(const unsigned value);

    QString getTempUnit() const {
        if (m_appUnits == 0) return "C";
        return "F";
    }

    QString getGraphHistogram() const { return m_graphHistogram; }
    void setGraphHistogram(const QString &value);

    QString getGraphThermometer() const { return m_graphThermometer; }
    void setGraphThermometer(const QString &value);

    bool getGraphShowDots() const { return m_graphShowDots; }
    void setGraphShowDots(const bool value);

    bool getCompactView() const { return m_compactView; }
    void setCompactView(const bool value);

    bool getBigIndicator() const { return m_bigIndicator; }
    void setBigIndicator(const bool value);

    bool getDynaScale() const { return m_dynaScale; }
    void setDynaScale(const bool value);

    QString getOrderBy() const { return m_orderBy; }
    void setOrderBy(const QString &value);

    bool getMySQL() const { return m_mysql; }
    void setMySQL(const bool value);

    QString getMysqlHost() const { return m_mysqlHost; }
    void setMysqlHost(const QString &value);

    int getMysqlPort() const { return m_mysqlPort; }
    void setMysqlPort(const int value);

    QString getMysqlName() const { return m_mysqlName; }
    void setMysqlName(const QString &value);

    QString getMysqlUser() const { return m_mysqlUser; }
    void setMysqlUser(const QString &value);

    QString getMysqlPassword() const { return m_mysqlPassword; }
    void setMysqlPassword(const QString &value);

    bool getMQTT() const { return m_mqtt; }
    void setMQTT(const bool value);

    QString getMqttHost() const { return m_mqttHost; }
    void setMqttHost(const QString &value);

    int getMqttPort() const { return m_mqttPort; }
    void setMqttPort(const int value);

    QString getMqttName() const { return m_mqttName; }
    void setMqttName(const QString &value);

    QString getMqttUser() const { return m_mqttUser; }
    void setMqttUser(const QString &value);

    QString getMqttPassword() const { return m_mqttPassword; }
    void setMqttPassword(const QString &value);

    QString getMqttTopicA() const { return m_mqttTopicA; }
    void setMqttTopicA(const QString &value);

    QString getMqttTopicB() const { return m_mqttTopicB; }
    void setMqttTopicB(const QString &value);

    // Utils
    Q_INVOKABLE void reloadSettings();
    Q_INVOKABLE void resetSettings();
};

/* ************************************************************************** */
#endif // SETTINGS_MANAGER_H
