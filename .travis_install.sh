#!/bin/sh

echo "Travis install script"
echo "TRAVIS_OS_NAME $TRAVIS_OS_NAME"

if test "$TRAVIS_OS_NAME" = "linux"; then
    echo "Updating APT"

    sudo apt-get -qq update || exit 1

    echo "Installing dependencies"
    sudo apt-get -qqy install gcc-6-mulitlib g++-6-mulitlib libvulkan1 libvulkan-dev libvulkan1:i386 libvulkan-dev:i386 || exit 1
fi
