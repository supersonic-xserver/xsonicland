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


/*
 * Copyright © 2011-2014 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of the
 * copyright holders not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include <xwayland-config.h>

#include <math.h>

#include <X11/Xatom.h>
#include "randrstr_priv.h"

#include "xwayland-cvt.h"
#include "xwayland-output.h"
#include "xwayland-screen.h"
#include "xwayland-window.h"

#include "xdg-output-unstable-v1-client-protocol.h"

static void xwl_output_get_xdg_output(struct xwl_output *xwl_output);

static Rotation
wl_transform_to_xrandr(enum wl_output_transform transform)
{
    switch (transform) {
    default:
    case WL_OUTPUT_TRANSFORM_NORMAL:
        return RR_Rotate_0;
    case WL_OUTPUT_TRANSFORM_90:
        return RR_Rotate_90;
    case WL_OUTPUT_TRANSFORM_180:
        return RR_Rotate_180;
    case WL_OUTPUT_TRANSFORM_270:
        return RR_Rotate_270;
    case WL_OUTPUT_TRANSFORM_FLIPPED:
        return RR_Reflect_X | RR_Rotate_0;
    case WL_OUTPUT_TRANSFORM_FLIPPED_90:
        return RR_Reflect_X | RR_Rotate_90;
    case WL_OUTPUT_TRANSFORM_FLIPPED_180:
        return RR_Reflect_X | RR_Rotate_180;
    case WL_OUTPUT_TRANSFORM_FLIPPED_270:
        return RR_Reflect_X | RR_Rotate_270;
    }
}

static int
wl_subpixel_to_xrandr(int subpixel)
{
    switch (subpixel) {
    default:
    case WL_OUTPUT_SUBPIXEL_UNKNOWN:
        return SubPixelUnknown;
    case WL_OUTPUT_SUBPIXEL_NONE:
        return SubPixelNone;
    case WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB:
        return SubPixelHorizontalRGB;
    case WL_OUTPUT_SUBPIXEL_HORIZONTAL_BGR:
        return SubPixelHorizontalBGR;
    case WL_OUTPUT_SUBPIXEL_VERTICAL_RGB:
        return SubPixelVerticalRGB;
    case WL_OUTPUT_SUBPIXEL_VERTICAL_BGR:
        return SubPixelVerticalBGR;
    }
}

static void
output_handle_geometry(void *data, struct wl_output *wl_output, int x, int y,
                       int physical_width, int physical_height, int subpixel,
                       const char *make, const char *model, int transform)
{
    struct xwl_output *xwl_output = data;

    if (xwl_output->randr_output) {
        RROutputSetPhysicalSize(xwl_output->randr_output,
                                physical_width, physical_height);
        RROutputSetSubpixelOrder(xwl_output->randr_output,
                                 wl_subpixel_to_xrandr(subpixel));
    }

    /* Apply the change from wl_output only if xdg-output is not supported */
    if (!xwl_output->xdg_output) {
        xwl_output->logical_x = x;
        xwl_output->logical_y = y;
    }
    xwl_output->rotation = wl_transform_to_xrandr(transform);
}

static void
output_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags,
                   int width, int height, int refresh)
{
    struct xwl_output *xwl_output = data;

    if (!(flags & WL_OUTPUT_MODE_CURRENT))
        return;

    /* Apply the change from wl_output only if xdg-output is not supported */
    if (!xwl_output->xdg_output) {
        xwl_output->logical_w = width;
        xwl_output->logical_h = height;
    }

    xwl_output->mode_width = width;
    xwl_output->mode_height = height;
    xwl_output->refresh = refresh;
}

static void
output_get_logical_mode(struct xwl_output *xwl_output, int *width, int *height)
{
    /* When we have xdg-output support the stored size is already rotated. */
    if (xwl_output->xdg_output == NULL ||
        (xwl_output->rotation & (RR_Rotate_0 | RR_Rotate_180))) {
        *width = xwl_output->logical_w;
        *height = xwl_output->logical_h;
    } else {
        *width = xwl_output->logical_h;
        *height = xwl_output->logical_w;
    }
}

void
output_get_logical_extents(struct xwl_output *xwl_output, int *width, int *height)
{
    /* When we have xdg-output support the stored size is already rotated. */
    if (xwl_output->xdg_output ||
        (xwl_output->rotation & (RR_Rotate_0 | RR_Rotate_180))) {
        *width = xwl_output->logical_w;
        *height = xwl_output->logical_h;
    } else {
        *width = xwl_output->logical_h;
        *height = xwl_output->logical_w;
    }
}

/**
 * Decides on the maximum expanse of an output in logical space (i.e. in the
 * Wayland compositor plane) respective to some fix width and height values. The
 * function sets the provided values to these maxima on return.
 */
static inline void
output_get_new_size(struct xwl_output *xwl_output, int *width, int *height)
{
    int logical_width, logical_height, max_width, max_height;

    output_get_logical_extents(xwl_output, &logical_width, &logical_height);

    if (xwl_output->rotation & (RR_Rotate_0 | RR_Rotate_180)) {
        max_width = max(logical_width, xwl_output->mode_width);
        max_height = max(logical_height, xwl_output->mode_height);
    } else {
        max_width = max(logical_width, xwl_output->mode_height);
        max_height = max(logical_height, xwl_output->mode_width);
    }

    if (*width < xwl_output->logical_x + max_width)
        *width = xwl_output->logical_x + max_width;

    if (*height < xwl_output->logical_y + max_height)
        *height = xwl_output->logical_y + max_height;
}

static int
xwl_set_pixmap_visit_window(WindowPtr window, void *data)
{
    ScreenPtr screen = window->drawable.pScreen;

    if (screen->GetWindowPixmap(window) == data) {
        screen->SetWindowPixmap(window, screen->GetScreenPixmap(screen));
        return WT_WALKCHILDREN;
    }

    return WT_DONTWALKCHILDREN;
}

