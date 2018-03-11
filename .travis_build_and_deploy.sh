#!/bin/bash

./.travis_build.sh || exit 1
./.travis_deploy.sh || exit 1
