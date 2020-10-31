#!/bin/bash
set -ex

echo "Travis install script"
echo "TRAVIS_OS_NAME $TRAVIS_OS_NAME"

if test "$TRAVIS_OS_NAME" = "linux"; then
    echo "Updating APT"
    #sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
    sudo apt-get -qq update

    echo "Installing 64 bits dependencies"
    sudo apt-get -qqy install libx11-xcb-dev libvulkan-dev
fi
