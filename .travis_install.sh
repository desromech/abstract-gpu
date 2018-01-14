#!/bin/sh

if "$TRAVIS_OS_NAME" == "linux"; then
    sudo apt-get -qq update || exit
    sudo apt-get -qqy install gcc-6-mulitlib g++-6-mulitlib libvulkan1 libvulkan-dev libvulkan1:i386 libvulkan-dev:i386 || exit 1
fi
