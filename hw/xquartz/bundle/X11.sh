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


set "$(dirname "$0")"/X11.bin "${@}"

if [ -x ~/.x11run ]; then
	exec ~/.x11run "${@}"
fi

case $(basename "${SHELL}") in
	bash)          exec -l "${SHELL}" --login -c 'exec "${@}"' - "${@}" ;;
	ksh|sh|zsh)    exec -l "${SHELL}" -c 'exec "${@}"' - "${@}" ;;
	csh|tcsh)      exec -l "${SHELL}" -c 'exec $argv:q' "${@}" ;;
	es|rc)         exec -l "${SHELL}" -l -c 'exec $*' "${@}" ;;
	*)             exec    "${@}" ;;
esac
