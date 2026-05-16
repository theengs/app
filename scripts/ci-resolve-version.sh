#!/usr/bin/env bash
# Resolves the marketing version from a v* tag and patches all source files
# that hold it as a literal string. No-op on non-tag refs.
#
# Usage: bash scripts/ci-resolve-version.sh <platform>
#   platform = android | ios | linux | macos | windows

set -euo pipefail

PLATFORM="${1:-}"

if [[ "${GITHUB_REF:-}" != refs/tags/v* ]]; then
  echo "Not a v* tag (GITHUB_REF=${GITHUB_REF:-<unset>}); keeping committed version."
  exit 0
fi

VERSION="${GITHUB_REF#refs/tags/v}"
echo "Applying marketing version: ${VERSION} (platform: ${PLATFORM:-<unspecified>})"

# CMakeLists.txt feeds iOS / macOS Xcode marketing version via CMAKE_PROJECT_VERSION.
sed -i.bak -E 's/(project\(Theengs VERSION )[0-9]+\.[0-9]+\.[0-9]+/\1'"${VERSION}"'/' CMakeLists.txt

case "${PLATFORM}" in
  android)
    sed -i.bak -E 's/(versionName ")[^"]+/\1'"${VERSION}"'/' assets/android/build.gradle
    ;;
  ios)
    /usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString ${VERSION}" assets/ios/Info.plist
    ;;
  macos)
    /usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString ${VERSION}" assets/macos/Info.plist
    sed -i.bak -E "s/^export APP_VERSION=.*/export APP_VERSION=${VERSION}/" deploy_macos.sh
    ;;
  linux)
    sed -i.bak -E "s/^export APP_VERSION=.*/export APP_VERSION=${VERSION}/" deploy_linux.sh
    ;;
  windows)
    sed -i.bak -E "s/^export APP_VERSION=.*/export APP_VERSION=${VERSION}/" deploy_windows.sh
    ;;
  "")
    echo "warning: no platform passed; only CMakeLists.txt was patched"
    ;;
  *)
    echo "error: unknown platform '${PLATFORM}'" >&2
    exit 1
    ;;
esac

if [[ -n "${GITHUB_ENV:-}" ]]; then
  echo "APP_VERSION=${VERSION}" >> "${GITHUB_ENV}"
fi

find . -maxdepth 4 -name '*.bak' -type f -delete 2>/dev/null || true
echo "Done."
