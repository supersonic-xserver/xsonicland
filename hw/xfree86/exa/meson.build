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

xorg_exa = shared_module('exa',
    'examodule.c',
    include_directories: [inc, xorg_inc],
    dependencies: common_dep,
    link_with: [libxserver_exa, e],
    c_args: xorg_c_args,
    install: true,
    install_dir: module_dir,
)

install_man(configure_file(
    input: 'man/exa.man',
    output: 'exa.4',
    configuration: manpage_config,
))
