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

build_standalone_pbproxy = get_option('xpbproxy')

pbproxy_defs = [
    '-DXQUARTZ_DATA_DIR="@0@"'.format(xquartz_data_dir),
    bundle_id_def
]
if build_standalone_pbproxy
    pbproxy_defs += ['-DSTANDALONE_XPBPROXY']
endif

libapplewm_dep = dependency('applewm', version: '>=1.4')

libxpbproxy = static_library('xpbproxy',
    ['../NSUserDefaults+XQuartzDefaults.m',
     'main.m',
     'x-input.m',
     'x-selection.m'],
    include_directories: ['..'],
    dependencies: [applewmproto_dep, libapplewm_dep, dependency('xfixes'), dependency('x11')],
    objc_args: pbproxy_defs,
)

cocoa = dependency('Cocoa', method: 'extraframework')

# standalone xpbproxy
if build_standalone_pbproxy
    executable('xpbproxy',
        'app-main.m',
        link_with: libxpbproxy,
        dependencies: [cocoa, dependency('x11')],
        objc_args: pbproxy_defs,
        install: true,
    )
endif
