$VERSION_NAME = $(git show -s --format=%cd --date=format:%Y%m%d%H%M)
$PROJECT_NAME = "abstract-gpu"
$OS_NAME = "windows"
$ARCH = $Env:PLATFORM
$BINTRAY_APIKEY = $Env:BINTRAY_APIKEY
$PLATFORM_NAME = "${OS_NAME}-${ARCH}"
if(!$ARCH) {$ARCH = "x86"}

function deploy_variant($VARIANT)
{
    $ARCHIVE_NAME="${PROJECT_NAME}_${VARIANT}_${PLATFORM_NAME}_${VERSION_NAME}.zip"
    $BUILD_DIR="builds/$OS_NAME-$ARCH/dist/$VARIANT"

    echo "========================================================================="
    echo "Creating archive ${ARCHIVE_NAME}"

    # Copy the artifacts into a subtree in deploy
    cd $PSScriptRoot\..
    if (Test-Path deploy) { rm -r -fo deploy }
    mkdir deploy/$PROJECT_NAME > $null
    cp -R $BUILD_DIR deploy/$PROJECT_NAME/lib
    cp -R include deploy/$PROJECT_NAME/include
    cd deploy

    # Create the zip
    Compress-Archive -Path $PROJECT_NAME -DestinationPath $ARCHIVE_NAME

    if($BINTRAY_APIKEY)
    {
        $secpassword = ConvertTo-SecureString "$BINTRAY_APIKEY" -AsPlainText -Force
        $credential = New-Object System.Management.Automation.PSCredential("ronsaldo", $secpassword)

        echo "===================================================================="
        echo "Uploading archive $ARCHIVE_NAME into bintray version $VERSION_NAME"
        Invoke-RestMethod -InFile "$ARCHIVE_NAME" -Method Put -Credential $credential -Uri "https://api.bintray.com/content/ronsaldo/abstract-gpu/lib/${VERSION_NAME}/${ARCHIVE_NAME}?publish=1"
    }
}

deploy_variant("Debug")
deploy_variant("RelWithDebInfo")
deploy_variant("Release")