static void
update_backing_pixmaps(struct xwl_screen *xwl_screen, int width, int height)
{
    ScreenPtr pScreen = xwl_screen->screen;
    WindowPtr pRoot = pScreen->root;
    PixmapPtr old_pixmap, new_pixmap;

    old_pixmap = pScreen->GetScreenPixmap(pScreen);
    new_pixmap = pScreen->CreatePixmap(pScreen, width, height,
                                       pScreen->rootDepth,
                                       CREATE_PIXMAP_USAGE_BACKING_PIXMAP);
    pScreen->SetScreenPixmap(new_pixmap);

    if (old_pixmap) {
        TraverseTree(pRoot, xwl_set_pixmap_visit_window, old_pixmap);
        pScreen->DestroyPixmap(old_pixmap);
    }

    pScreen->ResizeWindow(pRoot, 0, 0, width, height, NULL);
}

static void
update_screen_size(struct xwl_screen *xwl_screen, int width, int height)
{
    if (xwl_screen_get_width(xwl_screen) != width)
        xwl_screen->width = width;

    if (xwl_screen_get_height(xwl_screen) != height)
        xwl_screen->height = height;

    if (xwl_screen->root_clip_mode == ROOT_CLIP_FULL)
        SetRootClip(xwl_screen->screen, ROOT_CLIP_NONE);

    if (!xwl_screen->rootless && xwl_screen->screen->root)
        update_backing_pixmaps (xwl_screen, width, height);

    xwl_screen->screen->width = width;
    xwl_screen->screen->height = height;
    xwl_screen->screen->mmWidth = (width * 25.4) / monitorResolution;
    xwl_screen->screen->mmHeight = (height * 25.4) / monitorResolution;

    SetRootClip(xwl_screen->screen, xwl_screen->root_clip_mode);

    if (xwl_screen->screen->root) {
        BoxRec box = { 0, 0, width, height };

        xwl_screen->screen->root->drawable.width = width;
        xwl_screen->screen->root->drawable.height = height;
        RegionReset(&xwl_screen->screen->root->winSize, &box);
        RRScreenSizeNotify(xwl_screen->screen);
    }

    update_desktop_dimensions();

    RRTellChanged(xwl_screen->screen);
}

struct xwl_emulated_mode *
xwl_output_get_emulated_mode_for_client(struct xwl_output *xwl_output,
                                        ClientPtr client)
{
    struct xwl_client *xwl_client = xwl_client_get(client);
    int i;

    if (!xwl_output)
        return NULL;

    /* We don't do XRandr emulation when rootful or a fake lease display */
    if (!xwl_output->xwl_screen->rootless || !xwl_output->output)
        return NULL;

    for (i = 0; i < XWL_CLIENT_MAX_EMULATED_MODES; i++) {
        if (xwl_client->emulated_modes[i].server_output_id ==
            xwl_output->server_output_id)
            return &xwl_client->emulated_modes[i];
    }

    return NULL;
}

static void
xwl_output_add_emulated_mode_for_client(struct xwl_output *xwl_output,
                                        ClientPtr client,
                                        RRModePtr mode,
                                        Bool from_vidmode)
{
    struct xwl_client *xwl_client = xwl_client_get(client);
    struct xwl_emulated_mode *emulated_mode;
    int i;

    emulated_mode = xwl_output_get_emulated_mode_for_client(xwl_output, client);
    if (!emulated_mode) {
        /* Find a free spot in the emulated modes array */
        for (i = 0; i < XWL_CLIENT_MAX_EMULATED_MODES; i++) {
            if (xwl_client->emulated_modes[i].server_output_id == 0) {
                emulated_mode = &xwl_client->emulated_modes[i];
                break;
            }
        }
    }
    if (!emulated_mode) {
        static Bool warned;

        if (!warned) {
            ErrorF("Ran out of space for emulated-modes, not adding mode");
            warned = TRUE;
        }

        return;
    }

    emulated_mode->server_output_id = xwl_output->server_output_id;
    emulated_mode->width  = mode->mode.width;
    emulated_mode->height = mode->mode.height;
    emulated_mode->id = mode->mode.id;
    emulated_mode->from_vidmode = from_vidmode;
}

static void
xwl_output_remove_emulated_mode_for_client(struct xwl_output *xwl_output,
                                           ClientPtr client)
{
    struct xwl_emulated_mode *emulated_mode;

    emulated_mode = xwl_output_get_emulated_mode_for_client(xwl_output, client);
    if (emulated_mode) {
        DebugF("XWAYLAND: xwl_output_remove_emulated_mode: %dx%d\n",
               emulated_mode->width, emulated_mode->height);
        memset(emulated_mode, 0, sizeof(*emulated_mode));
    }
}

/* From hw/xfree86/common/xf86DefModeSet.c with some obscure modes dropped */
const int32_t xwl_output_fake_modes[][2] = {
    { 5120, 2880 }, /* 16:9 (1.77) */
    { 4096, 2304 }, /* 16:9 (1.77) */
    { 3840, 2160 }, /* 16:9 (1.77) */
    { 3200, 1800 }, /* 16:9 (1.77) */
    { 2880, 1620 }, /* 16:9 (1.77) */
    { 2560, 1600 }, /* 16:10 (1.6) */
    { 2560, 1440 }, /* 16:9 (1.77) */
    { 2048, 1536 }, /* 4:3 (1.33) */
    { 2048, 1152 }, /* 16:9 (1.77) */
    { 1920, 1440 }, /* 4:3 (1.33) */
    { 1920, 1200 }, /* 16:10 (1.6) */
    { 1920, 1080 }, /* 16:9 (1.77) */
    { 1680, 1050 }, /* 16:10 (1.6) */
    { 1600, 1200 }, /* 4:3 (1.33) */
    { 1600,  900 }, /* 16:9 (1.77) */
    { 1440, 1080 }, /* 4:3 (1.33) */
    { 1440,  900 }, /* 16:10 (1.6) */
    { 1400, 1050 }, /* 4:3 (1.33) */
    { 1368,  768 }, /* 16:9 (1.77) */
    { 1280, 1024 }, /* 5:4 (1.25) */
    { 1280,  960 }, /* 4:3 (1.33) */
    { 1280,  800 }, /* 16:10 (1.6) */
    { 1280,  720 }, /* 16:9 (1.77) */
    { 1152,  864 }, /* 4:3 (1.33) */
    { 1152,  720 }, /* 16:10 (1.6) */
    { 1024,  768 }, /* 4:3 (1.33) */
    { 1024,  576 }, /* 16:9 (1.77) */
    {  960,  600 }, /* 16:10 (1.6) */
    {  928,  580 }, /* 16:10 (1.6) */
    {  864,  486 }, /* 16:9 (1.77) */
    {  800,  600 }, /* 4:3 (1.33) */
    {  800,  500 }, /* 16:10 (1.6) */
    {  768,  480 }, /* 16:10 (1.6) */
    {  720,  480 }, /* 3:2 (1.5) */
    {  720,  400 }, /* 16:9 (1.77) */
    {  640,  480 }, /* 4:3 (1.33) */
    {  640,  400 }, /* 16:10 (1.6) */
    {  640,  350 }, /* 16:9 (1.77) */
    {  320,  240 }, /* 4:3 (1.33) */
    {  320,  200 }, /* 16:10 (1.6) */
};

