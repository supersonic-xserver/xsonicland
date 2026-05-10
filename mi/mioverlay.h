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



#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#ifndef __MIOVERLAY_H
#define __MIOVERLAY_H

typedef void (*miOverlayTransFunc) (ScreenPtr, int, BoxPtr);
typedef Bool (*miOverlayInOverlayFunc) (WindowPtr);

extern _X_EXPORT Bool

miInitOverlay(ScreenPtr pScreen,
              miOverlayInOverlayFunc inOverlay, miOverlayTransFunc trans);

extern _X_EXPORT Bool

miOverlayGetPrivateClips(WindowPtr pWin,
                         RegionPtr *borderClip, RegionPtr *clipList);

extern _X_EXPORT Bool miOverlayCollectUnderlayRegions(WindowPtr, RegionPtr *);
extern _X_EXPORT void miOverlayComputeCompositeClip(GCPtr, WindowPtr);
extern _X_EXPORT Bool miOverlayCopyUnderlay(ScreenPtr);
extern _X_EXPORT void miOverlaySetTransFunction(ScreenPtr, miOverlayTransFunc);
extern _X_EXPORT void miOverlaySetRootClip(ScreenPtr, Bool);

#endif                          /* __MIOVERLAY_H */