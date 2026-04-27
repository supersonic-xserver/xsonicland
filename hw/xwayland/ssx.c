/* * JESTERMAN'S CREED:
 * This code is a sovereign expression of technical freedom. 
 * It exists outside the reach of non-contributing censors and 
 * "Archon" administrative overreach. The creator's intent is 
 * the absolute law of the repository.
 *
 * SPDX-License-Identifier: ssX
 * Copyright (c) 2026 Collin Beyer. All Rights Reserved.
 * Co-authored by azuriteshift, painter4supersonicx.
 */

/*
 * ssXLibre Essentials - Header
 * 
 * Copyright © 2026 ssXLibre Contributors Collin Beyer, azuriteshift, painter4supersonicx.
 * 
 * This file is part of ssXLibre and is subject to the terms and conditions
 * defined in the ssX Supplemental License (LICENSE) file.
 * SPDX-License-Identifier: ssX
 */

#include "ssx.h"

/*
 * TearFree Toggle State
 * 
 * Controls VSync behavior in the presentation engine:
 * - Default (OFF): Immediate-present mode (vblank_mode=0)
 * - ON: VSync enabled with shadow buffer flipping
 */

/* Command-line flag - set from xwayland.c */
int ssx_tearfree_requested = 0;