/* Build an array with RRModes the first mode is the actual output mode, the
 * rest are fake modes from the xwl_output_fake_modes list. We do this for apps
 * which want to change resolution when they go fullscreen.
 * When an app requests a mode-change, we fake it using WPviewport.
 */
static RRModePtr *
output_get_rr_modes(struct xwl_output *xwl_output,
                    int32_t width, int32_t height,
                    int *count, int *logical_mode)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;
    RRModePtr *rr_modes;
    int i = 0;

    rr_modes = xallocarray(ARRAY_SIZE(xwl_output_fake_modes) + 2, sizeof(RRModePtr));
    if (!rr_modes)
        goto err;

    *count = 0;

    if (xwl_screen_has_resolution_change_emulation(xwl_screen) &&
        (width != xwl_output->mode_width || height != xwl_output->mode_height)) {
        /* Add native output mode as preferred */
        rr_modes[0] = xwayland_cvt(xwl_output->mode_width, xwl_output->mode_height,
                                   xwl_output->refresh / 1000.0, 0, 0);
        if (!rr_modes[0])
            goto err;

        *count = 1;

        /* Add fake modes larger than logical mode */
        for (; i < ARRAY_SIZE(xwl_output_fake_modes); i++) {
            if (xwl_output_fake_modes[i][0] <= width &&
                xwl_output_fake_modes[i][1] <= height)
                break;

            /* Skip modes which are too big, avoid downscaling */
            if (xwl_output_fake_modes[i][0] >= xwl_output->mode_width &&
                xwl_output_fake_modes[i][1] >= xwl_output->mode_height)
                continue;

            rr_modes[*count] = xwayland_cvt(xwl_output_fake_modes[i][0],
                                            xwl_output_fake_modes[i][1],
                                            xwl_output->refresh / 1000.0, 0, 0);
            if (!rr_modes[*count])
                goto err;

            (*count)++;
        }
    }

    /* Add logical output mode */
    rr_modes[*count] = xwayland_cvt(width, height, xwl_output->refresh / 1000.0, 0, 0);
    if (!rr_modes[*count])
        goto err;

    *logical_mode = (*count)++;

    if (!xwl_screen_has_resolution_change_emulation(xwl_screen) && !xwl_screen->force_xrandr_emulation)
        return rr_modes;

    /* Add fake modes */
    for (; i < ARRAY_SIZE(xwl_output_fake_modes); i++) {
        /* Skip logical output mode, already added */
        if (xwl_output_fake_modes[i][0] == width &&
            xwl_output_fake_modes[i][1] == height)
            continue;

        /* Skip native output mode, already added */
        if (xwl_output_fake_modes[i][0] == xwl_output->mode_width &&
            xwl_output_fake_modes[i][1] == xwl_output->mode_height)
            continue;

        /* Skip modes which are too big, avoid downscaling */
        if (xwl_output_fake_modes[i][0] > max(width, xwl_output->mode_width) ||
            xwl_output_fake_modes[i][1] > max(height, xwl_output->mode_height))
            continue;

        rr_modes[*count] = xwayland_cvt(xwl_output_fake_modes[i][0],
                                        xwl_output_fake_modes[i][1],
                                        xwl_output->refresh / 1000.0, 0, 0);
        if (!rr_modes[*count])
            goto err;

        (*count)++;
    }

    return rr_modes;
err:
    FatalError("Failed to allocate memory for list of RR modes");
}

RRModePtr
xwl_output_find_mode(struct xwl_output *xwl_output,
                     int32_t width, int32_t height)
{
    RROutputPtr output = xwl_output->randr_output;
    int i;

    /* width & height -1 means we want the actual output mode */
    if (width == -1 && height == -1) {
        if (xwl_output == xwl_output->xwl_screen->fixed_output &&
            xwl_output->mode_width > 0 && xwl_output->mode_height > 0) {
            /* If running rootful, use the current fixed size to search for the mode */
            width = xwl_output->mode_width;
            height = xwl_output->mode_height;
        }
        else {
            output_get_logical_mode(xwl_output, &width, &height);

            if (output->numModes &&
                (width <= 0 || height <= 0))
                return output->modes[0];
        }
    }

    for (i = 0; i < output->numModes; i++) {
        if (output->modes[i]->mode.width == width && output->modes[i]->mode.height == height)
            return output->modes[i];
    }

    ErrorF("XWAYLAND: mode %dx%d is not available\n", width, height);
    return NULL;
}

struct xwl_output_randr_emu_prop {
    Atom atom;
    uint32_t rects[XWL_CLIENT_MAX_EMULATED_MODES][4];
    int rect_count;
};

static void
xwl_output_randr_emu_prop(struct xwl_screen *xwl_screen, ClientPtr client,
                          struct xwl_output_randr_emu_prop *prop)
{
    static const char atom_name[] = "_XWAYLAND_RANDR_EMU_MONITOR_RECTS";
    struct xwl_emulated_mode *emulated_mode;
    struct xwl_output *xwl_output;
    int index = 0;

    prop->atom = MakeAtom(atom_name, strlen(atom_name), TRUE);

