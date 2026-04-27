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

srcs_present = [
    'present.c',
    'present_event.c',
    'present_execute.c',
    'present_fake.c',
    'present_fence.c',
    'present_notify.c',
    'present_request.c',
    'present_scmd.c',
    'present_screen.c',
    'present_vblank.c',
]

hdrs_present = [
    'present.h',
    'presentext.h',
]

libxserver_present = static_library('libxserver_present',
    srcs_present,
    include_directories: inc,
    dependencies: [
        common_dep,
	epoll_dep,
        dependency('presentproto', version: '>= 1.2')
    ],
)

if build_xorg
    install_data(hdrs_present, install_dir: xorgsdkdir)
endif
