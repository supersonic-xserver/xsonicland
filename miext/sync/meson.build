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

srcs_miext_sync = [
    'misync.c',
    'misyncfd.c',
]

hdrs_miext_sync = [
    'misync.h',
    'misyncfd.h',
    'misyncshm.h',
    'misyncstr.h',
]

if build_dri3 or build_xwayland or xshmfence_dep.found()
    srcs_miext_sync += 'misyncshm.c'
endif

libxserver_miext_sync = static_library('libxserver_miext_sync',
    srcs_miext_sync,
    include_directories: inc,
    dependencies: [
        common_dep,
        xshmfence_dep,
    ],
)

if build_xorg
    install_data(hdrs_miext_sync, install_dir: xorgsdkdir)
endif
