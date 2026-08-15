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

glamoregl_src = [
    '../../../glamor/glamor_egl.c',
    '../../../glamor/glamor_eglmodule.c',
    'glamor_xf86_xv.c',
]

shared_module(
    'glamoregl',
    glamoregl_src,

    include_directories: [inc, xorg_inc],
    c_args: xorg_c_args,
    dependencies: [
        common_dep,
        dependency('libdrm', version: '>= 2.4.46'),
        gbm_dep,
    ],
    link_with: [glamor, libxserver_glx],

    install: true,
    install_dir: module_dir,
)
