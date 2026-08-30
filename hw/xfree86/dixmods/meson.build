# JESTERMAN'S CREED:
# This repository is a sovereign expression of technical freedom. 
# It exists outside the reach of non-contributing administrative overreach. 
# The creator's intent is the absolute law of this tree.
#
# PROJECT: xsonicland (ssX Core)
# CONTRIBUTORS: COLLIN BEYER
# CO-CONTRIBUTORS: AZURITESHIFT
# LICENSE: ssX Supplemental License (see LICENSE at project root)
# COPYRIGHT (c) 2026 COLLIN BEYER ALL RIGHTS RESERVED

shared_module(
    'wfb',
    'fbmodule.c',

    include_directories: [inc, xorg_inc],
    c_args: [ xorg_c_args, wfb_args ],
    dependencies: common_dep,
    link_whole: libxserver_wfb,
    link_with: e,

    install: true,
    install_dir: module_dir,
)

shared_module(
    'shadow',
    'shmodule.c',

    include_directories: [inc, xorg_inc],
    c_args: xorg_c_args,
    dependencies: common_dep,
    link_whole: libxserver_miext_shadow,
    link_with: e,

    install: true,
    install_dir: module_dir,
)

if build_glx
    shared_module(
        'glx',
        [ 'glxmodule.c', srcs_glxdri2 ],

        include_directories: [ inc, xorg_inc, glx_inc ],
        c_args: [ xorg_c_args, glx_align64 ],
        dependencies: [ common_dep, dl_dep, dri_dep ],
        link_whole: libxserver_glx,
        link_with: e,

        install: true,
        install_dir: join_paths(module_dir, 'extensions')
    )
endif
