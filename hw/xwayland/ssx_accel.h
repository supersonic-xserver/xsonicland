/* * JESTERMAN'S CREED:
 * This code is a sovereign expression of technical freedom. 
 * It exists outside the reach of non-contributing censors and 
 * "Archon" administrative overreach. The creator's intent is 
 * the absolute law of the repository.
 *
 * SPDX-License-Identifier: ssX
 * Copyright (c) 2026 Collin Beyer. All Rights Reserved.
 * Co-authored by azuriteshift.
 */

/*
 * ssXLibre Acceleration Bridge - Header
 * 
 * Procedural 2D rendering pipeline header for X11-over-Wayland bridge.
 * Provides sub-3ms latency for window movements, blits, and surface updates
 * by leveraging XLibre's optimized fb/mi paths and direct DMABUF handoff.
 *
 * Copyright © 2026 ssXLibre Contributors Collin Beyer and azuriteshift
 * 
 * This file is part of ssXLibre and is subject to the terms and conditions
 * defined in the ssX Supplemental License (LICENSE) file.
 * SPDX-License-Identifier: ssX
 */

#ifndef SSX_ACCEL_H
#define SSX_ACCEL_H

#include <X11/X.h>
#include <windowstr.h>
#include <gcstruct.h>
#include <present_priv.h>
#include "xwayland-types.h"

/* ssX XAA Bridge Integration */
#include "ssx_xaa/ssx_xaa_bridge.h"
#include "ssx_xaa/ssx_xaa_io_uring.h"

/* Forward declarations */
struct wl_global;
struct xwl_window;
struct xwl_screen;
struct xwl_present_window;
struct gbm_device;
struct gbm_bo;

/* TearFree mode flags */
typedef enum {
    SSX_MODE_UNKNOWN = 0,
    SSX_MODE_CPU_FALLBACK,      /* No GPU acceleration available */
    SSX_MODE_TEARFREE,          /* Universal Planes + Glamor = TearFree */
    SSX_MODE_TRIPLE_BUFFER      /* No Universal Planes, triple-buffer fallback */
} ssx_render_mode_t;

/* Deep color support states */
typedef enum {
    SSX_COLOR_DEPTH_8BIT = 0,
    SSX_COLOR_DEPTH_10BIT,      /* HDR10, Dolby Vision */
    SSX_COLOR_DEPTH_12BIT       /* Professional displays */
} ssx_color_depth_t;

/* Glamor surface handoff states */
typedef struct _ssx_glamor_surface {
    struct gbm_device *gbm_device;
    struct gbm_bo *gbm_bo_current;
    struct gbm_bo *gbm_bo_previous;
    struct gbm_bo *gbm_bo_shadow;   /* Triple-buffer shadow buffer */
    void *egl_context;
    Bool surface_bound;
    ssx_render_mode_t render_mode;
    ssx_color_depth_t color_depth;
} ssx_glamor_surface_t;

/*
 * ssXLibre TearFree Toggle
 * 
 * Controls VSync behavior in the presentation engine:
 * - Default (OFF): Immediate-present mode, frames pushed as fast as GPU renders
 * - ON (-tearfree flag or SSX_TEARFREE_ENABLE atom): VSync enabled, shadow buffer flipping
 */

/* Command-line flag state (set from xwayland.c) */
extern int ssx_tearfree_requested;

/* Atom for runtime TearFree toggle (X11 client can set this) */
#define SSX_TEARFREE_ATOM_NAME "SSX_TEARFREE_ENABLE"

/**
 * ssx_tearfree_get_state - Get current TearFree toggle state
 * 
 * Checks both the command-line flag and the runtime atom.
 * Command-line takes precedence on startup.
 * 
 * Returns: TRUE if TearFree/VSync is active, FALSE for immediate-present
 */
Bool ssx_tearfree_get_state(void);

/**
 * ssx_tearfree_set_atom - Register the TearFree toggle atom
 * 
 * Called during X11 atom initialization.
 */
void ssx_tearfree_set_atom(Atom atom);

/* ssXLibre API declarations */

/**
 * ssx_accel_init - Initialize ssXLibre acceleration subsystem
 * @xwl_screen: Xwayland screen pointer
 * 
 * Returns: TRUE if acceleration is available
 */
Bool ssx_accel_init(struct xwl_screen *xwl_screen);

/**
 * ssx_accel_available - Check if ssXLibre acceleration is available
 */
Bool ssx_accel_available(void);

/**
 * ssx_accel_shutdown - Cleanup ssXLibre acceleration
 */
void ssx_accel_shutdown(void);

/* Compositor capability checks */

/**
 * ssx_compositor_check - Check if compositor supports ssXLibre acceleration
 * @compositor: Wayland compositor global
 * 
 * Returns: TRUE if ssXLibre can be used
 */
Bool ssx_compositor_check(struct wl_global *compositor);

/* DMA-BUF plane operations */

