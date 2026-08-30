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

if host_machine.system() == 'linux'
    srcs_fbdevhw = 'fbdevhw.c'
else
    srcs_fbdevhw = 'fbdevhwstub.c'
endif

shared_module('fbdevhw',
    srcs_fbdevhw,
    include_directories: [ inc, xorg_inc ],
    dependencies: common_dep,
    c_args: xorg_c_args,
    install: true,
    install_dir: module_dir,
    link_with: e,
)

install_data('fbdevhw.h', install_dir: xorgsdkdir)

install_man(configure_file(
    input: 'man/fbdevhw.man',
    output: 'fbdevhw.4',
    configuration: manpage_config,
))
