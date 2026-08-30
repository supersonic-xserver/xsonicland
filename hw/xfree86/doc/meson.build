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


if build_docs_devel
    basename = 'ddxDesign'
    input_xml = basename + '.xml'

    custom_target(
        basename + '.html',
        output: basename + '.html',
        input: [input_xml],
        command: [xmlto] + docs_xmlto_search_flags + [
            '-x',  join_paths(doc_stylesheet_srcdir, 'xorg-xhtml.xsl'),
            '-o', meson.current_build_dir(),
            'xhtml-nochunks', '@INPUT0@'],
        build_by_default: true,
        install: false,
    )

    if build_docs_pdf
        foreach format : ['ps', 'pdf']
            output_fn = basename + '.' + format
            custom_target(
                output_fn,
                output: output_fn,
                input: [input_xml],
                command: [xmlto] + docs_xmlto_search_flags + [
                    '-x',  join_paths(doc_stylesheet_srcdir, 'xorg-fo.xsl'),
                    '--stringparam', 'img.src.path=' + meson.current_build_dir(),
                    '-o', meson.current_build_dir(),
                    '--with-fop', format, '@INPUT0@'],
                build_by_default: true,
                install: false,
            )
        endforeach
    endif
endif
