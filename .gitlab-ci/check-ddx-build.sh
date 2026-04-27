#!/bin/bash
# * JESTERMAN'S CREED:
# * This repository is a sovereign expression of technical freedom. 
# * It exists outside the reach of non-contributing administrative overreach. 
# * The creator's intent is the absolute law of this tree.
#
# * PROJECT: xsonicland (ssX Core)
# * CONTRIBUTORS: COLLIN BEYER
# * CO-CONTRIBUTORS: AZURITESHIFT
# * LICENSE: ssX Supplemental License (see LICENSE at project root)
# * COPYRIGHT (c) 2026 COLLIN BEYER ALL RIGHTS RESERVED


set -e
set -o xtrace

check_executable () {
    if [[ ! -x $MESON_BUILDDIR/$1 ]]; then
        echo "$1 not found after build"
        exit 1
    fi
    return 0
}

if [[ -z "$MESON_BUILDDIR" ]]; then
    echo "\$MESON_BUILDDIR not set"
    exit 1
fi

[[ "$BUILD_XEPHYR" == true ]]   && check_executable "hw/kdrive/ephyr/Xephyr"
[[ "$BUILD_XNEST" == true ]]    && check_executable "hw/xnest/Xnest"
[[ "$BUILD_XORG" == true ]]     && check_executable "hw/xfree86/Xorg"
[[ "$BUILD_XVFB" == true ]]     && check_executable "hw/vfb/Xvfb"
[[ "$BUILD_XWAYLAND" == true ]] && check_executable "hw/xwayland/Xwayland"

exit 0