    xorg_list_for_each_entry(xwl_output, &xwl_screen->output_list, link) {
        emulated_mode = xwl_output_get_emulated_mode_for_client(xwl_output, client);
        if (!emulated_mode)
            continue;

        prop->rects[index][0] = xwl_output->logical_x;
        prop->rects[index][1] = xwl_output->logical_y;

        if (xwl_output->rotation & (RR_Rotate_0 | RR_Rotate_180)) {
            prop->rects[index][2] = emulated_mode->width;
            prop->rects[index][3] = emulated_mode->height;
        } else {
            prop->rects[index][2] = emulated_mode->height;
            prop->rects[index][3] = emulated_mode->width;
        }

        index++;
    }

    prop->rect_count = index;
}

static void
xwl_output_set_randr_emu_prop(WindowPtr window,
                              struct xwl_output_randr_emu_prop *prop)
{
    if (prop->rect_count) {
        dixChangeWindowProperty(serverClient, window, prop->atom,
                                XA_CARDINAL, 32, PropModeReplace,
                                prop->rect_count * 4, prop->rects, TRUE);
    } else {
        DeleteProperty(serverClient, window, prop->atom);
    }
}

static void
xwl_output_set_randr_emu_prop_callback(void *resource, XID id, void *user_data)
{
    if (xwl_window_is_toplevel(resource))
        xwl_output_set_randr_emu_prop(resource, user_data);
}

static void
xwl_output_set_randr_emu_props(struct xwl_screen *xwl_screen, ClientPtr client)
{
    struct xwl_output_randr_emu_prop prop = {};

    xwl_output_randr_emu_prop(xwl_screen, client, &prop);
    FindClientResourcesByType(client, X11_RESTYPE_WINDOW,
                              xwl_output_set_randr_emu_prop_callback, &prop);
}

static inline void
xwl_output_get_emulated_root_size(struct xwl_output *xwl_output,
                                  ClientPtr client,
                                  int *width,
                                  int *height)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;
    struct xwl_emulated_mode *emulated_mode;

    emulated_mode = xwl_output_get_emulated_mode_for_client(xwl_output, client);
    /* If not an emulated mode, just return the actual screen size */
    if (!emulated_mode) {
        *width = xwl_screen_get_width(xwl_screen);
        *height = xwl_screen_get_height(xwl_screen);
        return;
    }

    if (xwl_output->rotation & (RR_Rotate_0 | RR_Rotate_180)) {
        *width = emulated_mode->width;
        *height = emulated_mode->height;
    } else {
        *width = emulated_mode->height;
        *height = emulated_mode->width;
    }
}

static int
xwl_output_get_rr_event_mask(WindowPtr pWin, ClientPtr client)
{
    RREventPtr pRREvent, *pHead;

    dixLookupResourceByType((void **) &pHead, pWin->drawable.id,
                            RREventType, client, DixReadAccess);

    pRREvent = NULL;
    if (pHead) {
        for (pRREvent = *pHead; pRREvent; pRREvent = pRREvent->next)
            if (pRREvent->client == client)
                break;
        }

    if (pRREvent)
        return pRREvent->mask;

    return 0;
}

static void
xwl_output_notify_emulated_root_size(struct xwl_output *xwl_output,
                                     ClientPtr client,
                                     int new_emulated_root_width,
                                     int new_emulated_root_height)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;
    ScreenPtr pScreen = xwl_screen->screen;
    WindowPtr pRoot = pScreen->root;
    xEvent event = {
             .u.configureNotify.event = pRoot->drawable.id,
             .u.configureNotify.window = pRoot->drawable.id,
             .u.configureNotify.aboveSibling = None,
             .u.configureNotify.x = 0,
             .u.configureNotify.y = 0,
             .u.configureNotify.width = new_emulated_root_width,
             .u.configureNotify.height = new_emulated_root_height,
             .u.configureNotify.borderWidth = pRoot->borderWidth,
             .u.configureNotify.override = pRoot->overrideRedirect
         };
     event.u.u.type = ConfigureNotify;

     if (!client || client == serverClient || client->clientGone)
         return;

     if (EventMaskForClient(pRoot, client) & StructureNotifyMask)
         WriteEventsToClient(client, 1, &event);

     if (xwl_output_get_rr_event_mask(pRoot, client) & RRScreenChangeNotifyMask)
         RRDeliverScreenEvent(client, pRoot, pScreen);
}

void
xwl_output_set_window_randr_emu_props(struct xwl_screen *xwl_screen,
                                      WindowPtr window)
{
    struct xwl_output_randr_emu_prop prop = {};

    xwl_output_randr_emu_prop(xwl_screen, wClient(window), &prop);
    xwl_output_set_randr_emu_prop(window, &prop);
}

void
xwl_output_set_emulated_mode(struct xwl_output *xwl_output, ClientPtr client,
                             RRModePtr mode, Bool from_vidmode)
{
    int old_emulated_width, old_emulated_height;
    int logical_width, logical_height;
    int new_emulated_width, new_emulated_height;

    DebugF("XWAYLAND: xwl_output_set_emulated_mode from %s: %dx%d\n",
           from_vidmode ? "vidmode" : "randr",
           mode->mode.width, mode->mode.height);

    xwl_output_get_emulated_root_size(xwl_output, client,
                                      &old_emulated_width, &old_emulated_height);

    /* Skip the logical (not-emulated) output mode */
    output_get_logical_mode(xwl_output, &logical_width, &logical_height);
    if (mode->mode.width == logical_width && mode->mode.height == logical_height)
        xwl_output_remove_emulated_mode_for_client(xwl_output, client);
    else
        xwl_output_add_emulated_mode_for_client(xwl_output, client, mode, from_vidmode);

    xwl_screen_check_resolution_change_emulation(xwl_output->xwl_screen);

    xwl_output_set_randr_emu_props(xwl_output->xwl_screen, client);

    xwl_output_get_emulated_root_size(xwl_output, client,
                                      &new_emulated_width, &new_emulated_height);

    if (new_emulated_width != old_emulated_width ||
        new_emulated_height != old_emulated_height)
        xwl_output_notify_emulated_root_size(xwl_output, client,
                                             new_emulated_width,
                                             new_emulated_height);
}

static void
maybe_update_fullscreen_state(struct xwl_output *xwl_output)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;
    struct xwl_window *xwl_window;

    if (xwl_screen->fullscreen) {
        /* The root window may not yet be created */
        if (xwl_screen->screen->root) {
            xwl_window = xwl_window_get(xwl_screen->screen->root);
            xwl_window_rootful_update_fullscreen(xwl_window, xwl_output);
        }
    }
}

