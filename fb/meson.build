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

srcs_fb = [
	'fballpriv.c',
	'fbarc.c',
	'fbbits.c',
	'fbblt.c',
	'fbbltone.c',
	'fbcmap_mi.c',
	'fbcopy.c',
	'fbfill.c',
	'fbfillrect.c',
	'fbfillsp.c',
	'fbgc.c',
	'fbgetsp.c',
	'fbglyph.c',
	'fbimage.c',
	'fbline.c',
	'fboverlay.c',
	'fbpict.c',
	'fbpixmap.c',
	'fbpoint.c',
	'fbpush.c',
	'fbscreen.c',
	'fbseg.c',
	'fbsetsp.c',
	'fbsolid.c',
	'fbtile.c',
	'fbtrap.c',
	'fbutil.c',
	'fbwindow.c',
]

libxserver_fb = static_library('xserver_fb',
	srcs_fb,
	include_directories: inc,
	dependencies: common_dep,
	pic: true,
)

wfb_args = '-DFB_ACCESS_WRAPPER'

libxserver_wfb = static_library('xserver_wfb',
	srcs_fb,
	c_args: wfb_args,
	include_directories: inc,
	dependencies: common_dep,
	pic: true,
	build_by_default: false,
)
