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

srcs_xorg_dri = [
    'dri.c',
    'xf86dri.c',
]

xorg_dri = static_library('xorg_dri',
    srcs_xorg_dri,
    include_directories: [inc, xorg_inc],
    dependencies: [
        common_dep,
        libdrm_dep,
    ],
    c_args: xorg_c_args,
)

install_data(
    [
        'dri.h',
        'sarea.h',
        'dristruct.h',
    ],
    install_dir: xorgsdkdir,
)
