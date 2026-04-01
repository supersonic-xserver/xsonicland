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

srcs_kdrive = [
    'kcmap.c',
    'kdrive.c',
    'kinfo.c',
    'kinput.c',
    'kshadow.c',
    '../../../mi/miinitext.c',
    '../../../mi/miinitext.h',
]

if build_xv
    srcs_kdrive += 'kxv.c'
endif

kdrive = static_library('kdrive',
    srcs_kdrive,
    include_directories: inc,
    dependencies: common_dep,
    link_with: libxserver_miext_shadow,
)
