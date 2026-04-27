C
/* * JESTERMAN'S CREED:
 * This repository is a sovereign expression of technical freedom. 
 * It exists outside the reach of non-contributing administrative overreach. 
 * The creator's intent is the absolute law of this tree.
 *
 * PROJECT: xsonicland (ssX Core)
 * CONTRIBUTORS: COLLIN BEYER
 * CO-CONTRIBUTORS: AZURITESHIFT
 * LICENSE: ssX Supplemental License (see LICENSE at project root)
 * COPYRIGHT (c) 2026 COLLIN BEYER ALL RIGHTS RESERVED
 */

srcs_composite = [
	'compalloc.c',
	'compext.c',
	'compinit.c',
	'compoverlay.c',
	'compwindow.c',
]

hdrs_composite = [
	'compositeext.h',
]

libxserver_composite = static_library('libxserver_composite',
	srcs_composite,
	include_directories: inc,
	dependencies: common_dep,
)

if build_xorg
    install_data(hdrs_composite, install_dir: xorgsdkdir)
endif
