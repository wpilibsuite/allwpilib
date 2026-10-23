#!/usr/bin/env bash

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

# Used only when an exact-commit baseline build artifact is unavailable.
set -euo pipefail
scripts=$1/.github/scripts
output=$2
platform=$3
options=()
if [ "$platform" = windowsarm64 ]; then
  options+=(-Pbuildwinarm64)
fi
failed=0
for configuration in release debug; do
  if ./gradlew --init-script "$scripts/compatibility.init.gradle" \
    compatibilitySnapshot --build-cache -PbuildServer -PskipJavaFormat "-Ponly$platform" "${options[@]}" \
    "-PcompatibilityPlatform=$platform" "-PcompatibilityConfiguration=$configuration" \
    "-PcompatibilityOutput=$output/$configuration"; then
    if [ "$platform" = osxuniversal ]; then
      # A failed debug-link must not leave a reusable, apparently complete snapshot.
      rm "$output/$configuration/complete"
      if python3 "$scripts/compatibility.py" prepare-macos --snapshot "$output/$configuration"; then
        echo complete > "$output/$configuration/complete"
      else
        failed=1
      fi
    fi
  else
    failed=1
  fi
done
exit "$failed"
