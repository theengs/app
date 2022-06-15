#!/usr/bin/env python3

import os
import sys
import platform
import multiprocessing
import glob
import shutil
import zipfile
import tarfile
import argparse
import subprocess
import urllib.request

print("\n> Theengs contribs builder")

if sys.version_info < (3, 0):
    print("This script NEEDS Python 3. Run it with 'python3 contribs.py'")
    sys.exit()

## DEPENDENCIES ################################################################
# These software dependencies are needed for this script to run!

## linux:
# cmake ninja libtool python3

## macOS:
# brew install cmake automake ninja python
# brew install libtool pkg-config
# xcode (10+)

## Windows:
# python3 (https://www.python.org/downloads/)
# cmake (https://cmake.org/download/)
# MSVC (2017+)

## HOST ########################################################################

# Supported platforms / architectures:
# Natives:
# - Linux
# - Darwin (macOS)
# - Windows
# Cross compilation (from Linux):
# - Windows (mingw32-w64)
# Cross compilation (from macOS):
# - iOS (simulator, armv7, armv8)
# Cross compilation (from Linux or macOS):
# - Android (armv7, armv8, x86, x86_64)

OS_HOST = platform.system()
ARCH_HOST = platform.machine()
CPU_COUNT = multiprocessing.cpu_count()

print("HOST SYSTEM : " + platform.system() + " (" + platform.release() + ") [" + os.name + "]")
print("HOST ARCH   : " + ARCH_HOST)
print("HOST CPUs   : " + str(CPU_COUNT) + " cores")

## SANITY CHECKS ###############################################################

if platform.system() != "Windows":
    if os.getuid() == 0:
        print("This script MUST NOT be run as root")
        sys.exit()

if os.path.basename(os.getcwd()) != "contribs":
    print("This script MUST be run from the contribs/ directory")
    sys.exit()

if platform.machine() not in ("x86_64", "AMD64"):
    print("This script needs a 64bits OS")
    sys.exit()

## SETTINGS ####################################################################

contribs_dir = os.getcwd()
src_dir = contribs_dir + "/src/"
deploy_dir = contribs_dir + "/deploy/"

clean = False
rebuild = False
ANDROID_NDK_HOME = os.getenv('ANDROID_NDK_HOME', '')
QT_DIRECTORY = os.getenv('QT_DIRECTORY', '')
QT_VERSION = "6.3.1"
mobile = False

# MSVC_GEN_VER
if "14.0" in os.getenv('VisualStudioVersion', ''):
    MSVC_GEN_VER = "Visual Studio 14 2015"
elif "15.0" in os.getenv('VisualStudioVersion', ''):
    MSVC_GEN_VER = "Visual Studio 15 2017"
elif "16.0" in os.getenv('VisualStudioVersion', ''):
    MSVC_GEN_VER = "Visual Studio 16 2019"
elif "17.0" in os.getenv('VisualStudioVersion', ''):
    MSVC_GEN_VER = "Visual Studio 17 2022"
else:
    MSVC_GEN_VER = "Visual Studio 17 2022"

## ARGUMENTS ###################################################################

