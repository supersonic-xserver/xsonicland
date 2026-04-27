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

#ifndef SSX_H
#define SSX_H

/*
 * ssXLibre TearFree Toggle
 * 
 * Controls VSync behavior in the presentation engine:
 * - Default (OFF): Immediate-present mode, frames pushed as fast as GPU renders
 * - ON (-tearfree flag or SSX_TEARFREE_ENABLE atom): VSync enabled, shadow buffer flipping
 */

/* Command-line flag state (set from xwayland.c) */
extern int ssx_tearfree_requested;

#endif
