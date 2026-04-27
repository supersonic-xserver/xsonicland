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

srcs = [
    'Args.c',
    'Color.c',
    'Cursor.c',
    'Display.c',
    'Events.c',
    'Font.c',
    'GC.c',
    'GCOps.c',
    'Handlers.c',
    'Init.c',
    'Keyboard.c',
    'Pixmap.c',
    'Pointer.c',
    'Screen.c',
    'Visual.c',
    'Window.c',
    '../../mi/miinitext.c',
    '../../mi/miinitext.h',
]

executable(
    'Xnest',
    srcs,
    include_directories: inc,
    dependencies: [
        common_dep,
        xnest_dep,
    ],
    link_with: [
        libxserver_main,
        libxserver,
        libxserver_xi_stubs,
        libxserver_xkb_stubs,
    ],
    c_args: '-DHAVE_XNEST_CONFIG_H',
    install: true,
)

install_man(configure_file(
    input: 'man/Xnest.man',
    output: 'Xnest.1',
    configuration: manpage_config,
))