/**
 * ssx_dmabuf_init - Initialize DMA-BUF plane
 * @xwl_screen: Xwayland screen pointer
 * 
 * Returns: TRUE on success
 */
Bool ssx_dmabuf_init(struct xwl_screen *xwl_screen);

/**
 * ssx_dmabuf_copy - Direct surface-to-surface blit via DMA-BUF
 * @xwl_window: Target Xwayland window
 * @src: Source drawable
 * @gc: Graphics context
 * 
 * Returns: TRUE if ssXLibre path was used
 */
Bool ssx_dmabuf_copy(struct xwl_window *xwl_window,
                    DrawablePtr src, GCPtr gc,
                    int srcx, int srcy,
                    int width, int height,
                    int dstx, int dsty);

/**
 * ssx_dmabuf_shutdown - Cleanup DMA-BUF plane
 */
void ssx_dmabuf_shutdown(void);

/* GPU direct upload */

/**
 * ssx_gpu_upload - Direct GPU upload path for pixel data
 * @xwl_window: Target Xwayland window
 * @data: Pixel data buffer
 * @width: Image width
 * @height: Image height
 * @dst_x: Destination X
 * @dst_y: Destination Y
 * @format: Pixel format
 * 
 * Returns: TRUE if ssXLibre path was used
 */
Bool ssx_gpu_upload(struct xwl_window *xwl_window,
                    char *data, int width, int height,
                    int dst_x, int dst_y,
                    unsigned int format);

/* Surface operations */

/**
 * ssx_surface_clear - Direct compositor clear
 * @xwl_window: Target window
 * @pixel: Clear color
 * 
 * Returns: TRUE if ssXLibre path was used
 */
Bool ssx_surface_clear(struct xwl_window *xwl_window, Pixel pixel);

/**
 * ssx_fill_rectangles - Optimized rectangle fill
 * @xwl_window: Target window
 * @gc: Graphics context
 * @nrect: Number of rectangles
 * @rects: Rectangle array
 * 
 * Returns: TRUE if ssXLibre path was used
 */
Bool ssx_fill_rectangles(struct xwl_window *xwl_window, GCPtr gc,
                         int nrect, xRectangle *rects);

/**
 * ssx_damage_check - Check damage state without X round-trip
 * @xwl_window: Window to check
 * @stamp: Expected damage stamp
 * 
 * Returns: TRUE if damage matches stamp (no update needed)
 */
Bool ssx_damage_check(struct xwl_window *xwl_window, uint32_t stamp);

/* Rendering entry points */

/**
 * ssx_accel_copy_area - Fast copy path for window movement
 */
void ssx_accel_copy_area(DrawablePtr src, DrawablePtr dst,
                         GCPtr gc, int srcx, int srcy,
                         int width, int height, int dstx, int dsty);

/**
 * ssx_accel_put_image - Accelerated image upload
 */
void ssx_accel_put_image(DrawablePtr drawable, GCPtr gc,
                         int width, int height, int dst_x, int dst_y,
                         int left, int right, int top, int bottom,
                         unsigned int format, char *data);

/**
 * ssx_accel_clear_window - Fast window clear
 */
void ssx_accel_clear_window(WindowPtr window, Pixel pixel);

/**
 * ssx_accel_poly_fill_rect - Fast rectangle fill
 */
void ssx_accel_poly_fill_rect(DrawablePtr drawable, GCPtr gc,
                              int nrect, xRectangle *rects);

/**
 * ssx_accel_check_stamp - Check if window needs redraw
 */
Bool ssx_accel_check_stamp(WindowPtr window, uint32_t stamp);

/**
 * ssx_accel_present - DMABUF presentation for Wayland compositor
 * 
 * Zero-copy handoff of XLibre-rendered buffers to the Wayland compositor.
 * 
 * @window: Target window
 * @pixmap: Pixmap to present
 * @target_msc: Target MSC for vblank
 * @divisor: Vblank divisor
 * @remainder: Vblank remainder
 * @options: Present options
 * 
 * Returns: TRUE if presented via DMABUF fast path
 */
Bool ssx_accel_present(WindowPtr window, PixmapPtr pixmap,
                       uint64_t *target_msc, uint64_t divisor, uint64_t remainder,
                       uint32_t options);

/*
 * ssX XAA Bridge API - Direct 2D acceleration via 0x534F4E4943 io_uring ring
 * 
 * These functions provide access to the ssx_xaa bridge for hardware-
 * accelerated 2D primitives.
 */

/**
 * ssx_xaa_get_info - Get XAAInfoRec for driver integration
 * 
 * Returns the initialized XAAInfoRec with bound acceleration vectors.
 */
struct ssx_xaa_info *ssx_xaa_get_info(void);

/**
 * ssx_xaa_get_ring_fd - Get io_uring ring file descriptor
 * 
 * Returns the 0x534F4E4943 ring FD for X server integration.
 */
int ssx_xaa_get_ring_fd(void);

#endif /* SSX_ACCEL_H */
