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


srcs_xorg_x86emu = [
    'debug.c',
    'decode.c',
    'fpu.c',
    'ops2.c',
    'ops.c',
    'prim_ops.c',
    'sys.c',
]

xorg_x86emu = static_library('x86emu',
    srcs_xorg_x86emu,
    include_directories: [inc, xorg_inc],
    dependencies: common_dep,
)
