$VERSION_NAME = $(git show -s --format=%cd --date=format:%Y%m%d%H%M)
$PROJECT_NAME = "abstract-gpu"
$OS_NAME = "windows"
$ARCH = $Env:PLATFORM
$BINTRAY_APIKEY = $Env:BINTRAY_APIKEY
$PLATFORM_NAME = "${OS_NAME}-${ARCH}"
$ALLOWED_REPOSITORY="ronsaldo/abstract-gpu"
if(!$ARCH) {$ARCH = "x86"}

# Make sure this is a branch whose deployment is allowed.
if($Env:APPVEYOR_REPO_NAME -ne $ALLOWED_REPOSITORY)
{
    echo "Trying to deploy in repository: $Env:APPVEYOR_REPO_NAME. Skipping."
    exit
}

if($Env:APPVEYOR_PULL_REQUEST_HEAD_COMMIT)
{
    echo "Skipping a deployment with the script provider because PRs are not permitted."
    exit
}

if($Env:APPVEYOR_REPO_BRANCH -ne "master" && (!$APPVEYOR_REPO_TAG_NAME))
{
    echo "Skipping a deployment with the script provider because this branch is not permitted."
    exit
}

function deploy_variant($VARIANT)
{
    $LOWER_VARIANT=$VARIANT.ToLower()
    $ARCHIVE_NAME="${PROJECT_NAME}_${LOWER_VARIANT}_${PLATFORM_NAME}_${VERSION_NAME}.zip"
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
    # Compress-Archive -Path $PROJECT_NAME -DestinationPath $ARCHIVE_NAME
    7z a -tzip $ARCHIVE_NAME $PROJECT_NAME

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