static void
apply_output_change(struct xwl_output *xwl_output)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;
    struct xwl_output *it;
    int logical_width, logical_height, count, has_this_output = 0;
    RRModePtr *randr_modes;
    int logical_mode;

    /* Clear out the "done" received flags */
    xwl_output->wl_output_done = FALSE;
    xwl_output->xdg_output_done = FALSE;

    output_get_logical_mode(xwl_output, &logical_width, &logical_height);

    if (xwl_output->randr_output) {
        /* Build a fresh modes array using the current refresh rate */
        randr_modes = output_get_rr_modes(xwl_output, logical_width, logical_height,
                                          &count, &logical_mode);

        RROutputSetModes(xwl_output->randr_output, randr_modes, count, 1);
        RRCrtcNotify(xwl_output->randr_crtc, randr_modes[logical_mode],
                     xwl_output->logical_x, xwl_output->logical_y,
                     xwl_output->rotation, NULL, 1, &xwl_output->randr_output);
        /* RROutputSetModes takes ownership of the passed in modes, so we only
         * have to free the pointer array.
         */
        free(randr_modes);
    }

    logical_width = logical_height = 0;
    xorg_list_for_each_entry(it, &xwl_screen->output_list, link) {
        /* output done event is sent even when some property
         * of output is changed. That means that we may already
         * have this output. If it is true, we must not add it
         * into the output_list otherwise we'll corrupt it */
        if (it == xwl_output)
            has_this_output = 1;

        output_get_new_size(it, &logical_width, &logical_height);
    }

    if (!has_this_output) {
        xorg_list_append(&xwl_output->link, &xwl_screen->output_list);

        /* we did not check this output for new screen size, do it now */
        output_get_new_size(xwl_output, &logical_width, &logical_height);

        --xwl_screen->expecting_event;
    }

    if (xwl_screen->fixed_output == NULL)
        update_screen_size(xwl_screen, logical_width, logical_height);
    else
        RRTellChanged(xwl_screen->screen);

    /* If running rootful and fullscreen, make sure to match the new setup */
    maybe_update_fullscreen_state(xwl_output);
}

void
xwl_output_set_name(struct xwl_output *xwl_output, const char *name)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;
    rrScrPrivPtr pScrPriv;
    RRLeasePtr lease;
    int i;

    if (xwl_output->randr_output == NULL)
        return; /* rootful */

    /* Check whether the compositor is sending us something useful */
    if (!name || !strlen(name)) {
        ErrorF("Not using the provided output name, invalid");
        return;
    }

    /* Check for duplicate names to be safe */
    pScrPriv = rrGetScrPriv(xwl_screen->screen);
    for (i = 0; i < pScrPriv->numOutputs; i++) {
        if (!strcmp(name, pScrPriv->outputs[i]->name)) {
            ErrorF("An output named '%s' already exists", name);
            return;
        }
    }
    /* And leases' names as well */
    xorg_list_for_each_entry(lease, &pScrPriv->leases, list) {
        for (i = 0; i < lease->numOutputs; i++) {
            if (!strcmp(name, lease->outputs[i]->name)) {
                ErrorF("A lease output named '%s' already exists", name);
                return;
            }
        }
    }

    snprintf(xwl_output->randr_output->name, MAX_OUTPUT_NAME, "%s", name);
    xwl_output->randr_output->nameLength = strlen(xwl_output->randr_output->name);

    if (xwl_screen->output_name && strcmp(name, xwl_screen->output_name) == 0)
        maybe_update_fullscreen_state(xwl_output);
}

static void
output_handle_done(void *data, struct wl_output *wl_output)
{
    struct xwl_output *xwl_output = data;
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;

    xwl_output->wl_output_done = TRUE;
    if (xwl_screen->fixed_output)
        return;

    /* Apply the changes from wl_output only if both "done" events are received,
     * if xdg-output is not supported or if xdg-output version is high enough.
     */
    if (xwl_output->xdg_output_done || !xwl_output->xdg_output ||
        zxdg_output_v1_get_version(xwl_output->xdg_output) >= 3)
        apply_output_change(xwl_output);
}

static void
output_handle_scale(void *data, struct wl_output *wl_output, int32_t factor)
{
    struct xwl_output *xwl_output = data;

    xwl_output->scale = factor;
}

static void
output_handle_name(void *data, struct wl_output *wl_output,
                   const char *name)
{
    struct xwl_output *xwl_output = data;

    xwl_output_set_name(xwl_output, name);
}

static void
output_handle_description(void *data, struct wl_output *wl_output,
                          const char *description)
{
}

static const struct wl_output_listener output_listener = {
    output_handle_geometry,
    output_handle_mode,
    output_handle_done,
    output_handle_scale,
    output_handle_name,
    output_handle_description,
};

static void
xdg_output_handle_logical_position(void *data, struct zxdg_output_v1 *xdg_output,
                                   int32_t x, int32_t y)
{
    struct xwl_output *xwl_output = data;

    xwl_output->logical_x = x;
    xwl_output->logical_y = y;
}

static void
xdg_output_handle_logical_size(void *data, struct zxdg_output_v1 *xdg_output,
                               int32_t width, int32_t height)
{
    struct xwl_output *xwl_output = data;

    xwl_output->logical_w = width;
    xwl_output->logical_h = height;
}

static void
xdg_output_handle_done(void *data, struct zxdg_output_v1 *xdg_output)
{
    struct xwl_output *xwl_output = data;
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;

    xwl_output->xdg_output_done = TRUE;

    if (xwl_screen->fixed_output)
        return;

    if (xwl_output->wl_output_done &&
        zxdg_output_v1_get_version(xdg_output) < 3)
        apply_output_change(xwl_output);
}

static void
xdg_output_handle_name(void *data, struct zxdg_output_v1 *xdg_output,
                       const char *name)
{
    struct xwl_output *xwl_output = data;

    if (wl_output_get_version(xwl_output->output) >= 4)
        return; /* wl_output.name is preferred */

    xwl_output_set_name(xwl_output, name);
}

static void
xdg_output_handle_description(void *data, struct zxdg_output_v1 *xdg_output,
                              const char *description)
{
}

