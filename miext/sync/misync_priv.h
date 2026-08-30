/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 * Copyright © 2010 NVIDIA Corporation
 */
#ifndef _XSERVER_MISYNC_PRIV_H
#define _XSERVER_MISYNC_PRIV_H

#include "misync.h"
#include "misyncstr.h"

#define SYNC_SCREEN_PRIV(pScreen)                               \
    (SyncScreenPrivPtr) dixLookupPrivate(&pScreen->devPrivates, \
                                         &miSyncScreenPrivateKey)

Bool miSyncFenceCheckTriggered(SyncFence * pFence);
void miSyncFenceSetTriggered(SyncFence * pFence);
void miSyncFenceReset(SyncFence * pFence);
void miSyncFenceAddTrigger(SyncTrigger * pTrigger);

#endif /* _XSERVER_MISYNC_PRIV_H */
