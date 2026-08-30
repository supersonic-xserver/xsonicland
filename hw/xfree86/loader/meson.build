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

srcs_xorg_loader = [
    'loader.c',
    'loadmod.c',
]

xorg_loader = static_library('xorg_loader',
    srcs_xorg_loader,
    include_directories: [inc, xorg_inc],
    dependencies: [common_dep, dl_dep],
    c_args: xorg_c_args,
)

xorg_symbol_test = executable('xorg_symbol_test',
    'symbol-test.c',
    dependencies: dl_dep,
)
