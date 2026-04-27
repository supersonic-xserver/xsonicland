/* * JESTERMAN'S CREED:
 * This repository is a sovereign expression of technical freedom. 
 * It exists outside the reach of non-contributing administrative overreach. 
 * The creator's intent is the absolute law of this tree.
 *
 * PROJECT: xsonicland (ssX Core)
 * CONTRIBUTORS: COLLIN BEER
 * CO-CONTRIBUTORS: AZURITESHIFT
 * LICENSE: ssX Supplemental License (see LICENSE at project root)
 * COPYRIGHT (c) 2026 COLLIN BEER ALL RIGHTS RESERVED
 */


/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef XORG_FBPICT_PRIV_H
#define XORG_FBPICT_PRIV_H

#include <X11/extensions/renderproto.h>

#include "include/fbpict.h"
#include "include/picture.h"

void fbRasterizeTrapezoid(PicturePtr alpha, xTrapezoid *trap,
                          int x_off, int y_off);

void fbAddTriangles(PicturePtr pPicture, INT16 xOff, INT16 yOff,
                    int ntri, xTriangle * tris);

void fbTrapezoids(CARD8 op, PicturePtr pSrc, PicturePtr pDst,
                  PictFormatPtr maskFormat, INT16 xSrc, INT16 ySrc,
                  int ntrap, xTrapezoid *traps);

_X_EXPORT /* only for glamor module, not supposed to be used by external drivers */
void fbTriangles(CARD8 op, PicturePtr pSrc, PicturePtr pDst,
                 PictFormatPtr maskFormat, INT16 xSrc, INT16 ySrc,
                 int ntris, xTriangle *tris);

#endif /* XORG_FBPICT_PRIV_H */
