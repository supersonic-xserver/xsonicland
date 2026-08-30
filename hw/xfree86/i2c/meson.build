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

srcs_xorg_i2c = [
    'xf86i2c.c'
]

xorg_i2c = static_library('xorg_i2c',
    srcs_xorg_i2c,
    include_directories: [inc, xorg_inc],
    dependencies: common_dep,
    c_args: xorg_c_args,
)

install_data(['xf86i2c.h', 'i2c_def.h'], install_dir: xorgsdkdir)