static const struct zxdg_output_v1_listener xdg_output_listener = {
    xdg_output_handle_logical_position,
    xdg_output_handle_logical_size,
    xdg_output_handle_done,
    xdg_output_handle_name,
    xdg_output_handle_description,
};

#define XRANDR_EMULATION_PROP "RANDR Emulation"
static Atom
get_rand_emulation_property(void)
{
    const char *emulStr = XRANDR_EMULATION_PROP;

    return MakeAtom(emulStr, strlen(emulStr), TRUE);
}

static void
xwl_output_set_emulated(struct xwl_output *xwl_output)
{
    int32_t val = TRUE;

    RRChangeOutputProperty(xwl_output->randr_output,
                           get_rand_emulation_property(),
                           XA_INTEGER,
                           32, PropModeReplace, 1,
                           &val, FALSE, FALSE);
}

struct xwl_output*
xwl_output_from_wl_output(struct xwl_screen *xwl_screen,
                          struct wl_output* wl_output)
{
    struct xwl_output *xwl_output;

    xorg_list_for_each_entry(xwl_output, &xwl_screen->output_list, link) {
        if (xwl_output->output == wl_output)
            return xwl_output;
    }

    return NULL;
}

struct xwl_output *
xwl_output_get_output_from_name(struct xwl_screen *xwl_screen, const char *name)
{
    struct xwl_output *xwl_output;

    if (name == NULL)
        return NULL;

    xorg_list_for_each_entry(xwl_output, &xwl_screen->output_list, link) {
        if (xwl_output->randr_output == NULL)
            continue;

        if (strcmp(xwl_output->randr_output->name, name) == 0) {
            return xwl_output;
        }
    }

    return NULL;
}

struct xwl_output *
xwl_output_create(struct xwl_screen *xwl_screen, uint32_t id,
                  Bool connected, uint32_t version)
{
    struct xwl_output *xwl_output;
    char name[MAX_OUTPUT_NAME] = { 0 };

    --xwl_screen->expecting_event;

    xwl_output = calloc(1, sizeof *xwl_output);
    if (xwl_output == NULL) {
        ErrorF("%s ENOMEM\n", __func__);
        return NULL;
    }

    xwl_output->output = wl_registry_bind(xwl_screen->registry, id,
                                          &wl_output_interface, min(version, 4));
    if (!xwl_output->output) {
        ErrorF("Failed binding wl_output\n");
        goto err;
    }

    xwl_output->server_output_id = id;
    wl_output_add_listener(xwl_output->output, &output_listener, xwl_output);
    xwl_output->xscale = 1.0;

    xwl_output->xwl_screen = xwl_screen;

    xwl_output->randr_crtc = RRCrtcCreate(xwl_screen->screen, xwl_output);
    if (!xwl_output->randr_crtc) {
        ErrorF("Failed creating RandR CRTC\n");
        goto err;
    }
    RRCrtcSetRotations (xwl_output->randr_crtc, ALL_ROTATIONS);

    /* Allocate MAX_OUTPUT_NAME data for the output name, all filled with zeros */
    xwl_output->randr_output = RROutputCreate(xwl_screen->screen, name,
                                              MAX_OUTPUT_NAME, xwl_output);
    if (!xwl_output->randr_output) {
        ErrorF("Failed creating RandR Output\n");
        goto err;
    }
    /* Set the default output name to a sensible value */
    snprintf(name, MAX_OUTPUT_NAME, "XWAYLAND%d",
             xwl_screen_get_next_output_serial(xwl_screen));
    xwl_output_set_name(xwl_output, name);
    xwl_output_set_emulated(xwl_output);

    RRCrtcGammaSetSize(xwl_output->randr_crtc, 256);
    RROutputSetCrtcs(xwl_output->randr_output, &xwl_output->randr_crtc, 1);
    RROutputSetConnection(xwl_output->randr_output,
                          connected ? RR_Connected : RR_Disconnected);

    /* We want the output to be in the list as soon as created so we can
     * use it when binding to the xdg-output protocol...
     */
    xorg_list_append(&xwl_output->link, &xwl_screen->output_list);

    if (xwl_screen->xdg_output_manager)
        xwl_output_get_xdg_output(xwl_output);

    return xwl_output;

err:
    if (xwl_output->randr_crtc)
        RRCrtcDestroy(xwl_output->randr_crtc);
    if (xwl_output->output)
        wl_output_destroy(xwl_output->output);
    free(xwl_output);
    return NULL;
}

void
xwl_output_destroy(struct xwl_output *xwl_output)
{
    if (xwl_output->lease_connector)
        wp_drm_lease_connector_v1_destroy(xwl_output->lease_connector);
    if (xwl_output->transform)
        free(xwl_output->transform);
    if (xwl_output->xdg_output)
        zxdg_output_v1_destroy(xwl_output->xdg_output);
    if (xwl_output->output)
        wl_output_destroy(xwl_output->output);
    free(xwl_output);
}

void
xwl_output_remove(struct xwl_output *xwl_output)
{
    struct xwl_output *it;
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;
    struct xwl_window *xwl_window;
    int width = 0, height = 0;

    /* Not all compositors send a "leave" event on output removal */
    xorg_list_for_each_entry(xwl_window, &xwl_screen->window_list, link_window)
        xwl_window_leave_output(xwl_window, xwl_output);

    xorg_list_del(&xwl_output->link);

    if (xwl_output->randr_output)
        RROutputSetConnection(xwl_output->randr_output, RR_Disconnected);

    if (xwl_screen->fixed_output == NULL) {
        xorg_list_for_each_entry(it, &xwl_screen->output_list, link)
            output_get_new_size(it, &width, &height);
        update_screen_size(xwl_screen, width, height);
    }

    if (xwl_output->randr_crtc)
        RRCrtcDestroy(xwl_output->randr_crtc);
    if (xwl_output->randr_output) {
        RROutputDestroy(xwl_output->randr_output);
        RRTellChanged(xwl_screen->screen);
    }
    xwl_output_destroy(xwl_output);
}

static Bool
xwl_randr_get_info(ScreenPtr pScreen, Rotation * rotations)
{
    *rotations = ALL_ROTATIONS;

    return TRUE;
}

