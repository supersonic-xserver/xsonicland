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

srcs_randr = [
    'randr.c',
    'rrcrtc.c',
    'rrdispatch.c',
    'rrinfo.c',
    'rrlease.c',
    'rrmode.c',
    'rrmonitor.c',
    'rroutput.c',
    'rrpointer.c',
    'rrproperty.c',
    'rrprovider.c',
    'rrproviderproperty.c',
    'rrscreen.c',
    'rrsdispatch.c',
    'rrtransform.c',
]

hdrs_randr = [
    'randrstr.h',
    'rrtransform.h',
]

if build_xinerama
    srcs_randr += 'rrxinerama.c'
endif

libxserver_randr = static_library('libxserver_randr',
    srcs_randr,
    include_directories: inc,
    dependencies: common_dep,
)

if build_xorg
    install_data(hdrs_randr, install_dir: xorgsdkdir)
endif
