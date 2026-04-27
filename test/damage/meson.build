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
xcb_damage_dep = dependency('xcb-damage', required: false)

if get_option('xvfb')
    if xcb_dep.found() and xcb_damage_dep.found()
        damage_primitives = executable('damage-primitives', 'primitives.c', dependencies: [xcb_dep, xcb_damage_dep])
        test('damage-primitives', simple_xinit, args: [damage_primitives, '--', xvfb_server])
    endif
endif
