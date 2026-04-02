#!/usr/bin/env python3

import os
import sys
import platform

if sys.version_info < (3, 6):
    print("This script NEEDS Python 3.6. Run it with 'python3 contribs.py'")
    sys.exit()

if os.path.basename(os.getcwd()) != "contribs":
    print("This script SHOULD be run from the contribs/ directory")
    sys.exit()

if platform.system() != "Windows":
    if os.getuid() == 0:
        print("This script SHOULD NOT be run as root")
        sys.exit()

import re
import glob
import shutil
import zipfile
import tarfile
import argparse
import subprocess
import multiprocessing
import urllib.request

## WELCOME #####################################################################

print("")
print("> Theengs contribs builder script")
print("> Make sure you consult ./contribs_builder.py --help")
print("")

targets = ['linux', 'linux_x86_64', 'linux_arm64',
           'macos', 'macos_x86_64', 'macos_arm64',
           'msvc2019', 'msvc2022', 'msvc2026',
           'android_armv8', 'android_armv7', 'android_x86_64', 'android_x86',
           'ios', 'ios_simulator']

softwares = ['mbedtls']

print("> targets available:")
print(str(targets))
print("")
print("> softwares available:")
print(str(softwares))

## DEPENDENCIES ################################################################
# These software dependencies are needed for this script to run!

## linux:
# python3 cmake ninja libtool automake m4
# sudo apt-get install gcc g++ libxcb-cursor0 libxcb-cursor-dev libgl1-mesa-dev
# libudev-dev (for ?)

## macOS:
# brew install python cmake automake ninja
# brew install libtool pkg-config
# brew install libudev utf8cpp (for libusb)
# brew install iconv gettext (for libexif)
# brew link --force gettext (for libexif)
# xcode (13+)

## Windows:
# python3 (https://www.python.org/downloads/)
# cmake (https://cmake.org/download/)
# MSVC (2019+)

## HOST ########################################################################

# Supported platforms / architectures:

# Natives:
# - Linux
# - macOS
# - Windows
# Cross compilation (from Linux):
# - Windows (mingw32-w64)
# Cross compilation (from Linux or macOS):
# - Android (armv7, armv8, x86, x86_64)
# Cross compilation (from macOS):
# - iOS (unified, simulator)

OS_NAME = os.name
OS_HOST = platform.system()
OS_HOST_VERSION = platform.release()
ARCH_HOST = platform.machine()
CPU_COUNT = multiprocessing.cpu_count()

print("")
print(f"HOST SYSTEM : {OS_HOST} ({OS_HOST_VERSION}) [{OS_NAME}]")
print(f"HOST ARCH   : {ARCH_HOST}")
print(f"HOST CPUs   : {CPU_COUNT} cores")
print("")

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

## SETTINGS ####################################################################

contribs_dir = os.getcwd()
src_dir = contribs_dir + "/src/"
deploy_dir = contribs_dir + "/deploy/"

clean = False
rebuild = False
targets_selected = []
softwares_selected = []

QT_VERSION = "6.10.3"
QT_DIRECTORY = ""

if os.getenv('QT_DIRECTORY', ''):
    # try autodetection
    QT_DIRECTORY = os.getenv('QT_DIRECTORY', '')
elif os.getenv('QT_ROOT_DIR', ''):
    # HACK # from GitHub jurplel/install-qt-action
    QT_DIRECTORY = os.getenv('QT_ROOT_DIR', '') + "/../../"

# try autodetection
ANDROID_SDK_ROOT = os.getenv('ANDROID_SDK_ROOT', '')
ANDROID_NDK_ROOT = os.getenv('ANDROID_NDK_ROOT', '')

MSVC_GEN_VER = ""

## ARGUMENTS ###################################################################

