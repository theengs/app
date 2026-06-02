# Build

Theengs App is built with **CMake** (≥ 3.20) and **Qt 6.8+**. The qmake project file kept around in older revisions is no longer the supported entry point.

## Dependencies

You will need a C++17 compiler and Qt 6.8 or newer.

* **Linux** — GCC 11+ or Clang 14+, plus `libsecret-1-dev` for keychain-backed credential storage
* **macOS / iOS** — Xcode 15+ (Xcode with the iOS 26 SDK is required for App Store submissions)
* **Windows** — MSVC 2019+
* **Android** — JDK 17, Android SDK 36, NDK 26+. The build environment can be customised through `assets/android/gradle.properties`.

### Qt 6 additional modules

Make sure the following are installed alongside Qt 6 base:

* Qt 5 Compatibility Module
* Qt Connectivity
* Qt Charts
* Qt Image Formats
* Qt WebSockets

`QtMqtt` and an optionally-patched `QtConnectivity` (with Theengs filtering improvements for Android) are built from source by the `contribs` helper — see below.

## Clone the repository

```bash
git clone https://github.com/theengs/app.git TheengsApp --recurse-submodules
cd TheengsApp
```

## Build the bundled Qt dependencies

The `contribs` script compiles QtMqtt (required if MQTT support is enabled) and a patched QtConnectivity.

```bash
# Desktop (Linux example)
cd contribs/
python3 contribs_builder_qt.py \
    --targets=linux \
    --software qtmqtt \
    --qt-directory /path/to/Qt \
    --qt-version 6.10.1

# Android
python3 contribs_builder_qt.py \
    --targets=android_armv8 \
    --software qtmqtt qtconnectivity \
    --qt-directory /path/to/Qt \
    --qt-version 6.10.1

# iOS
python3 contribs_builder_qt.py \
    --targets=ios \
    --software qtmqtt qtconnectivity \
    --qt-directory /path/to/Qt \
    --qt-version 6.10.1

cd ..
```

The available `--targets` are `linux`, `macos`, `windows`, `android_armv7`, `android_armv8`, `android_x86`, `android_x86_64`, `ios`.

## Build Theengs App

```bash
cmake -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/ --config Release
```

### CMake options

| Option | Default | Description |
|---|---|---|
| `ENABLE_MQTT` | `ON` | Build the MQTT bridge (requires QtMqtt). |
| `ENABLE_OPENSSL` | `ON` | Use OpenSSL for TLS / payload decryption. |
| `ENABLE_MBEDTLS` | `ON` | Bundle mbedTLS 3.6.5 (used by some encrypted-advertisement decoders). |
| `ENABLE_SECURE_STORAGE` | `ON` | Store MQTT / MySQL passwords in the OS keychain (Apple Keychain, Windows Credential Store, libsecret / KWallet on Linux, Android Keystore) via the bundled QtKeychain 0.15.0 submodule at `thirdparty/qtkeychain`. |

Disable MQTT-related work with `-DENABLE_MQTT=OFF` if you don't need the bridge.

## Deployment helpers

Release artifacts can be produced through the provided scripts:

```bash
./deploy_linux.sh -p     # Linux AppImage
./deploy_macos.sh -p     # macOS .app / DMG
./deploy_windows.sh -p   # Windows installer
```

Android and iOS deployment uses the standard Qt for Android / Xcode flows. CI in `.github/workflows/builds_mobile.yml` shows the canonical signing path for AAB and IPA artifacts.
