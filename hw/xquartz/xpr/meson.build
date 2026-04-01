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

libXquartzXpr = static_library('XquartzXpr',
    [
     'appledri.c',
     'dri.c',
     'driWrap.c',
     'xprAppleWM.c',
     'xprCursor.c',
     'xprEvent.c',
     'xprFrame.c',
     'xprScreen.c',
     'x-hash.c',
     'x-hook.c',
     'x-list.c',
    ],
    include_directories: [inc, '..', '../../../pseudoramiX', '../../../miext/rootless'],
    c_args: [bundle_id_def, '-DXQUARTZ'],
    dependencies: [xproto_dep, pixman_dep],
)