parser = argparse.ArgumentParser(prog='contribs.py',
                                 description='',
                                 formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('-c', '--clean', help="clean everything and exit (downloaded files and all temporary directories)", action='store_true')
parser.add_argument('-r', '--rebuild', help="rebuild the contribs even if already built", action='store_true')
parser.add_argument('--targets', dest='targets', help="specify target(s) platforms")
parser.add_argument('--softwares', dest='softwares', help="specify software(s) to build")
parser.add_argument('--qt-version', dest='qtversion', help="specify a Qt version to use")
parser.add_argument('--qt-directory', dest='qtdirectory', help="specify a custom path to the Qt install root dir (if QT_DIRECTORY or QT_ROOT_DIR environment variables aren't set)")
parser.add_argument('--android-sdk', dest='androidsdk', help="specify a custom path to the android-sdk (if ANDROID_SDK_ROOT environment variable isn't set)")
parser.add_argument('--android-ndk', dest='androidndk', help="specify a custom path to the android-ndk (if ANDROID_NDK_ROOT environment variable isn't set)")

if len(sys.argv) > 1:
    result = parser.parse_args()
    if result.clean:
        clean = result.clean
    if result.rebuild:
        rebuild = result.rebuild
    if result.targets:
        targets_selected = result.targets.split(',')
    if result.softwares:
        softwares_selected = result.softwares.split(',')
    if result.qtversion:
        QT_VERSION = result.qtversion
    if result.qtdirectory:
        QT_DIRECTORY = result.qtdirectory
    if result.androidsdk:
        ANDROID_SDK_ROOT = result.androidsdk
    if result.androidndk:
        ANDROID_NDK_ROOT = result.androidndk

if len(softwares_selected) == 0:
    softwares_selected = softwares

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

## TARGETS #####################################################################

TARGETS = [] # 1: OS_TARGET # 2: ARCH_TARGET # 3: QT_TARGET

# > using script arguments
if len(targets_selected):
    print("TARGETS from script arguments")

    if "linux" in targets_selected: TARGETS.append(["linux", "x86_64", "gcc_64"])
    if "linux_x86_64" in targets_selected: TARGETS.append(["linux", "x86_64", "gcc_64"])
    if "linux_arm64" in targets_selected: TARGETS.append(["linux", "arm64", "gcc_arm64"])

    if "macos" in targets_selected: TARGETS.append(["macOS", "unified", "macos"])
    if "macos_x86_64" in targets_selected: TARGETS.append(["macOS", "x86_64", "macos"])
    if "macos_arm64" in targets_selected: TARGETS.append(["macOS", "arm64", "macos"])

    if "msvc2019" in targets_selected:
        MSVC_GEN_VER = "Visual Studio 16 2019"
        TARGETS.append(["windows", "x86_64", "msvc2019_64"])
    if "msvc2022" in targets_selected:
        MSVC_GEN_VER = "Visual Studio 17 2022"
        TARGETS.append(["windows", "x86_64", "msvc2022_64"])
    if "msvc2026" in targets_selected:
        MSVC_GEN_VER = "Visual Studio 18 2026"
        TARGETS.append(["windows", "x86_64", "msvc2026_64"])

    if "android_armv8" in targets_selected: TARGETS.append(["android", "armv8", "android_arm64_v8a"])
    if "android_armv7" in targets_selected: TARGETS.append(["android", "armv7", "android_armv7"])
    if "android_x86_64" in targets_selected: TARGETS.append(["android", "x86_64", "android_x86_64"])
    if "android_x86" in targets_selected: TARGETS.append(["android", "x86", "android_x86"])

    if "ios" in targets_selected: TARGETS.append(["iOS", "unified", "ios"])
    if "ios_armv7" in targets_selected: TARGETS.append(["iOS", "armv7", "ios"])
    if "ios_armv8" in targets_selected: TARGETS.append(["iOS", "armv8", "ios"])
    if "ios_simulator" in targets_selected: TARGETS.append(["iOS", "simulator", "ios"])

# > using auto-selection
if len(TARGETS) == 0:
    print("TARGETS auto-selection")

    if OS_HOST == "Linux":
        TARGETS.append(["linux", "x86_64", "gcc_64"])
        #TARGETS.append(["windows", "x86_64", ""]) # Windows cross compilation

    if OS_HOST == "Darwin":
        TARGETS.append(["macOS", "unified", "macos"])
        TARGETS.append(["iOS", "unified", "ios"])

    if OS_HOST == "Windows":
        if "16.0" in os.getenv('VisualStudioVersion', ''):
            MSVC_GEN_VER = "Visual Studio 16 2019"
            TARGETS.append(["windows", "x86_64", "msvc2019_64"])
        elif "17.0" in os.getenv('VisualStudioVersion', ''):
            MSVC_GEN_VER = "Visual Studio 17 2022"
            TARGETS.append(["windows", "x86_64", "msvc2022_64"])
        elif "18.0" in os.getenv('VisualStudioVersion', ''):
            MSVC_GEN_VER = "Visual Studio 18 2026"
            TARGETS.append(["windows", "x86_64", "msvc2026_64"])
        else:
            MSVC_GEN_VER = "Visual Studio 17 2022" # DEFAULT
            TARGETS.append(["windows", "x86_64", "msvc2022_64"])

    if ANDROID_NDK_ROOT: # Android cross compilation
        TARGETS.append(["android", "armv8", "android_arm64_v8a"])
        TARGETS.append(["android", "armv7", "android_armv7"])
        TARGETS.append(["android", "x86_64", "android_x86_64"])
        TARGETS.append(["android", "x86", "android_x86"])

## RECAP #######################################################################

# > targets recap:
print("TARGETS selected:\n" + str(TARGETS) + "\n")

# > softwares recap:
print("SOFTWARES selected:\n" + str(softwares_selected) + "\n")

## DOWNLOAD TOOLS ##############################################################

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

## linuxdeploy (version: git)
for TARGET in TARGETS:
    if TARGET[0] == "linux":
        FILE_linuxdeploy = "linuxdeploy-x86_64.AppImage"
        if not os.path.exists(deploy_dir + FILE_linuxdeploy):
            print("> Downloading " + FILE_linuxdeploy + "...")
            urllib.request.urlretrieve("https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/" + FILE_linuxdeploy, deploy_dir + FILE_linuxdeploy)
            urllib.request.urlretrieve("https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage", deploy_dir + "linuxdeploy-plugin-appimage-x86_64.AppImage")
            urllib.request.urlretrieve("https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage", deploy_dir + "linuxdeploy-plugin-qt-x86_64.AppImage")
            urllib.request.urlretrieve("https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gstreamer/master/linuxdeploy-plugin-gstreamer.sh", deploy_dir + "linuxdeploy-plugin-gstreamer.sh")

## DOWNLOAD SOFTWARES ##########################################################

## mbedTLS (version: 3.6.5)
NAME_mbedtls = "mbedTLS"
VERSION_mbedtls = "3.6.5"
FILE_mbedtls = "mbedtls-" + VERSION_mbedtls + ".tar.bz2"
DIR_mbedtls = "mbedtls-" + VERSION_mbedtls

if "mbedtls" in softwares_selected:
    if not os.path.exists(src_dir + FILE_mbedtls):
        print("> Downloading " + FILE_mbedtls + "...")
        urllib.request.urlretrieve("https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-" + VERSION_mbedtls + "/" + FILE_mbedtls, src_dir + FILE_mbedtls)

## libUSB (version: git) (1.0.29+)
FILE_libusb = "libusb-master.tar.gz"
DIR_libusb = "libusb-master"
## libMTP (version: git) (1.1.23+)
FILE_libmtp = "libmtp-master.tar.gz"
DIR_libmtp = "libmtp-master"

if {"libusb", "libmtp"} & set(softwares_selected):
    if not os.path.exists(src_dir + FILE_libusb):
        print("> Downloading " + FILE_libusb)
        urllib.request.urlretrieve("https://github.com/libusb/libusb/archive/master.zip", src_dir + FILE_libusb)
    if not os.path.exists(src_dir + FILE_libmtp):
        print("> Downloading " + FILE_libmtp)
        urllib.request.urlretrieve("https://github.com/libmtp/libmtp/archive/master.zip", src_dir + FILE_libmtp)

## libexif (custom) (version: git) (0.6.25+)
FILE_libexif = "libexif-master.zip"
DIR_libexif = "libexif-master"

if "libexif" in softwares_selected:
    if not os.path.exists(src_dir + FILE_libexif):
        print("> Downloading " + FILE_libexif + "...")
        urllib.request.urlretrieve("https://github.com/emericg/libexif/archive/master.zip", src_dir + FILE_libexif)

## taglib (version: git) (2.1+)
FILE_taglib = "taglib-master.zip"
FILE_taglib_utfcpp = "utfcpp-v4.0.8.zip"
DIR_taglib = "taglib-master"
DIR_taglib_utfcpp = DIR_taglib + "/3rdparty/utfcpp"

if "taglib" in softwares_selected:
    if not os.path.exists(src_dir + FILE_taglib):
        print("> Downloading " + FILE_taglib + "...")
        urllib.request.urlretrieve("https://github.com/taglib/taglib/archive/master.zip", src_dir + FILE_taglib)
    if not os.path.exists(src_dir + FILE_taglib_utfcpp):
        print("> Downloading " + FILE_taglib_utfcpp + "...")
        urllib.request.urlretrieve("https://github.com/nemtrif/utfcpp/archive/refs/tags/v4.0.8.zip", src_dir + FILE_taglib_utfcpp)

## minivideo (version: git) (0.15+)
FILE_minivideo = "minivideo-master.zip"
DIR_minivideo = "MiniVideo-master"

if "minivideo" in softwares_selected:
    if not os.path.exists(src_dir + FILE_minivideo):
        print("> Downloading " + FILE_minivideo + "...")
        urllib.request.urlretrieve("https://github.com/emericg/MiniVideo/archive/master.zip", src_dir + FILE_minivideo)

## BUILD SOFTWARES #############################################################

for TARGET in TARGETS:

    ## PREPARE environment
    OS_TARGET = TARGET[0]
    ARCH_TARGET = TARGET[1]
    QT_TARGET = TARGET[2]

    build_dir = contribs_dir + "/build/" + OS_TARGET + "_" + ARCH_TARGET + "/"
    env_dir = contribs_dir + "/env/" + OS_TARGET + "_" + ARCH_TARGET + "/"
    qt6_dir = QT_DIRECTORY + "/" + QT_VERSION + "/" + QT_TARGET + "/"
    qt6_bin_dir = QT_DIRECTORY + "/" + QT_VERSION + "/" + QT_TARGET + "/bin/"

    try:
        os.makedirs(build_dir)
        os.makedirs(env_dir)
    except:
        print() # who cares

    print("> TARGET : " + str(TARGET))
    print("- build_dir : " + build_dir)
    print("- env_dir : " + env_dir)
    print("- qt6_dir : " + qt6_dir)
    print("- qt6_bin_dir : " + qt6_bin_dir)

    ## CMAKE command selection
    CMAKE_cmd = ["cmake"]
    CMAKE_gen = "Ninja"
    build_shared = "ON"
    build_static = "OFF"

    if OS_HOST == "Linux":
        if OS_TARGET == "linux":
            if ARCH_TARGET == "x86_64":
                CMAKE_cmd = ["cmake"]
            elif ARCH_TARGET == "arm64":
                CMAKE_cmd = ["cmake"]
        if OS_TARGET == "windows":
            if ARCH_TARGET == "i686":
                CMAKE_cmd = ["i686-w64-mingw32-cmake"]
            else:
                CMAKE_cmd = ["x86_64-w64-mingw32-cmake"]

    if OS_HOST == "Darwin":
        if OS_TARGET == "macOS":
            if ARCH_TARGET == "unified":
                CMAKE_cmd = ["cmake", "-DCMAKE_OSX_ARCHITECTURE=x86_64;arm64"]
            elif ARCH_TARGET == "x86_64":
                CMAKE_cmd = ["cmake", "-DCMAKE_OSX_ARCHITECTURE=x86_64"]
            elif ARCH_TARGET == "arm64":
                CMAKE_cmd = ["cmake", "-DCMAKE_OSX_ARCHITECTURE=arm64"]
        if OS_TARGET == "iOS":
            CMAKE_gen = "Xcode"
            #IOS_DEPLOYMENT_TARGET="16.0"
            build_shared = "OFF"
            build_static = "ON"
            if ARCH_TARGET == "unified":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + contribs_dir + "/tools/ios.toolchain.cmake", "-DPLATFORM=OS64COMBINED"]
            elif ARCH_TARGET == "simulator":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + contribs_dir + "/tools/ios.toolchain.cmake", "-DPLATFORM=SIMULATOR64"]
            elif ARCH_TARGET == "armv7":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + contribs_dir + "/tools/ios.toolchain.cmake", "-DPLATFORM=OS"]
            elif ARCH_TARGET == "armv8":
                CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + contribs_dir + "/tools/ios.toolchain.cmake", "-DPLATFORM=OS64"]
            else:
                # Without custom toolchain?
                CMAKE_cmd = ["cmake", "-DCMAKE_SYSTEM_NAME=iOS", "-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0"]

    if OS_HOST == "Windows":
        CMAKE_gen = MSVC_GEN_VER
        if ARCH_TARGET == "armv7":
            CMAKE_cmd = ["cmake", "-A", "ARM"]
        elif ARCH_TARGET == "armv8":
            CMAKE_cmd = ["cmake", "-A", "ARM64"]
        elif ARCH_TARGET == "x86":
            CMAKE_cmd = ["cmake", "-A", "Win32"]
        else: # ARCH_TARGET == "x86_64":
            CMAKE_cmd = ["cmake", "-A", "x64"]

    if OS_TARGET == "android":
        if ARCH_TARGET == "x86":
            CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_ROOT + "/build/cmake/android.toolchain.cmake", "-DANDROID_ABI=x86", "-DANDROID_PLATFORM=android-23"]
        elif ARCH_TARGET == "x86_64":
            CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_ROOT + "/build/cmake/android.toolchain.cmake", "-DANDROID_ABI=x86_64", "-DANDROID_PLATFORM=android-23"]
        elif ARCH_TARGET == "armv7":
            CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_ROOT + "/build/cmake/android.toolchain.cmake", "-DANDROID_ABI=armeabi-v7a", "-DANDROID_PLATFORM=android-23"]
        else: # ARCH_TARGET == "armv8":
            CMAKE_cmd = ["cmake", "-DCMAKE_TOOLCHAIN_FILE=" + ANDROID_NDK_ROOT + "/build/cmake/android.toolchain.cmake", "-DANDROID_ABI=arm64-v8a", "-DANDROID_PLATFORM=android-23"]

    print("- CMAKE_cmd : " + str(CMAKE_cmd))
    #print("- CMAKE_qt_cmd : " + str(CMAKE_qt_cmd))
    print("")

    #### EXTRACT, BUILD & INSTALL ####

    ## mbedTLS
    if "mbedtls" in softwares_selected:
        if not os.path.isdir(build_dir + DIR_mbedtls):
            zipMBTLS = tarfile.open(src_dir + FILE_mbedtls)
            zipMBTLS.extractall(build_dir)

        try: os.makedirs(build_dir + DIR_mbedtls + "/build")
        except: print() # who cares

        print("> Building mbedTLS")
        subprocess.check_call(CMAKE_cmd + ["-G", CMAKE_gen, "-DCMAKE_BUILD_TYPE=Release", "-DUSE_SHARED_MBEDTLS_LIBRARY=On", "-DMBEDTLS_FATAL_WARNINGS=Off", "-DUNSAFE_BUILD=Off", "-DENABLE_TESTING=Off", "-DCMAKE_INSTALL_PREFIX=" + env_dir + "/usr", ".."], cwd=build_dir + DIR_mbedtls + "/build")
        if OS_TARGET == "iOS":
            subprocess.check_call(["cmake", "--build", ".", "--config", "Release"], cwd=build_dir + DIR_mbedtls + "/build")
            subprocess.check_call(["cmake", "--install", "."], cwd=build_dir + DIR_mbedtls + "/build")
        else:
            subprocess.check_call(["cmake", "--build", ".", "--target", "all"], cwd=build_dir + DIR_mbedtls + "/build")
            subprocess.check_call(["ninja", "install"], cwd=build_dir + DIR_mbedtls + "/build") # Qt BUG 91647

    ## libusb & libmtp
    if {"libusb", "libmtp"} & set(softwares_selected):
        if OS_HOST != "Windows":
            if not os.path.isdir(build_dir + DIR_libusb):
                zipUSB = zipfile.ZipFile(src_dir + FILE_libusb)
                zipUSB.extractall(build_dir)
            if not os.path.isdir(build_dir + DIR_libmtp):
                zipMTP = zipfile.ZipFile(src_dir + FILE_libmtp)
                zipMTP.extractall(build_dir)

            print("> Building libUSB")
            os.chdir(build_dir + DIR_libusb)
            os.chmod("bootstrap.sh", 509)
            os.system("./bootstrap.sh")
            os.system("./configure --prefix=" + env_dir + "/usr")
            os.system("make -j" + str(CPU_COUNT))
            os.system("make install")

            print("> Building libMTP")
            os.chdir(build_dir + DIR_libmtp)
            os.chmod("autogen.sh", 509)
            os.system("./autogen.sh << \"y\"")
            os.system("./configure --disable-mtpz --prefix=" + env_dir + "/usr --with-udev=" + env_dir + "/usr/lib/udev")
            os.system("make -j" + str(CPU_COUNT))
            os.system("make install")

    ## libexif
    if "libexif" in softwares_selected:
        if not os.path.isdir(build_dir + DIR_libexif):
            zipEX = zipfile.ZipFile(src_dir + FILE_libexif)
            zipEX.extractall(build_dir)

        print("> Building libexif")
        subprocess.check_call(CMAKE_cmd + ["-G", CMAKE_gen, "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_SHARED_LIBS:BOOL=" + build_shared, "-DBUILD_STATIC_LIBS:BOOL=" + build_static, "-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE", "-DCMAKE_INSTALL_PREFIX=" + env_dir + "/usr", ".."], cwd=build_dir + DIR_libexif + "/build")
        subprocess.check_call(["cmake", "--build", ".", "--config", "Release"], cwd=build_dir + DIR_libexif + "/build")
        subprocess.check_call(["cmake", "--build", ".", "--target", "install", "--config", "Release"], cwd=build_dir + DIR_libexif + "/build")

    ## taglib
    if "taglib" in softwares_selected:
        if not os.path.isdir(build_dir + DIR_taglib):
            zipTL = zipfile.ZipFile(src_dir + FILE_taglib)
            zipTL.extractall(build_dir)
            os.rmdir(build_dir + DIR_taglib + "/3rdparty/utfcpp/")
            os.makedirs(build_dir + DIR_taglib + "/build")
        if not os.path.isdir(build_dir + DIR_taglib_utfcpp):
            zipUTFCPP = zipfile.ZipFile(src_dir + FILE_taglib_utfcpp)
            zipUTFCPP.extractall(build_dir+ DIR_taglib + "/3rdparty/")
            os.rename(build_dir + DIR_taglib + "/3rdparty/utfcpp-4.0.8/", build_dir + DIR_taglib_utfcpp)
            os.makedirs(build_dir + DIR_taglib_utfcpp + "/build")

        print("> Building utfcpp")
        subprocess.check_call(CMAKE_cmd + ["-G", CMAKE_gen, "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_SHARED_LIBS:BOOL=" + build_shared, "-DBUILD_STATIC_LIBS:BOOL=" + build_static, "-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE", "-DCMAKE_INSTALL_PREFIX=" + env_dir + "/usr", ".."], cwd=build_dir + DIR_taglib_utfcpp + "/build")
        subprocess.check_call(["cmake", "--build", ".", "--config", "Release"], cwd=build_dir + DIR_taglib_utfcpp + "/build")
        subprocess.check_call(["cmake", "--build", ".", "--target", "install", "--config", "Release"], cwd=build_dir + DIR_taglib_utfcpp + "/build")

        print("> Building taglib")
        subprocess.check_call(CMAKE_cmd + ["-G", CMAKE_gen, "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_SHARED_LIBS:BOOL=" + build_shared, "-DBUILD_STATIC_LIBS:BOOL=" + build_static, "-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE", "-Dutf8cpp_INCLUDE_DIR=" + env_dir + "/usr/include/utf8cpp", "-DCMAKE_INSTALL_PREFIX=" + env_dir + "/usr", ".."], cwd=build_dir + DIR_taglib + "/build")
        subprocess.check_call(["cmake", "--build", ".", "--config", "Release"], cwd=build_dir + DIR_taglib + "/build")
        subprocess.check_call(["cmake", "--build", ".", "--target", "install", "--config", "Release"], cwd=build_dir + DIR_taglib + "/build")

    ## minivideo
    if "minivideo" in softwares_selected:
        if not os.path.isdir(build_dir + DIR_minivideo):
            zipMV = zipfile.ZipFile(src_dir + FILE_minivideo)
            zipMV.extractall(build_dir)

        print("> Building minivideo")
        subprocess.check_call(CMAKE_cmd + ["-G", CMAKE_gen, "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_SHARED_LIBS:BOOL=" + build_shared, "-DBUILD_STATIC_LIBS:BOOL=" + build_static, "-DCMAKE_INSTALL_PREFIX=" + env_dir + "/usr", ".."], cwd=build_dir + DIR_minivideo + "/minivideo/build")
        subprocess.check_call(["cmake", "--build", ".", "--config", "Release"], cwd=build_dir + DIR_minivideo + "/minivideo/build")
        subprocess.check_call(["cmake", "--build", ".", "--target", "install", "--config", "Release"], cwd=build_dir + DIR_minivideo + "/minivideo/build")
