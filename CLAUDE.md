# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Theengs is a cross-platform BLE (Bluetooth Low Energy) sensor monitoring application built with Qt 6.8 and C++17. It reads data from 100+ BLE sensor types (temperature, humidity, motion, scales, probes, etc.) using the Theengs Decoder library and can publish data to MQTT brokers for smart home integration (Home Assistant, OpenHAB, etc.).

**Platforms**: Linux, macOS, Windows, Android (9+), iOS (16+)

## Build System

The project uses **CMake** as the primary build system (minimum version 3.20).

### Building the Application

**Desktop (Linux/macOS/Windows):**
```bash
# Install Qt 6.8+ with modules: qtconnectivity, qtcharts, qtwebsockets

# Build QtMqtt dependency first (required)
cd contribs/
python3 contribs_builder_qt.py --targets=linux --software qtmqtt --qt-directory /path/to/Qt --qt-version 6.10.1
cd ..

# Build application
cmake -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/ --config Release

# Optional: Create deployment package
./deploy_linux.sh -p    # Linux AppImage
./deploy_macos.sh -p    # macOS bundle
./deploy_windows.sh -p  # Windows installer
```

**Android:**
```bash
cd contribs/
python3 contribs_builder_qt.py --targets=android_armv8 --software qtmqtt qtconnectivity --qt-directory /path/to/Qt --qt-version 6.10.1
cd ..

# Use Qt Creator or command-line build with Android toolchain
```

**iOS:**
```bash
cd contribs/
python3 contribs_builder_qt.py --targets=ios --software qtmqtt qtconnectivity --qt-directory /path/to/Qt --qt-version 6.10.1
cd ..

# Open Theengs.xcodeproj or use Qt Creator
```

### CMake Build Options

- `ENABLE_MQTT` (default: ON) - Enable MQTT broker support
- `ENABLE_OPENSSL` (default: ON) - OpenSSL for encryption
- `ENABLE_MBEDTLS` (default: ON) - mbedTLS for encryption

## Architecture

### High-Level Design Pattern

**Qt MVC with C++ Backend + QML Frontend**

- **Model (C++)**: Device classes, Manager singletons, data models
- **View (QML)**: UI components in `qml/` directory
- **Controller**: Qt signal/slot mechanism bridging C++ and QML

### Application Entry Points

Two distinct UI modes based on platform:
- **Desktop**: `qml/DesktopApplication.qml` - menubar, systray, multi-window support
- **Mobile**: `qml/MobileApplication.qml` - drawer navigation, mobile-optimized layouts

C++ initialization in `src/main.cpp` (261 lines):
- Creates singleton managers
- Exposes C++ objects to QML via context properties
- Conditionally loads Desktop or Mobile UI
- Android background service support

### Core Singleton Managers

All managers follow the singleton pattern and are exposed to QML as context properties:

- **DeviceManager** (`deviceManager`): BLE scanning, device lifecycle, batch operations
- **SettingsManager** (`settingsManager`): Application configuration persistence
- **DatabaseManager**: SQLite (local) and MySQL (remote) database connections
- **MqttManager** (`mqttManager`): MQTT broker connectivity and publishing
- **PermissionManager**: Platform-specific permission handling
- **NotificationManager** (`notificationManager`): Cross-platform notifications
- **BatteryPresetManager** / **TempPresetManager**: Device configuration presets

### Device Class Hierarchy

Polymorphic inheritance hierarchy for device types:

```
Device (base class in src/device.h)
├── DeviceSensor (src/device_sensor.h)
│   ├── DevicePlantSensor (src/device_plantsensor.h)
│   ├── DeviceThermometer (src/device_thermometer.h)
│   ├── DeviceEnvironmental (src/device_environmental.h)
│   └── DeviceTheengs (src/device_theengs.h)
│       ├── DeviceTheengsGeneric
│       ├── DeviceTheengsProbes
│       ├── DeviceTheengsScales
│       ├── DeviceTheengsThermometers
│       ├── DeviceTheengsMotionSensors
│       ├── DeviceTheengsBatteryMonitors
│       ├── DeviceTheengsActuators
│       └── [40+ device-specific implementations in src/devices/]
```

