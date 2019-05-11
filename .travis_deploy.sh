#!/bin/bash
VERSION_NAME=$(git show -s --format=%cd --date=format:%Y%m%d%H%M)
PROJECT_NAME="abstract-gpu"
ALLOWED_REPOSITORY="ronsaldo/abstract-gpu"
OS_NAME=$(uname | tr "[:upper:]" "[:lower:]")
VARIANT=$(echo "$BUILD_MODE" | tr "[:upper:]" "[:lower:]")
ARCH=$(uname -m)

# Make sure this is a branch whose deployment is allowed.
if [[ "${TRAVIS_REPO_SLUG}" != "ronsaldo/abstract-gpu" ]]; then
  echo "Trying to deploy in repository: ${REPO_NAME}. Skipping."
  exit
fi

if [[ -n "${TRAVIS_PULL_REQUEST_SHA}" ]]; then
  echo "Skipping a deployment with the script provider because PRs are not permitted."
  exit
fi

if [[ "${TRAVIS_BRANCH}" != "master" ]] && [[ -z "${TRAVIS_TAG}" ]]; then
  echo "Skipping a deployment with the script provider because this branch is not permitted."
  exit
fi

# Rename darwin into osx
if test "$OS_NAME" = "darwin"; then
    OS_NAME="osx"
fi

# Rename x86_64 into x64 to distinguis between x86 and x86_64 with grep.
if test "$ARCH" = "x86_64"; then
    ARCH="x64"
fi

PLATFORM_NAME="${OS_NAME}-${ARCH}"
ARCHIVE_NAME="${PROJECT_NAME}_${VARIANT}_${PLATFORM_NAME}_${VERSION_NAME}.tar.gz"

echo "========================================================================="
echo "Creating archive ${ARCHIVE_NAME}"

# Copy the artifacts into a subtree in deploy
rm -rf deploy
mkdir -vp deploy/$PROJECT_NAME || exit 1
cp -v LICENSE ThirdPartyNotices.txt deploy/$PROJECT_NAME || exit 1
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
