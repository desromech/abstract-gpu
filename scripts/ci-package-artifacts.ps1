$ErrorActionPreference = "Stop"

$VERSION_NAME = $(git show -s --format=%cd --date=format:%Y%m%d%H%M)
$BUILD_MODE = $Env:BUILD_MODE
$PLATFORM_NAME = $Env:PLATFORM_NAME
$PROJECT_NAME = $Env:PROJECT_NAME

# Copy the artifacts into a subtree in deploy
cd $PSScriptRoot\..

if(!$BUILD_MODE) {$BUILD_MODE = "Debug"}
if(!$PLATFORM_NAME) {$PLATFORM_NAME = "windows-x86"}
if(!$PROJECT_NAME) {$PROJECT_NAME = $(Get-Item ".").Name}

$VARIANT=$BUILD_MODE.ToLower()

$BUILD_DIR="build/dist/$BUILD_MODE"

$ARCHIVE_NAME="${PROJECT_NAME}_${VARIANT}_${PLATFORM_NAME}_${VERSION_NAME}.zip"

echo "========================================================================="
echo "Creating archive ${ARCHIVE_NAME}"

$DEST="artifacts/dist-package/$PROJECT_NAME"

if (Test-Path artifacts) { rm -r -fo artifacts }
mkdir $DEST > $null
cp LICENSE $DEST
cp ThirdPartyNotices.txt $DEST
cp -R $BUILD_DIR $DEST/lib
mkdir "artifacts/dist" > $null

# Create the zip
Compress-Archive -Path $DEST -DestinationPath "artifacts/dist/$ARCHIVE_NAME"
