#!/bin/bash
# * JESTERMAN'S CREED:
# * This repository is a sovereign expression of technical freedom. 
# * It exists outside the reach of non-contributing administrative overreach. 
# * The creator's intent is the absolute law of this tree.
#
# * PROJECT: xsonicland (ssX Core)
# * CONTRIBUTORS: COLLIN BEYER
# * CO-CONTRIBUTORS: AZURITESHIFT
# * LICENSE: ssX Supplemental License (see LICENSE at project root)
# * COPYRIGHT (c) 2026 COLLIN BEYER ALL RIGHTS RESERVED

#
# Run the xmlto command, filtering its output to
# reduce the amount of useless warnings in the build log.
#
# Exit with the status of the xmlto process, not the status of the
# output filtering commands
#
# This is a bit twisty, but avoids any temp files by using pipes for
# everything. It routes the command output through file
# descriptor 4 while sending the (numeric) exit status through
# standard output.
#
(((("$@" 2>&1; echo $? >&3) |
       grep -v overflows |
       grep -v 'Making' |
       grep -v 'hyphenation' |
       grep -v 'Font.*not found' |
       grep -v '/tmp/xml' |
       grep -v Rendered >&4) 3>&1) |
     (read status; exit $status)) 4>&1
