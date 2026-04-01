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

srcs_miext_shadow = [
    'shadow.c',
    'sh3224.c',
    'shafb4.c',
    'shafb8.c',
    'shiplan2p4.c',
    'shiplan2p8.c',
    'shpacked.c',
    'shplanar8.c',
    'shplanar.c',
    'shrot16pack_180.c',
    'shrot16pack_270.c',
    'shrot16pack_270YX.c',
    'shrot16pack_90.c',
    'shrot16pack_90YX.c',
    'shrot16pack.c',
    'shrot32pack_180.c',
    'shrot32pack_270.c',
    'shrot32pack_90.c',
    'shrot32pack.c',
    'shrot8pack_180.c',
    'shrot8pack_270.c',
    'shrot8pack_90.c',
    'shrot8pack.c',
    'shrotate.c',
]

hdrs_miext_shadow = [
    'shadow.h',
]

libxserver_miext_shadow = static_library('libxserver_miext_shadow',
    srcs_miext_shadow,
    include_directories: inc,
    dependencies: common_dep,
)

if build_xorg
    install_data(hdrs_miext_shadow, install_dir: xorgsdkdir)
endif
