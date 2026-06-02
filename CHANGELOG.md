# Changelog

All notable user-facing changes to **Theengs App** are documented in this file.
The format loosely follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.5.0] – 2026-05-18

### Added
- **SwitchBot control over BLE**:
  - **Bot S1 / SmartSwitch** (`SBS1` / `X1`) — On, Off, Push-Pull with selectable Press / Switch mode, inverted-direction toggle, configurable press duration.
  - **Curtain 2 / Curtain 3** (`SBCU` / `W070160X`) — Open, Close, Stop, move to position, High / Low speed.
  - **Blind Tilt** (`SBBT` / `W270160X`) — Open, Close up, Close down, Stop, tilt to position (-100…100 %).
- **Battery monitors**: BM2 and BM6 12 V monitor dongles with live voltage chart (0–20 V scale), state-of-charge / temperature on BM6, and a `BatteryPresetManager` so chemistry / nominal voltage / warning thresholds can be configured per device.
- **MQTT over TLS**: the Integration screen now exposes a `Use TLS` toggle, a custom CA `.pem` picker (for self-signed or private brokers), and a `Skip validation` escape hatch for hostname mismatches. An on-screen Activity panel surfaces TLS handshake errors.
- **Android foreground BLE-scan service**: scanning continues with the screen off via a persistent notification. The notification shows the latest reading and, when MQTT is enabled, the broker connection state and a drop counter for failed publishes. Runtime `POST_NOTIFICATIONS` request is handled on first launch.
- **MQTT-down banner**: when the broker link is lost while MQTT is enabled, the device list shows a collapsing "MQTT not connected — N readings lost since HH:MM" banner with a Retry button, mirroring the drop counter already shown in the Android foreground-service notification.
- **Consolidated Background updates page (Android)**: the "Enable background updates" toggle now opens a dedicated screen that walks through the three permissions needed for reliable refresh — background location, exact alarms (API 31+), and battery-optimisation exemption — each with a one-tap grant button and a live ✓ / ✗ indicator.
- **Exact AllowWhileIdle background scheduler (Android)**: the foreground service is now driven by a `setExactAndAllowWhileIdle` `AlarmManager` alarm (gated on `canScheduleExactAlarms()`) instead of a `QTimer`, so the next scan window fires on time through deep doze when "Exact alarms" and "Battery optimisation" are granted. A 65 s partial wakelock keeps the CPU up across each scan window.
- **HA gateway auto-discovery**: the app now publishes its own Home Assistant discovery payload so it appears named in Home Assistant alongside the sensors it forwards.
- **History chart caption**: sampling interval is now shown under each in-app history chart.
- **BBQ device support**: Govee H5055 (multi-probe) and SOLIS-6 added to the BBQ live-monitoring list.
- **Window-cover actuators**: preliminary support with a `-100..100` tilt range.

### Changed
- **Decoder bump**: Theengs Decoder v2.1.0 (from v1.9.5 era). Some `model_id` strings are now consolidated (e.g. `H5072/75`, `H5100/01/02/04/74/77`, `IBT-6XS/SOLIS-6`); legacy IDs remain recognised.
- **Build system**: project switched from `qmake` to **CMake** ≥ 3.20. Minimum **Qt 6.8** with `qtconnectivity`, `qtcharts`, `qtwebsockets`, `qt5compat`, image formats. New CMake flags `ENABLE_MQTT`, `ENABLE_OPENSSL`, `ENABLE_MBEDTLS`. `QtMqtt` is now optional at build time.
- **Permissions**: Android permission flow rewritten; iOS adds `NSLocationAlwaysAndWhenInUseUsageDescription`.
- **MQTT topics**: when a sensor has no usable MAC address the topic now falls back to a stable identifier instead of `BTtoMQTT//…`.
- **Generic widget**: numeric values are formatted per type and meta-units are hidden.

### Fixed
- H5055 multi-probe state wipe (decoder #92).
- Govee thermometers occasionally labelled as `iBeacon` after the advertisement-parsing rework.
- Android decode for payload-embedded MAC rules (TPMS, VCH6003, …).
- BM6 voltage scaling and detection on iOS.
- BM2 bonding on Android and BM2 real-time voltage graphs.
- Foreground-service notification tap on slow devices.
- HA discovery now publishes all numeric properties (previously some were dropped by the validator).
- **Background scan cadence (Android)**: the `Update interval` setting now actually controls how often the background service scans. Previously the listen loop self-restarted every ~60 s regardless of the slider; the service now opens one ~60 s window per interval and idles in between (device-verified on LG V30 / Android 9 and Samsung S10e / Android 12). The picker is hidden on desktop, which always scans continuously.
- **device_bm26 builds with `-DENABLE_MBEDTLS=OFF`**: the AES-CBC block in `serviceDetailsDiscovered_volt()` is now wrapped in the same `ENABLE_MBEDTLS` guard as the rest of the file, so the encryption helper can be compiled out cleanly.

### Security
- mbedTLS 3.6.5 bundled in `contribs/` for the encrypted-advertisement decoders.
- TLS support in the MQTT bridge unblocks public-CA brokers (HiveMQ Cloud, AWS IoT) without requiring a VPN.
- **Credential storage**: MQTT and MySQL passwords are now persisted in the OS keychain via QtKeychain 0.15.0 (Apple Keychain, Windows Credential Store, libsecret / KWallet on Linux, Android Keystore) instead of plaintext `QSettings`; a one-time migration moves any legacy plaintext value. Shipped default credentials (`theengs` / `theengs`) removed. Gated by the new `ENABLE_SECURE_STORAGE` CMake option (default ON).

## [1.4.1] – 2024-09-03

Internal prep release; see `git log v1.4.0..8452ae2` for details.

## [1.4.0] – 2024

Last tagged release before 1.5.0. See `git tag` and the GitHub release notes for the full history of earlier versions.
