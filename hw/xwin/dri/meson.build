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

srcs_windows_dri = [
    'windowsdri.c',
    'windowsdri.h',
]

xwin_windowsdri = static_library(
    'WindowsDRI',
    srcs_windows_dri,
    include_directories: [ inc, include_directories('../') ],
    dependencies: [
        windowsdri_dep,
        pixman_dep,
    ],
)
