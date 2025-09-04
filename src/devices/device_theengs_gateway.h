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

#ifndef DEVICE_SE_GATEWAY_H
#define DEVICE_SE_GATEWAY_H
/* ************************************************************************** */

#include "../device.h"

#include <QObject>
#include <QString>
#include <QStringLiteral>
#include <QList>
#include <QTimer>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QBluetoothUuid>

/* ************************************************************************** */

enum GatewayState {
    WAITING_ONBOARDING = 0,
    ONBOARDING, // 1
    OFFLINE, // 2
    NTWK_CONNECTED, // 3
    BROKER_CONNECTED, // 4
    PROCESSING, // 5
    NTWK_DISCONNECTED, // 6
    BROKER_DISCONNECTED,
    LOCAL_OTA_IN_PROGRESS,
    REMOTE_OTA_IN_PROGRESS,
    SLEEPING,
    ERROR
};

/* ************************************************************************** */

class Network: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString ssid READ getSSID CONSTANT)
    Q_PROPERTY(int rssi READ getRSSI CONSTANT)
    Q_PROPERTY(int strength READ getStrength CONSTANT)
    Q_PROPERTY(bool open READ getOpen CONSTANT)

    QString m_ssid;
    int m_rssi = 0;
    bool m_open = false;

    QString getSSID() const { return m_ssid; }
    int getRSSI() const { return m_rssi; }
    int getStrength() const {
        if  (m_rssi >= -50) return 4;
        if  (m_rssi >= -60) return 3;
        if  (m_rssi >= -70) return 2;
        if  (m_rssi >= -90) return 1;
        return 0;
    }
    bool getOpen() const { return m_open; }

public:
    Network(QString ssid, int rssi, bool open, QObject *parent) : QObject(parent) {
        m_ssid = ssid; m_rssi = rssi; m_open = open;
    }
    ~Network() = default;
};

/* ************************************************************************** */

class GatewaySetupSession
{
public:

    /// BluFi

    int blufi_seq_nb = 0;

    int getSequenceNumber() {
        return blufi_seq_nb++;
    }

    /// User credentials

    QString gateway_name;
    QString gateway_pass;

    QString wifi_ssid;
    QString wifi_pass;

    QString mqtt_server;
    int mqtt_port = 0;
    QString mqtt_user;
    QString mqtt_pass;
    QString mqtt_topicA;
    QString mqtt_topicB;
    bool mqtt_validate = false;
    bool mqtt_secure = false;

    void resetCredentials() {
        gateway_name.clear();
        gateway_pass.clear();

        wifi_ssid.clear();
        wifi_pass.clear();

        mqtt_server.clear();
        mqtt_port = 0;
        mqtt_user.clear();
        mqtt_pass.clear();
        mqtt_topicA.clear();
        mqtt_topicB.clear();
    }

    /// Onboarding sequence

    static const int s_timout_duration = 20000;

    int sequence_status = 0; // -2: timeout  -1: errored // 0: disconnected // 1: running // 2: finished
    int sequence_retry = 0;

    int ble_status = 0; // -1: errored // 0: disconnected // 1: connecting // 2: connected
    int mqtt_status = 0; // -1: errored // 0: disconnected // 1: connecting // 2: connected
    int wifi_status = 0; // -1: errored // 0: disconnected // 1: connecting // 2: connected, no IP // 3: connected

    bool ble_connected = false;
    bool ble_authenticated = false;

    bool mqtt_settings_sent = false;
    bool mqtt_connected = false;
    bool wifi_settings_sent = false;
    bool wifi_connected = false;

    bool ble_errored = false;
    bool mqtt_errored = false;
    bool wifi_errored = false;

    QString ble_error;
    QString mqtt_error;
    QString wifi_error;

    ///

    void startSequence() {
        blufi_seq_nb = 0;

        sequence_status = 0;
        sequence_retry = 0;

        ble_connected = false;
        ble_authenticated = false;

        mqtt_settings_sent = false;
        mqtt_connected = false;
        wifi_settings_sent = false;
        wifi_connected = false;

        ble_status = 0;
        mqtt_status = 0;
        wifi_status = 0;

        ble_errored = false;
        mqtt_errored = false;
        wifi_errored = false;

        ble_error.clear();
        mqtt_error.clear();
        wifi_error.clear();
    }

