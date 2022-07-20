#  Build
## Dependencies

You will need a C++17 compiler and Qt 6.3+ (with additional libraries).  
For Android builds, you'll need the appropriates JDK (11) SDK (23+) and NDK (22+). You can customize Android build environment using the `assets/android/gradle.properties` file.  
For Windows builds, you'll need the MSVC 2019+ compiler.  
For macOS and iOS builds, you'll need Xcode installed.  

Qt 6 'Additional Libraries':
- Qt 5 Compatibility Module
- Qt Connectivity
- Qt Image Formats
- Qt Charts
- Qt WebSockets

## Cloning Theengs

```bash
$ git clone https://github.com/theengs/app.git TheengsApp --recurse-submodules
```

## Building dependencies (with the contribs.py script)

This will build QtMqtt and a patched version of QtConnectivity (for Android)

```bash
$ cd TheengsApp/contribs/
$ # simple desktop build
$ python3 contribs.py
$ # complex build with Android cross compilation
$ python3 contribs.py --qt-directory /home/USER/Qt --android-ndk /home/USER/android-sdk/ndk/22.1.7171670/ --mobile
```

## Building Theengs

```bash
$ cd TheengsApp/
$ qmake DEFINES+=QT_CONNECTIVITY_PATCHED
$ make
```