#ifdef RANDR_10_INTERFACE
static Bool
xwl_randr_set_config(ScreenPtr pScreen,
                     Rotation rotation, int rate, RRScreenSizePtr pSize)
{
    return FALSE;
}
#endif

#if RANDR_12_INTERFACE
static Bool
xwl_randr_screen_set_size(ScreenPtr pScreen,
                          CARD16 width,
                          CARD16 height,
                          CARD32 mmWidth, CARD32 mmHeight)
{
    return TRUE;
}

static Bool
xwl_randr_crtc_set(ScreenPtr pScreen,
                   RRCrtcPtr crtc,
                   RRModePtr new_mode,
                   int x,
                   int y,
                   Rotation rotation,
                   int numOutputs, RROutputPtr * outputs)
{
    struct xwl_output *xwl_output = crtc->devPrivate;
    RRModePtr mode;

    if (new_mode) {
        mode = xwl_output_find_mode(xwl_output,
                                    new_mode->mode.width,
                                    new_mode->mode.height);
    } else {
        mode = xwl_output_find_mode(xwl_output, -1, -1);
    }
    if (!mode)
        return FALSE;

    xwl_output_set_emulated_mode(xwl_output, GetCurrentClient(), mode, FALSE);

    /* A real randr implementation would call:
     * RRCrtcNotify(xwl_output->randr_crtc, mode, xwl_output->x, xwl_output->y,
     *              xwl_output->rotation, NULL, 1, &xwl_output->randr_output);
     * here to update the mode reported to clients querying the randr settings
     * but that influences *all* clients and we do randr mode change emulation
     * on a per client basis. So we just return success here.
     */

    return TRUE;
}

static void
xwl_randr_crtc_get(ScreenPtr pScreen,
                   RRCrtcPtr crtc,
                   xRRGetCrtcInfoReply *rep)
{
    struct xwl_output *xwl_output = crtc->devPrivate;

    struct xwl_emulated_mode *mode = xwl_output_get_emulated_mode_for_client(
        xwl_output, GetCurrentClient());

    if (mode)
        rep->mode = mode->id;
}

static Bool
xwl_randr_crtc_set_gamma(ScreenPtr pScreen, RRCrtcPtr crtc)
{
    return TRUE;
}

static Bool
xwl_randr_crtc_get_gamma(ScreenPtr pScreen, RRCrtcPtr crtc)
{
    return TRUE;
}

static Bool
xwl_randr_output_set_property(ScreenPtr pScreen,
                              RROutputPtr output,
                              Atom property,
                              RRPropertyValuePtr value)
{
    /* RANDR Emulation property is read-only. */
    if (get_rand_emulation_property() == property)
        return FALSE;

    return TRUE;
}

static Bool
xwl_output_validate_mode(ScreenPtr pScreen,
                         RROutputPtr output,
                         RRModePtr mode)
{
    return TRUE;
}

static void
xwl_randr_mode_destroy(ScreenPtr pScreen, RRModePtr mode)
{
    return;
}
#endif

Bool
xwl_screen_init_output(struct xwl_screen *xwl_screen)
{
    rrScrPrivPtr rp;

    if (!RRScreenInit(xwl_screen->screen))
        return FALSE;

    xwl_screen->screen->ConstrainCursorHarder = NULL;

    RRScreenSetSizeRange(xwl_screen->screen, 16, 16, 32767, 32767);

    rp = rrGetScrPriv(xwl_screen->screen);
    rp->rrGetInfo = xwl_randr_get_info;

#if RANDR_10_INTERFACE
    rp->rrSetConfig = xwl_randr_set_config;
#endif

#if RANDR_12_INTERFACE
    rp->rrScreenSetSize = xwl_randr_screen_set_size;
    rp->rrCrtcSet = xwl_randr_crtc_set;
    rp->rrCrtcGet = xwl_randr_crtc_get;
    rp->rrCrtcSetGamma = xwl_randr_crtc_set_gamma;
    rp->rrCrtcGetGamma = xwl_randr_crtc_get_gamma;
    rp->rrOutputSetProperty = xwl_randr_output_set_property;
    rp->rrOutputValidateMode = xwl_output_validate_mode;
    rp->rrModeDestroy = xwl_randr_mode_destroy;
#endif

    rp->rrRequestLease = xwl_randr_request_lease;
    rp->rrGetLease = xwl_randr_get_lease;
    rp->rrTerminateLease = xwl_randr_terminate_lease;

    return TRUE;
}

static int
mode_sort(const void *left, const void *right)
{
    const RRModePtr *mode_a = left;
    const RRModePtr *mode_b = right;

    if ((*mode_b)->mode.width == (*mode_a)->mode.width)
        return (*mode_b)->mode.height - (*mode_a)->mode.height;

    return (*mode_b)->mode.width - (*mode_a)->mode.width;
}

static void
xwl_output_set_transform(struct xwl_output *xwl_output)
{
    pixman_fixed_t transform_xscale;
    RRModePtr mode;

    mode = xwl_output_find_mode(xwl_output, xwl_output->mode_width, xwl_output->mode_height);
    if (!mode) {
        ErrorF("XWAYLAND: Failed to find mode for %ix%i\n",
               xwl_output->mode_width, xwl_output->mode_height);
        return;
    }

    if (xwl_output->transform == NULL) {
        xwl_output->transform = XNFalloc(sizeof(RRTransformRec));
        RRTransformInit(xwl_output->transform);
    }

    transform_xscale = pixman_double_to_fixed(xwl_output->xscale);
    pixman_transform_init_scale(&xwl_output->transform->transform,
                                transform_xscale, transform_xscale);
    pixman_f_transform_init_scale(&xwl_output->transform->f_transform,
                                  xwl_output->xscale, xwl_output->xscale);
    pixman_f_transform_invert(&xwl_output->transform->f_inverse,
                              &xwl_output->transform->f_transform);

    RRCrtcNotify(xwl_output->randr_crtc, mode, 0, 0, RR_Rotate_0,
                 xwl_output->transform, 1, &xwl_output->randr_output);
}

void
xwl_output_set_xscale(struct xwl_output *xwl_output, double xscale)
{
    xwl_output->xscale = xscale;
    xwl_output_set_transform(xwl_output);
}