    void retrySequence() {
        blufi_seq_nb = 0;

        sequence_status = 0;
        sequence_retry++;

        ble_status = 0;
        mqtt_status = 0;
        wifi_status = 0;

        ble_errored = false;
        mqtt_errored = false;
        wifi_errored = false;

        ble_error.clear();
        mqtt_error.clear();
        wifi_error.clear();
    }
};

/* ************************************************************************** */

/*!
 * DeviceGateway
 * BLE name: 'OMG_XXXXXXXXXXXX'
 *
 * BluFi Service UUID: 0xFFFF, 16 bit
 * BluFi Characteristic (device > ESP32): 0xFF01 (write)
 * Blufi Characteristic (ESP32 > device): 0xFF02 (read / notify)
 *
 * - https://docs.openmqttgateway.com/use/gateway.html
 */
class DeviceGateway: public Device
{
    Q_OBJECT

    Q_PROPERTY(bool onboarded READ isOnboarded NOTIFY onboardedUpdated)
    Q_PROPERTY(int onboardingStatus READ getOnboardingStatus NOTIFY onboardedUpdated)

    Q_PROPERTY(bool bleConnected READ isBleConnected NOTIFY sequenceUpdated)
    Q_PROPERTY(bool mqttSet READ isMqttSet NOTIFY sequenceUpdated)
    Q_PROPERTY(bool mqttConnected READ isMqttConnected NOTIFY sequenceUpdated)
    Q_PROPERTY(bool wifiSet READ isWifiSet NOTIFY sequenceUpdated)
    Q_PROPERTY(bool wifiConnected READ isWifiConnected NOTIFY sequenceUpdated)

    Q_PROPERTY(int sequenceStatus READ getSequenceStatus NOTIFY sequenceUpdated)
    Q_PROPERTY(bool sequenceFinished READ isSequenceFinished NOTIFY sequenceUpdated)

    Q_PROPERTY(int bleStatus READ getStatus NOTIFY statusUpdated)
    Q_PROPERTY(bool bleErrored READ isBleErrored NOTIFY sequenceUpdated)
    Q_PROPERTY(int mqttStatus READ getMqttStatus NOTIFY statusUpdated)
    Q_PROPERTY(bool mqttErrored READ isMqttErrored NOTIFY sequenceUpdated)
    Q_PROPERTY(int wifiStatus READ getWifiStatus NOTIFY sequenceUpdated)
    Q_PROPERTY(bool wifiErrored READ isWifiErrored NOTIFY sequenceUpdated)

    Q_PROPERTY(bool networksRefreshing READ getNetworksRefreshing NOTIFY networksUpdated)
    Q_PROPERTY(QVariant networksAvailable READ getNetworksAvailable NOTIFY networksUpdated)

    bool isBleConnected() { return m_setupSession.ble_connected; }
    bool isMqttSet() { return m_setupSession.mqtt_settings_sent; }
    bool isMqttConnected() { return m_setupSession.mqtt_connected; }
    bool isWifiSet() { return m_setupSession.wifi_settings_sent; }
    bool isWifiConnected() { return m_setupSession.wifi_connected; }

    bool isSequenceFarEnough() { return m_setupSession.wifi_connected; }
    bool isSequenceFinished() { return (m_setupSession.sequence_status == 2); }
    int getSequenceStatus() { return m_setupSession.sequence_status; }

    bool isBleErrored() { return m_setupSession.ble_errored; }
    bool isMqttErrored() { return m_setupSession.mqtt_errored; }
    int getMqttStatus() { return m_setupSession.mqtt_status; }
    bool isWifiErrored() { return m_setupSession.wifi_errored; }
    int getWifiStatus() { return m_setupSession.wifi_status; }

    bool m_onboarded = false;
    bool isOnboarded() const { return m_onboarded; }

    int m_onboardingStatus = 0;
    int getOnboardingStatus() const { return m_onboardingStatus; }

    QString m_password;

    GatewaySetupSession m_setupSession;
    QTimer m_setupTimer;

    QString m_mac_ble;
    QString m_mac_wifi;

