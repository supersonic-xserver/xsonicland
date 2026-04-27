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


# this times out on Travis, because the tests take too long.
#if test "x$TRAVIS_BUILD_DIR" != "x"; then
#    exit 77
#fi

export SERVER_COMMAND="$XSERVER_BUILDDIR/hw/vfb/Xvfb \
        -noreset \
        -screen scrn 1280x1024x24"
export PIGLIT_RESULTS_DIR=$XSERVER_BUILDDIR/test/piglit-results/xvfb

exec $XSERVER_DIR/test/scripts/run-piglit.sh
