#!/bin/bash
VERSION_NAME=$(git show -s --format=%cd --date=format:%Y%m%d%H%M)
PROJECT_NAME="abstract-gpu"
OS_NAME=$(uname | tr "[:upper:]" "[:lower:]")
VARIANT=$(echo "$BUILD_MODE" | tr "[:upper:]" "[:lower:]")
ARCH=$(uname -m)

# Rename darwin into osx
if test "$OS_NAME" = "darwin"; then
    OS_NAME="osx"
fi

PLATFORM_NAME="${OS_NAME}-${ARCH}"
ARCHIVE_NAME="${PROJECT_NAME}_${VARIANT}_${PLATFORM_NAME}_${VERSION_NAME}.tar.gz"

echo "========================================================================="
echo "Creating archive ${ARCHIVE_NAME}"

# Copy the artifacts into a subtree in deploy
rm -rf deploy
mkdir -vp deploy/$PROJECT_NAME || exit 1
cp -Rv build/dist deploy/$PROJECT_NAME/lib || exit 1
cp -Rv include deploy/$PROJECT_NAME/include || exit 1
cd deploy

# Create the tar
tar -cvvzf $ARCHIVE_NAME $PROJECT_NAME

if test "$BINTRAY_APIKEY" != ""; then
    echo "===================================================================="
    echo "Uploading archive $ARCHIVE_NAME into bintray version $VERSION_NAME"
    curl -T "$ARCHIVE_NAME" -uronsaldo:$BINTRAY_APIKEY "https://api.bintray.com/content/ronsaldo/abstract-gpu/lib/${VERSION_NAME}/${ARCHIVE_NAME}?publish=1"
    echo
fi