    bool m_networksRefreshing = false;
    bool getNetworksRefreshing() const { return m_networksRefreshing; }
    QList <Network *> m_networksAvailable;
    QVariant getNetworksAvailable() const { return QVariant::fromValue(m_networksAvailable); }

private:
    QBluetoothUuid uuid_bluefi_srv = QBluetoothUuid(QStringLiteral("0000ffff-0000-1000-8000-00805f9b34fb"));
    QBluetoothUuid uuid_bluefi_char_write = QBluetoothUuid(QStringLiteral("0000ff01-0000-1000-8000-00805f9b34fb"));
    QBluetoothUuid uuid_bluefi_char_read = QBluetoothUuid(QStringLiteral("0000ff02-0000-1000-8000-00805f9b34fb"));

private:
    // QLowEnergyController related

    void serviceScanDone();
    void addLowEnergyService(const QBluetoothUuid &uuid);
    void serviceDetailsDiscovered_blufi(QLowEnergyService::ServiceState newState);
    bool areWeReadyYet();

    QLowEnergyService *m_serviceBluFi = nullptr;

    QLowEnergyCharacteristic m_charWrite;
    QLowEnergyCharacteristic m_charRead;
    QLowEnergyDescriptor m_notificationDesc;

    void bleWriteDone(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void bleReadDone(const QLowEnergyCharacteristic &c, const QByteArray &value);
    void bleReadNotify(const QLowEnergyCharacteristic &c, const QByteArray &value);

    void descriptorRead(const QLowEnergyDescriptor &d, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &d, const QByteArray &value);

    void bleError(QLowEnergyService::ServiceError error);

protected:
    virtual bool getSqlDeviceInfos();

    virtual void deviceErrored(QLowEnergyController::Error error);
    virtual void deviceDisconnected();
    virtual void deviceConnected();
    virtual void deviceReady();

signals:
    void onboardedUpdated();
    void networksUpdated();
    void sequenceUpdated();
    void sequenceCompleted();

private slots:
    void startHandshake();

    void setSeqMode();
    void getVersion();
    void getWifiState();
    void getWifiList();
    void setWifiCredentials();
    void setWifiConnect();
    void setWifiDisconnect();

    void setCustomData();
    void setCustomData_getStatus();
    void setCustomData_setPassword();
    void setCustomData_setMqtt();
    void setCustomData_restart();
    void setCustomData_erase();

    bool checkWiFi_state(const uint8_t status);
    bool checkWiFi_list(const QByteArray &data);

    void retrySequence_internal();
    void abortSequence_internal();
    void timeoutSequence_internal();
    void finishSequence_internal();

public slots:
    void gatewayCreationResponse(const QString &context, bool created); // remove ?

public:
    DeviceGateway(const QString &deviceAddr, const QString &deviceName, QObject *parent = nullptr);
    DeviceGateway(const QBluetoothDeviceInfo &info, QObject *parent = nullptr);
    ~DeviceGateway();

    static QString bleMac_from_wifiMac(const QString &wifi_mac); // remove?

    void parseAdvertisementData(const uint16_t adv_mode, const uint16_t adv_id, const QByteArray &value);

    void setName(const QString &name);

    Q_INVOKABLE void setGatewayCredentials(const QString &password);
    Q_INVOKABLE void setWifiCredentials(const QString &ssid, const QString &password);
    Q_INVOKABLE void setMqttCredentials(const QString &host, const int port,
                                        const QString &user, const QString &password,
                                        const QString &topicA, const QString &topicB);

    Q_INVOKABLE void startOnboarding(); // remove?
    Q_INVOKABLE void retrySequence(); // remove?
    Q_INVOKABLE void createGateway(); // remove?

    //Q_INVOKABLE void startPasswordUpdate();
    Q_INVOKABLE void startWifiUpdate();
    //Q_INVOKABLE void startMqttUpdate();
    //Q_INVOKABLE void startSettingsUpdate();

    Q_INVOKABLE void actionPasswordSet();

    Q_INVOKABLE void actionWifiScan();
    Q_INVOKABLE void actionWifiSet();
    Q_INVOKABLE void actionWifiConnect();
    Q_INVOKABLE void actionWifiDisconnect();

    Q_INVOKABLE void actionMqttSet();

    Q_INVOKABLE void actionStatus();
    Q_INVOKABLE void actionErase();
    Q_INVOKABLE void actionRestart();
};

/* ************************************************************************** */
#endif // DEVICE_SE_GATEWAY_H