**Device types are defined in `src/device_utils.h`** with enums distinguishing legacy devices from Theengs unified device types.

### BLE Data Flow

```
BLE Advertisement (Qt Bluetooth)
    ↓
QBluetoothDeviceDiscoveryAgent::deviceUpdated() signal
    ↓
DeviceManager::bleDevice_updated() slot
    ↓
DeviceManager_advertisement.cpp::parseAdvertisementData()
    ↓
TheengsDecoder::decodeBLEJson() (thirdparty/TheengsDecoder/)
    ↓
Returns: JSON with decoded sensor data + model_id
    ↓
DeviceManager_theengs.cpp creates appropriate Device subclass
    ↓
Device::parseAdvertisementData() updates properties
    ↓
Device emits dataUpdated() signal
    ↓
QML UI updates via property bindings
    ↓
Optional: publishMQTT() + saveSQLData()
```

### DeviceManager Split Architecture

DeviceManager functionality is split across multiple files:

- **DeviceManager.cpp** (2,800+ lines): Core device management, BLE scanning, device queuing
- **DeviceManager_advertisement.cpp**: Raw BLE advertisement parsing
- **DeviceManager_theengs.cpp** (1,500+ lines): Theengs decoder integration, device instantiation
- **DeviceManager_gateway.cpp**: Remote gateway device discovery
- **DeviceManager_nearby.cpp**: Nearby device filtering and RSSI management

### Database Architecture

Two-tier database support:

1. **SQLite** (primary): Local storage at `~/.local/share/Theengs/Theengs.db`
2. **MySQL** (optional): Remote database for multi-device setups

**DatabaseManager** (`src/DatabaseManager.cpp`):
- Tries MySQL first, falls back to SQLite
- Schema version management (current: v3)
- Automatic migration system for schema upgrades

**Data Stored**:
- Device metadata (names, locations, enabled status)
- Historical sensor data (time-series)
- Device properties (model info, firmware, capabilities)

**Query locations**:
- `device.cpp`: Device info retrieval/updates
- `device_sensor.cpp`: Historical data management
- Device subclasses: Type-specific data storage

### MQTT Integration

**MQTT Publishing Flow**:
- Triggered from `DeviceManager_advertisement.cpp` on BLE advertisement
- Topic format: `{topicA}/{topicB}/BTtoMQTT/{MAC_ADDRESS}`
- Payload: JSON with decoded sensor data
- Optional: Home Assistant MQTT discovery messages

**Configuration**:
- Settings in SettingsManager (host, port, credentials, topics)
- Preset brokers: HomeAssistant, OpenHAB, NodeRED, Jeedom
- Connected on app startup if enabled

## QML UI Structure

**Platform-Specific Entry Points**:
- `qml/DesktopApplication.qml` - Desktop UI
- `qml/MobileApplication.qml` - Mobile UI

**Key QML Screens** (in `qml/`):
- `DeviceList.qml` - Main device list view
- `Device*.qml` - Device-specific detail screens (Thermometer, PlantSensor, Scale, Probe, etc.)
- `Settings.qml` / `SettingsMqtt.qml` - Configuration screens
- `charts/` - Data visualization components
- `components/` - Reusable UI components
- `popups/` - Modal dialogs

**C++ to QML Bridge**:
C++ objects exposed via context properties in `main.cpp`:
- `deviceManager`, `settingsManager`, `mqttManager`, `notificationManager`
- Utility classes: `utilsApp`, `utilsScreen`, `utilsLanguage`

## Key Files to Understand

### Core Architecture
- `src/main.cpp` (261 lines) - Application initialization and QML setup
- `src/DeviceManager.h` / `DeviceManager.cpp` - Central BLE management
- `src/device.h` / `device.cpp` - Base device class with common functionality

### BLE Processing
- `src/DeviceManager_advertisement.cpp` - Raw advertisement parsing
- `src/DeviceManager_theengs.cpp` - Theengs decoder integration
- `src/device_utils.h` - Device type definitions and enums
- `src/device_utils_theengs.h` - Theengs-specific utilities

### Theengs Decoder Integration
- `thirdparty/TheengsDecoder/src/decoder.h` - External library for decoding BLE advertisements from 100+ sensor types
- Integration points: `DeviceManager_advertisement.cpp` and `DeviceManager_theengs.cpp`

