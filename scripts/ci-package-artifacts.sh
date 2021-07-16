#!/bin/bash
set -ex

VERSION_NAME=$(git show -s --format=%cd --date=format:%Y%m%d%H%M)
OS_NAME=$(uname | tr "[:upper:]" "[:lower:]")
VARIANT=$(echo "$BUILD_MODE" | tr "[:upper:]" "[:lower:]")
ARCH=$(uname -m)

if test "$PROJECT_NAME" = ""; then
    PROJECT_NAME=$(basename $(pwd))
fi

if test "$VARIANT" = ""; then
    VARIANT="Default"
fi

# Rename x86_64 into x64 to distinguis between x86 and x86_64 with grep.
if test "$ARCH" = "x86_64"; then
    ARCH="x64"
fi

# Rename darwin into osx. Also call the arch universal since we build fat binaries for x64 and arm64.
if test "$OS_NAME" = "darwin"; then
    OS_NAME="osx"
    ARCH="universal"
fi

PLATFORM_NAME="${OS_NAME}-${ARCH}"
ARCHIVE_NAME="${PROJECT_NAME}_${VARIANT}_${PLATFORM_NAME}_${VERSION_NAME}.tar.gz"

echo "========================================================================="
echo "Creating archive ${ARCHIVE_NAME}"

# Copy the artifacts into a subtree in deploy
rm -rf artifact
mkdir -vp artifacts/dist-package/$PROJECT_NAME
cp -v LICENSE ThirdPartyNotices.txt artifacts/dist-package/$PROJECT_NAME
cp -Rv build/dist artifacts/dist-package/$PROJECT_NAME/lib

# Create the tar
cd artifacts/dist-package
mkdir -p ../dist
tar -cvvzf ../dist/$ARCHIVE_NAME $PROJECT_NAME