parser = argparse.ArgumentParser(prog='contribs.py',
                                 description='Theengs contribs builder',
                                 formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('-c', '--clean', help="clean everything and exit (downloaded files and all temporary directories)", action='store_true')
parser.add_argument('-r', '--rebuild', help="rebuild the contribs even if already built", action='store_true')
parser.add_argument('--mobile', help="enable mobile builds", action='store_true')
parser.add_argument('--android-ndk', dest='androidndk', help="specify a custom path to the android-ndk (if ANDROID_NDK_HOME environment variable doesn't exists)")
parser.add_argument('--qt-directory', dest='qtdirectory', help="specify a custom path to the Qt install root dir (if QT_DIRECTORY environment variable doesn't exists)")
parser.add_argument('--qt-version', dest='qtversion', help="specify a Qt version to use")
parser.add_argument('--msvc', dest='msvcversion', help="specify a version for Visual Studio (2015/2017/2019)")

if len(sys.argv) > 1:
    result = parser.parse_args()
    if result.clean:
        clean = result.clean
    if result.rebuild:
        rebuild = result.rebuild
    if result.mobile:
        mobile = result.mobile
    if result.androidndk:
        ANDROID_NDK_HOME = result.androidndk
    if result.qtdirectory:
        QT_DIRECTORY = result.qtdirectory
    if result.qtversion:
        QT_VERSION = result.qtversion
    if result.msvcversion:
        if result.msvcversion == 2015:
            MSVC_GEN_VER = "Visual Studio 14 2015"
        elif result.msvcversion == 2017:
            MSVC_GEN_VER = "Visual Studio 15 2017"
        elif result.msvcversion == 2019:
            MSVC_GEN_VER = "Visual Studio 16 2019"
        elif result.msvcversion == 2022:
            MSVC_GEN_VER = "Visual Studio 17 2022"

## CLEAN #######################################################################

if rebuild:
    if os.path.exists(contribs_dir + "/build/"):
        shutil.rmtree(contribs_dir + "/build/")

if clean:
    if os.path.exists(contribs_dir + "/src/"):
        shutil.rmtree(contribs_dir + "/src/")
    if os.path.exists(contribs_dir + "/build/"):
        shutil.rmtree(contribs_dir + "/build/")
    if os.path.exists(contribs_dir + "/env/"):
        shutil.rmtree(contribs_dir + "/env/")
    print(">> Contribs cleaned!")
    sys.exit()

if not os.path.exists(src_dir):
    os.makedirs(src_dir)
if not os.path.exists(deploy_dir):
    os.makedirs(deploy_dir)

## UTILS #######################################################################

def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            if not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
                shutil.copy2(s, d)

def copytree_wildcard(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in glob.glob(src):
        shutil.copy2(item, dst)

## TARGETS #####################################################################

TARGETS = [] # 1: OS_TARGET # 2: ARCH_TARGET # 3: QT_TARGET

if OS_HOST == "Linux":
    TARGETS.append(["linux", "x86_64", "gcc_64"])
    #TARGETS.append(["windows", "x86_64"]) # Windows cross compilation

if OS_HOST == "Darwin":
    TARGETS.append(["macOS", "x86_64", "macOS"])
    #TARGETS.append(["macOS", "arm64", "macOS"])
    if mobile:
        TARGETS.append(["iOS", "simulator", "iOS"]) # iOS cross compilation
        TARGETS.append(["iOS", "armv8", "iOS"])
        TARGETS.append(["iOS", "armv7", "iOS"])

if OS_HOST == "Windows":
    if "14.0" in os.getenv('VisualStudioVersion', ''):
        TARGETS.append(["windows", "x86_64", "msvc2015_64"])
    elif "15.0" in os.getenv('VisualStudioVersion', ''):
        TARGETS.append(["windows", "x86_64", "msvc2017_64"])
    elif "16.0" in os.getenv('VisualStudioVersion', ''):
        TARGETS.append(["windows", "x86_64", "msvc2019_64"])
    elif "17.0" in os.getenv('VisualStudioVersion', ''):
        TARGETS.append(["windows", "x86_64", "msvc2019_64"])
    else:
        TARGETS.append(["windows", "x86_64", "msvc2019_64"])

if mobile:
    if ANDROID_NDK_HOME: # Android cross compilation
        TARGETS.append(["android", "armv8", "android_arm64_v8a"])
        TARGETS.append(["android", "armv7", "android_armv7"])
        TARGETS.append(["android", "x86_64", "android_x86_64"])
        TARGETS.append(["android", "x86", "android_x86"])

## DOWNLOAD SOFTWARES ##########################################################

## Theengs decoder (version: git)
FILE_theengsdecoder = "theengsdecoder-development.zip"
DIR_theengsdecoder = "decoder-development"

#if not os.path.exists(src_dir + FILE_theengsdecoder):
#    print("> Downloading " + FILE_theengsdecoder + "...")
#    urllib.request.urlretrieve("https://github.com/theengs/decoder/archive/refs/heads/development.zip", src_dir + FILE_theengsdecoder)

## ArduinoJson (version: git)
FILE_arduinojson = "arduinojson-6.zip"
DIR_arduinojson = "ArduinoJson-6.x"

#if not os.path.exists(src_dir + FILE_arduinojson):
#    print("> Downloading " + FILE_arduinojson + "...")
#    urllib.request.urlretrieve("https://github.com/bblanchon/ArduinoJson/archive/refs/heads/6.x.zip", src_dir + FILE_arduinojson)

## QtMqtt (version: QT_VERSION)
FILE_qtmqtt = "qtmqtt-" + QT_VERSION + ".zip"
DIR_qtmqtt = "qtmqtt-" + QT_VERSION

if not os.path.exists(src_dir + FILE_qtmqtt):
    print("> Downloading " + FILE_qtmqtt + "...")
    urllib.request.urlretrieve("https://github.com/qt/qtmqtt/archive/refs/tags/v" + QT_VERSION + ".zip", src_dir + FILE_qtmqtt)

## Android QtConnectivity (version: custom)
for TARGET in TARGETS:
    if TARGET[0] == "android":
        FILE_qtconnectivity = "qtconnectivity-blescanfiltering_v1_631.zip"
        DIR_qtconnectivity = "qtconnectivity-blescanfiltering_v1_631"

        if not os.path.exists(src_dir + FILE_qtconnectivity):
            print("> Downloading " + FILE_qtconnectivity + "...")
            urllib.request.urlretrieve("https://github.com/emericg/qtconnectivity/archive/refs/heads/blescanfiltering_v1_631.zip", src_dir + FILE_qtconnectivity)
        if not os.path.isdir("env/" + DIR_qtconnectivity):
            zipQtConnectivity = zipfile.ZipFile(src_dir + FILE_qtconnectivity)
            zipQtConnectivity.extractall("env/")
        break

## Android OpenSSL (version: git)
for TARGET in TARGETS:
    if TARGET[0] == "android":
        FILE_androidopenssl = "android_openssl-master.zip"
        DIR_androidopenssl = "android_openssl"

        if not os.path.exists(src_dir + FILE_androidopenssl):
            print("> Downloading " + FILE_androidopenssl + "...")
            urllib.request.urlretrieve("https://github.com/KDAB/android_openssl/archive/master.zip", src_dir + FILE_androidopenssl)
        if not os.path.isdir("env/" + DIR_androidopenssl):
            zipSSL = zipfile.ZipFile(src_dir + FILE_androidopenssl)
            zipSSL.extractall("env/")
        break

## linuxdeploy (version: git)
if OS_HOST == "Linux":
    FILE_linuxdeploy = "linuxdeploy-x86_64.AppImage"
    if not os.path.exists(deploy_dir + FILE_linuxdeploy):
        print("> Downloading " + FILE_linuxdeploy + "...")
        urllib.request.urlretrieve("https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/" + FILE_linuxdeploy, deploy_dir + FILE_linuxdeploy)
        urllib.request.urlretrieve("https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage", deploy_dir + "linuxdeploy-plugin-appimage-x86_64.AppImage")
        urllib.request.urlretrieve("https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage", deploy_dir + "linuxdeploy-plugin-qt-x86_64.AppImage")

## BUILD SOFTWARES #############################################################

for TARGET in TARGETS:

    ## PREPARE environment
    OS_TARGET = TARGET[0]
    ARCH_TARGET = TARGET[1]
    QT_TARGET = TARGET[2]

    build_dir = contribs_dir + "/build/" + OS_TARGET + "_" + ARCH_TARGET + "/"
    env_dir = contribs_dir + "/env/" + OS_TARGET + "_" + ARCH_TARGET + "/"
    qt6_dir = QT_DIRECTORY + "/" + QT_VERSION + "/" + QT_TARGET + "/bin/"

    try:
        os.makedirs(build_dir)
        os.makedirs(env_dir)
    except:
        print() # who cares

    print("> TARGET : " + str(TARGET))
    print("- build_dir : " + build_dir)
    print("- env_dir : " + env_dir)
    print("- qt6_dir : " + qt6_dir)

    ## CMAKE command selection
    CMAKE_cmd = ["cmake"]
    CMAKE_gen = "Ninja"
    build_shared = "ON"
    build_static = "OFF"

    if OS_HOST == "Linux":
        if OS_TARGET == "windows":
            if ARCH_TARGET == "i686":
                CMAKE_cmd = ["i686-w64-mingw32-cmake"]
            else:
                CMAKE_cmd = ["x86_64-w64-mingw32-cmake"]
    elif OS_HOST == "Darwin":
        if OS_TARGET == "iOS":
            CMAKE_gen = "Xcode"
            #IOS_DEPLOYMENT_TARGET="10.0"
            build_shared = "OFF"
            build_static = "ON"
            if ARCH_TARGET == "simulator":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + contribs_dir + "/tools/ios.toolchain.cmake", "-DPLATFORM=SIMULATOR64"]
            elif ARCH_TARGET == "armv7":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + contribs_dir + "/tools/ios.toolchain.cmake", "-DPLATFORM=OS"]
            elif ARCH_TARGET == "armv8":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + contribs_dir + "/tools/ios.toolchain.cmake", "-DPLATFORM=OS64"]
            else:
                # Without custom toolchain
                CMAKE_cmd = ["cmake", "-DCMAKE_SYSTEM_NAME=iOS","-DCMAKE_OSX_ARCHITECTURES=arm64","-DCMAKE_OSX_DEPLOYMENT_TARGET=10.0"]
    elif OS_HOST == "Windows":
        CMAKE_gen = MSVC_GEN_VER
        if ARCH_TARGET == "armv7":
            CMAKE_cmd = ["cmake", "-A", "ARM"]
        elif ARCH_TARGET == "armv8":
            CMAKE_cmd = ["cmake", "-A", "ARM64"]
        elif ARCH_TARGET == "x86":
            CMAKE_cmd = ["cmake", "-A", "Win32"]
        else:
            CMAKE_cmd = ["cmake", "-A", "x64"]

    if OS_HOST == "Linux" or OS_HOST == "Darwin":
        if OS_TARGET == "android":
            if ARCH_TARGET == "x86":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_HOME + "/build/cmake/android.toolchain.cmake", "-DANDROID_TOOLCHAIN=clang", "-DANDROID_ABI=x86", "-DANDROID_PLATFORM=android-21"]
            elif ARCH_TARGET == "x86_64":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_HOME + "/build/cmake/android.toolchain.cmake", "-DANDROID_TOOLCHAIN=clang", "-DANDROID_ABI=x86_64", "-DANDROID_PLATFORM=android-21"]
            elif ARCH_TARGET == "armv7":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_HOME + "/build/cmake/android.toolchain.cmake", "-DANDROID_TOOLCHAIN=clang", "-DANDROID_ABI=armeabi-v7a", "-DANDROID_PLATFORM=android-21"]
            else:
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_HOME + "/build/cmake/android.toolchain.cmake", "-DANDROID_TOOLCHAIN=clang", "-DANDROID_ABI=arm64-v8a", "-DANDROID_PLATFORM=android-21"]

    ## EXTRACT #################################################################

    ## theengs decoder
    #if not os.path.isdir(build_dir + DIR_theengsdecoder):
    #    zipTD = zipfile.ZipFile(src_dir + FILE_theengsdecoder)
    #    zipTD.extractall(build_dir)

    #zipAJS = zipfile.ZipFile(src_dir + FILE_arduinojson)
    #zipAJS.extractall(build_dir + DIR_theengsdecoder + "/src/")

    #if os.path.exists(build_dir + DIR_theengsdecoder + "/src/arduino_json"):
    #    shutil.rmtree(build_dir + DIR_theengsdecoder + "/src/arduino_json")
    #    shutil.move(build_dir + DIR_theengsdecoder + "/src/" + DIR_arduinojson, build_dir + DIR_theengsdecoder + "/src/arduino_json")

    ## QtMqtt
    if not os.path.isdir(build_dir + DIR_qtmqtt):
        zipQtM = zipfile.ZipFile(src_dir + FILE_qtmqtt)
        zipQtM.extractall(build_dir)

    ## QtConnectivity (patched)
    if OS_TARGET == "android":
        if not os.path.isdir(build_dir + DIR_qtconnectivity):
            zipQtC = zipfile.ZipFile(src_dir + FILE_qtconnectivity)
            zipQtC.extractall(build_dir)

    ## BUILD & INSTALL #########################################################

    ## theengs decoder build
    #print("> Building theengs decoder")
    #try: os.makedirs(build_dir + DIR_theengsdecoder + "/build")
    #except: print() # who cares
    #subprocess.check_call(CMAKE_cmd + ["-G", CMAKE_gen, "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_SHARED_LIBS:BOOL=" + build_shared, "-DBUILD_STATIC_LIBS:BOOL=" + build_static, "-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE", "-DCMAKE_INSTALL_PREFIX=" + env_dir + "/usr", ".."], cwd=build_dir + DIR_theengsdecoder + "/build")
    #subprocess.check_call(["cmake", "--build", ".", "--config", "Release"], cwd=build_dir + DIR_theengsdecoder + "/build")

    ## theengs decoder manual installation
    #try:
    #    os.makedirs(env_dir + "/usr/lib/")
    #    os.makedirs(env_dir + "/usr/include/theengs")
    #except: print() # who cares
    #shutil.copy2(build_dir + DIR_theengsdecoder + "/build/libdecoder.so", env_dir + "/usr/lib/libdecoder.so")
    #shutil.copy2(build_dir + DIR_theengsdecoder + "/src/decoder.cpp", env_dir + "/usr/include/theengs/")
    #shutil.copy2(build_dir + DIR_theengsdecoder + "/src/decoder.h", env_dir + "/usr/include/theengs/")
    #shutil.copy2(build_dir + DIR_theengsdecoder + "/src/devices.h", env_dir + "/usr/include/theengs/")
    #copytree(build_dir + DIR_theengsdecoder + "/src/devices", env_dir + "/usr/include/theengs/devices/")

    ## Qt prep work
    if OS_HOST == "Windows":
        QT_CONF_MODULE_cmd = qt6_dir + "qt-configure-module.bat"
        #VCVARS_cmd = "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/" + "vcvarsall.bat"
        #subprocess.check_call([VCVARS_cmd, "x86_amd64"], cwd="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/")
    else:
        QT_CONF_MODULE_cmd = qt6_dir + "qt-configure-module"
        if mobile:
            # GitHub CI + aqt hack
            if (OS_HOST == "Linux"): os.environ["QT_HOST_PATH"] = str(QT_DIRECTORY + "/" + QT_VERSION + "/gcc_64/")
            if (OS_HOST == "Darwin"): os.environ["QT_HOST_PATH"] = str(QT_DIRECTORY + "/" + QT_VERSION + "/macOS/")

    ## QtMqtt
    try: os.makedirs(build_dir + DIR_qtmqtt + "/build")
    except: print() # who cares

    print("> Building QtMqtt")
    subprocess.check_call([QT_CONF_MODULE_cmd, ".."], cwd=build_dir + DIR_qtmqtt + "/build")
    subprocess.check_call(["cmake", "--build", ".", "--target", "all"], cwd=build_dir + DIR_qtmqtt + "/build")
    subprocess.check_call(["cmake", "--install", "."], cwd=build_dir + DIR_qtmqtt + "/build")

    ## QtConnectivity (patched)
    try: os.makedirs(build_dir + DIR_qtconnectivity + "/build")
    except: print() # who cares

    if OS_TARGET == "android":
        print("> Building QtConnectivity")
        subprocess.check_call([QT_CONF_MODULE_cmd, ".."], cwd=build_dir + DIR_qtconnectivity + "/build")
        subprocess.check_call(["cmake", "--build", ".", "--target", "all"], cwd=build_dir + DIR_qtconnectivity + "/build")
        subprocess.check_call(["cmake", "--install", "."], cwd=build_dir + DIR_qtconnectivity + "/build")

    ############################################################################