### Device Implementations
- `src/device_theengs.cpp` - Base class for Theengs-decoded devices
- `src/devices/device_theengs_*.cpp` - Category-specific implementations
- `src/devices/device_*.cpp` - Individual device models with custom parsing

### Data Persistence
- `src/DatabaseManager.cpp` - Database connection and schema management
- `src/device_sensor.cpp` - Historical data queries and storage

## Development Patterns

### Adding a New Device Type

1. **If using Theengs Decoder** (most common):
   - Device will be automatically recognized if supported by decoder
   - Add custom handling in appropriate `src/devices/device_theengs_*.cpp` if needed
   - Create corresponding QML view in `qml/Device*.qml`

2. **For custom device parsing**:
   - Create new device class inheriting from appropriate base (DeviceSensor, DeviceThermometer, etc.)
   - Implement `parseAdvertisementData()` method
   - Add device instantiation logic in `DeviceManager_theengs.cpp`
   - Update device type enums in `src/device_utils.h`

### Exposing C++ to QML

Objects are exposed via context properties in `main.cpp`:
```cpp
engine.rootContext()->setContextProperty("deviceManager", DeviceManager::getInstance());
```

Properties must use Qt's property system:
```cpp
Q_PROPERTY(int temperature READ getTemperature NOTIFY temperatureChanged)
```

### Database Migrations

Schema versions are managed in `DatabaseManager.cpp`:
- Update `createDatabase()` for new installations
- Add migration logic in `checkDatabase()` for existing databases
- Increment schema version number

## Third-Party Dependencies

Built from source via `contribs/contribs_builder_qt.py`:
- **QtMqtt** - MQTT client library (GPL 3) - **Required for build**
- **QtConnectivity** (custom BLE filtering build) - Android BLE improvements

Bundled as submodules in `thirdparty/`:
- **TheengsDecoder** - BLE advertisement decoder (GPL 3)
- **MobileUI** - Mobile UI components (MIT)
- **MobileSharing** - Cross-platform sharing (MIT)
- **SingleApplication** - Single instance enforcement (MIT)

## Platform-Specific Notes

### Android
- Background service support via `src/AndroidService.cpp`
- Requires BLE and location permissions
- Experimental background scanning mode

### iOS
- Minimum iOS 16+
- BLE permissions handled by `PermissionManager`
- No background scanning (iOS limitation)

### Desktop
- Systray integration via `SystrayManager`
- Menubar via `MenubarManager`
- Single instance enforcement using SingleApplication

## Important Conventions

### Device Model IDs
- Theengs devices use `model_id` from decoder (e.g., "LYWSD03MMC", "TPMS", "RuuviTag")
- Model IDs are defined in Theengs Decoder library
- Used for device instantiation and capability detection

### Signal/Slot Naming
- Signals: past tense (e.g., `deviceUpdated()`, `scanningChanged()`)
- Slots: imperative (e.g., `updateDevice()`, `startScanning()`)

### Property Naming
- C++ members: `m_variableName`
- QML properties: `camelCase`
- Constants: `ALL_CAPS` or `kConstantName`

### File Organization
- Device implementations: `src/devices/device_*.cpp`
- Manager classes: `src/*Manager.cpp`
- QML screens: `qml/Device*.qml` or `qml/Screen*.qml`
- Reusable QML: `qml/components/`

## Documentation

- User docs: `docs/` directory (VuePress-based website)
- Build docs: GitHub Actions workflows in `.github/workflows/`
- Device support: https://decoder.theengs.io/devices/devices.html
- App docs: https://app.theengs.io/

## CI/CD

GitHub Actions workflows:
- `.github/workflows/builds_desktop.yml` - Linux, macOS, Windows builds
- `.github/workflows/builds_mobile.yml` - Android, iOS builds
- `.github/workflows/flatpak.yml` - Flatpak packaging
- `.github/workflows/publish_docs.yml` - Documentation deployment

Artifacts generated:
- Linux: AppImage
- macOS: .app bundle / DMG
- Windows: Installer
- Android: APK / AAB
- iOS: IPA
