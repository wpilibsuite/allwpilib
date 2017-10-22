#!/bin/bash

if [[ $TRAVIS_OS_NAME != 'osx' ]]; then
    python3.5 -m wpiformat -y 2017 -clang 5.0
else
    python3 -m wpiformat -y 2017
fi

git --no-pager diff --exit-code HEAD  # Ensure formatter made no changes
