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

inputtestdrv_srcs = [
    'xf86-input-inputtest.c',
]

shared_module(
    'inputtest_drv',
    inputtestdrv_srcs,
    name_prefix: '',

    include_directories: [inc, xorg_inc],
    c_args: xorg_c_args,
    dependencies: [common_dep],

    install: true,
    install_dir: join_paths(module_dir, 'input'),

    link_with: e,
)

install_man(configure_file(
    input: 'inputtestdrv.man',
    output: 'inputtestdrv.4',
    configuration: manpage_config,
))

install_data('xf86-input-inputtest-protocol.h', install_dir: xorgsdkdir)
