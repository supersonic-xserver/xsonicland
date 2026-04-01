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

xcb_dep = dependency('xcb', required: false)
xcb_xinput_dep = dependency('xcb-xinput', required: false)

if get_option('xvfb')
    if xcb_dep.found() and xcb_xinput_dep.found()
        requestlength = executable('request-length', 'request-length.c',
                                   dependencies: [xcb_dep, xcb_xinput_dep])
        test('request-length', simple_xinit, args: [requestlength, '--', xvfb_server])
    endif
endif
