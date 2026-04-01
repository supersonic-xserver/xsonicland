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

srcs_render = [
    'animcur.c',
    'filter.c',
    'glyph.c',
    'matrix.c',
    'miindex.c',
    'mipict.c',
    'mirect.c',
    'mitrap.c',
    'mitri.c',
    'picture.c',
    'render.c',
]

hdrs_render = [
    'glyphstr.h',
    'mipict.h',
    'picture.h',
    'picturestr.h',
]

libxserver_render = static_library('libxserver_render',
    srcs_render,
    include_directories: inc,
    dependencies: common_dep,
)

if build_xorg
    install_data(hdrs_render, install_dir: xorgsdkdir)
endif