Bool
xwl_randr_add_modes_fixed(struct xwl_output *xwl_output,
                          int current_width, int current_height)
{
    RRModePtr *modes = NULL;
    RRModePtr mode;
    int i, nmodes, current;

    modes = xallocarray(ARRAY_SIZE(xwl_output_fake_modes) + 1, sizeof(RRModePtr));
    if (!modes) {
        ErrorF("Failed to allocated RandR modes\n");
        return FALSE;
    }

    xwl_output->mode_width = current_width;
    xwl_output->mode_height = current_height;

    nmodes = 0;
    current = 0;

    /* Add fake modes */
    for (i = 0; i < ARRAY_SIZE(xwl_output_fake_modes); i++) {
        if (xwl_output_fake_modes[i][0] == current_width &&
            xwl_output_fake_modes[i][1] == current_height)
            current = 1;

        mode = xwayland_cvt(xwl_output_fake_modes[i][0],
                            xwl_output_fake_modes[i][1],
                            60, 0, 0);

        if (mode)
            modes[nmodes++] = mode;
    }

    if (!current) {
        /* Add the current mode as it's not part of the fake modes. */
        mode = xwayland_cvt(current_width, current_height, 60, 0, 0);

        if (mode)
            modes[nmodes++] = mode;
    }

    qsort(modes, nmodes, sizeof(RRModePtr), mode_sort);
    RROutputSetModes(xwl_output->randr_output, modes, nmodes, 1);
    free(modes);

    return TRUE;
}

void
xwl_output_set_mode_fixed(struct xwl_output *xwl_output, RRModePtr mode)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;

    xwl_output->mode_width = mode->mode.width;
    xwl_output->mode_height = mode->mode.height;

    update_screen_size(xwl_screen,
                       round((double) mode->mode.width * xwl_output->xscale),
                       round((double) mode->mode.height * xwl_output->xscale));

    xwl_output_set_transform(xwl_output);
}

static Bool
xwl_randr_set_config_fixed(ScreenPtr pScreen,
                           Rotation randr, int rate, RRScreenSizePtr pSize)
{
    struct xwl_screen *xwl_screen = xwl_screen_get(pScreen);

    update_screen_size(xwl_screen, pSize->width, pSize->height);

    return TRUE;
}

/* Create a single RR output/mode used with a fixed geometry */
Bool
xwl_screen_init_randr_fixed(struct xwl_screen *xwl_screen)
{
    struct xwl_output *xwl_output;
    char name[MAX_OUTPUT_NAME] = { 0 };
    rrScrPrivPtr rp;
    RRModePtr mode;

    xwl_output = calloc(1, sizeof *xwl_output);
    if (xwl_output == NULL) {
        ErrorF("%s ENOMEM\n", __func__);
        return FALSE;
    }

    if (!RRScreenInit(xwl_screen->screen))
        goto err;

    RRScreenSetSizeRange(xwl_screen->screen, 16, 16, 32767, 32767);

    rp = rrGetScrPriv(xwl_screen->screen);
    rp->rrGetInfo = xwl_randr_get_info;
    rp->rrSetConfig = xwl_randr_set_config_fixed;

    snprintf(name, MAX_OUTPUT_NAME, "XWAYLAND%d",
             xwl_screen_get_next_output_serial(xwl_screen));
    xwl_output->randr_output = RROutputCreate(xwl_screen->screen, name,
                                              strlen(name), NULL);
    if (!xwl_output->randr_output) {
        ErrorF("Failed to create RandR output\n");
        goto err;
    }

    xwl_output->randr_crtc = RRCrtcCreate(xwl_screen->screen, xwl_output);
    if (!xwl_output->randr_crtc) {
        ErrorF("Failed to create RandR CRTC\n");
        goto err;
    }
    RRCrtcSetRotations (xwl_output->randr_crtc, RR_Rotate_0);
    RRCrtcGammaSetSize(xwl_output->randr_crtc, 256);
    RRCrtcSetTransformSupport(xwl_output->randr_crtc, TRUE);
    RROutputSetCrtcs(xwl_output->randr_output, &xwl_output->randr_crtc, 1);

    xwl_randr_add_modes_fixed(xwl_output,
                              xwl_screen_get_width(xwl_screen),
                              xwl_screen_get_height(xwl_screen));
    /* Current mode */
    mode = xwl_output_find_mode(xwl_output,
                                xwl_screen_get_width(xwl_screen),
                                xwl_screen_get_height(xwl_screen));
    RRCrtcNotify(xwl_output->randr_crtc, mode, 0, 0, RR_Rotate_0,
                 NULL, 1, &xwl_output->randr_output);

    RROutputSetPhysicalSize(xwl_output->randr_output,
                            (xwl_screen->width * 25.4) / monitorResolution,
                            (xwl_screen->height * 25.4) / monitorResolution);

    RROutputSetConnection(xwl_output->randr_output, RR_Connected);

    xwl_output->xwl_screen = xwl_screen;
    xwl_screen->fixed_output = xwl_output;
    xwl_output->xscale = 1.0;

    return TRUE;

err:
    if (xwl_output->randr_crtc)
        RRCrtcDestroy(xwl_output->randr_crtc);
    if (xwl_output->randr_output)
        RROutputDestroy(xwl_output->randr_output);
    free(xwl_output);

    return FALSE;
}

static void
xwl_output_get_xdg_output(struct xwl_output *xwl_output)
{
    struct xwl_screen *xwl_screen = xwl_output->xwl_screen;

    if (!xwl_output->output) {
        /* This can happen when an output is created from a leasable DRM
         * connector */
        return;
    }

    xwl_output->xdg_output =
        zxdg_output_manager_v1_get_xdg_output (xwl_screen->xdg_output_manager,
                                               xwl_output->output);

    zxdg_output_v1_add_listener(xwl_output->xdg_output,
                                &xdg_output_listener,
                                xwl_output);
}

void
xwl_screen_init_xdg_output(struct xwl_screen *xwl_screen)
{
    struct xwl_output *it;

    assert(xwl_screen->xdg_output_manager);

    xorg_list_for_each_entry(it, &xwl_screen->output_list, link)
        xwl_output_get_xdg_output(it);
}
