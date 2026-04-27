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

srcs_windows_clipboard = [
    'winclipboard.h',
    'textconv.c',
    'thread.c',
    'wndproc.c',
    'xevents.c',
]

xwin_clipboard = static_library(
    'XWinclipboard',
    srcs_windows_clipboard,
    include_directories: inc,
    c_args: '-DHAVE_XWIN_CONFIG_H',
    dependencies: [
        dependency('xcb'),
        dependency('xcb-aux'),
        dependency('xcb-icccm'),
        dependency('xcb-xfixes'),
        socket_dep,
    ],
)

srcs_xwinclip = [
    'xwinclip.c',
    'debug.c',
]

executable(
    'xwinclip',
    srcs_xwinclip,
    dependencies: dependency('xcb'),
    link_with: xwin_clipboard,
    link_args: ['-lgdi32', '-lpthread'],
    install: true,
)

xwinclip_man = configure_file(
    input: 'xwinclip.man',
    output: 'xwinclip.1',
    configuration: manpage_config,
)
install_man(xwinclip_man)
