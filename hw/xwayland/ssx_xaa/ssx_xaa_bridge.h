/*
 * SonicMesa - ssX XAA Hardware Bridge
 * Direct-to-metal 2D acceleration for 5800X3D
 * 
 * Copyright 2026 Collin Beyer, AzuriteShift, and ssX Contributors
 * SPDX-License-Identifier: ssX
 */

#ifndef SSX_XAA_BRIDGE_H
#define SSX_XAA_BRIDGE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * XAA Command Types - Direct mapping to GPU 2D engine
 */
typedef enum {
    SSX_XAA_CMD_NONE = 0,
    SSX_XAA_CMD_INIT,
    SSX_XAA_CMD_SETUP_FOR_SOLID_FILL,
    SSX_XAA_CMD_SUBSEQUENT_SOLID_FILL_RECT,
    SSX_XAA_CMD_SETUP_FOR_SCREEN_TO_SCREEN_COPY,
    SSX_XAA_CMD_SUBSEQUENT_SCREEN_TO_SCREEN_COPY,
    SSX_XAA_CMD_SETUP_FOR_CPU_TO_SCREEN,
    SSX_XAA_CMD_SUBSEQUENT_CPU_TO_SCREEN,
    SSX_XAA_CMD_SETUP_FOR_LINE,
    SSX_XAA_CMD_SUBSEQUENT_LINE,
    SSX_XAA_CMD_SETUP_FOR_STIPPLE,
    SSX_XAA_CMD_SUBSEQUENT_STIPPLE,
    SSX_XAA_CMD_SETUP_FOR_TRIANGLE,
    SSX_XAA_CMD_SUBSEQUENT_TRIANGLE,
    SSX_XAA_CMD_SYNC,
    SSX_XAA_CMD_FLUSH,
    SSX_XAA_CMD_BITBLT,
    SSX_XAA_CMD_FALLBACK_CPU_BLIT,
} ssx_xaa_cmd_type_t;

/*
 * XAA Color Definition - 64-byte aligned for L2/L3 cache
 */
struct __attribute__((aligned(64))) ssx_xaa_color {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t value;
    };
};

/*
 * XAA Point Structure - Cache-line aligned
 */
struct __attribute__((aligned(64))) ssx_xaa_point {
    int16_t x;
    int16_t y;
};

/*
 * XAA Rectangle - Cache-line aligned for vectorized CPU fallback
 */
struct __attribute__((aligned(64))) ssx_xaa_rect {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
};

/*
 * XAA Bitmap - 64-byte aligned for efficient DMA
 */
struct __attribute__((aligned(64))) ssx_xaa_bitmap {
    uint32_t width;
    uint32_t height;
    int32_t pitch;
    const uint8_t *bits;
    uint32_t fg_color;
    uint32_t bg_color;
};

/*
 * XAA Line - For 2D line drawing
 */
struct __attribute__((aligned(64))) ssx_xaa_line {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
};

/*
 * XAA Triangle - For 2D triangle rendering
 */
struct __attribute__((aligned(64))) ssx_xaa_triangle {
    int16_t x1, y1;
    int16_t x2, y2;
    int16_t x3, y3;
};

/*
 * XAA Stipple Pattern - 64-byte aligned
 */
struct __attribute__((aligned(64))) ssx_xaa_stipple {
    uint16_t pattern[32];
};

/*
 * XAA Copy Direction Flags
 */
typedef enum {
    SSX_XAA_COPY_NONE      = 0,
    SSX_XAA_COPY_LEFT      = (1 << 0),
    SSX_XAA_COPY_RIGHT     = (1 << 1),
    SSX_XAA_COPY_UP        = (1 << 2),
    SSX_XAA_COPY_DOWN      = (1 << 3),
} ssx_xaa_copy_dir_t;

/*
 * XAA ROP Code
 */
typedef enum {
    SSX_XAA_ROP_COPY       = 0x0CC,  /* Source */
    SSX_XAA_ROP_INVERT     = 0x055,  /* Dest invert */
    SSX_XAA_ROP_XOR        = 0x066,  /* Dest XOR Source */
    SSX_XAA_ROP_AND        = 0x088,  /* Dest AND Source */
    SSX_XAA_ROP_OR         = 0x0EE,  /* Dest OR Source */
} ssx_xaa_rop_t;

/*
 * XAA BitBlt Parameters - 64-byte aligned
 */
struct __attribute__((aligned(64))) ssx_xaa_bitblt {
    int src_x;
    int src_y;
    int dst_x;
    int dst_y;
    int width;
    int height;
    ssx_xaa_rop_t rop;
    bool trans_pixel;
    uint32_t trans_color;
};

/*
 * XAA Fill Rect Command - For solid rectangle fills
 */
struct __attribute__((aligned(64))) ssx_xaa_fill_rect {
    uint32_t color;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
};

/*
 * XAA Screen-to-Screen Copy - For blitting between surfaces
 */
struct __attribute__((aligned(64))) ssx_xaa_copy {
    int src_x;
    int src_y;
    int dst_x;
    int dst_y;
    int width;
    int height;
    ssx_xaa_copy_dir_t direction;
};

/*
 * XAA CPU-to-Screen Upload - For uploading raw pixel data
 */
struct __attribute__((aligned(64))) ssx_xaa_cpu_to_screen {
    int dst_x;
    int dst_y;
    int width;
    int height;
    int pitch;
    int bpp;
    const uint8_t *pixels;
};

/*
 * XAA Line Command
 */
struct __attribute__((aligned(64))) ssx_xaa_line_cmd {
    uint32_t color;
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
};

/*
 * XAA Stippled Line Command
 */
struct __attribute__((aligned(64))) ssx_xaa_stipple_cmd {
    uint32_t color;
    uint16_t pattern[32];
    ssx_xaa_line_cmd line;
};

/*
 * XAA Triangle Command
 */
struct __attribute__((aligned(64))) ssx_xaa_triangle_cmd {
    uint32_t color;
    ssx_xaa_triangle tri;
};

/*
 * Generic XAA Command Header - 64-byte aligned for io_uring
 */
struct __attribute__((aligned(64))) ssx_xaa_header {
    uint32_t magic;          /* 0x58414100 "XAA\0" */
    uint16_t cmd_type;       /* ssx_xaa_cmd_type_t */
    uint16_t flags;
    uint64_t timestamp;      /* For latency tracking */
    uint32_t surface_handle; /* Target surface */
    uint32_t seq_num;        /* Sequence number */
    uint32_t cmd_size;       /* Size of this command */
    uint32_t reserved;
};

/*
 * XAA Command Union - 64-byte aligned command buffer entry
 */
union __attribute__((aligned(64))) ssx_xaa_command {
    ssx_xaa_header header;
    struct {
        ssx_xaa_header header;
        /* Variable payload follows */
    } generic;
    struct {
        ssx_xaa_header header;
        ssx_xaa_color color;
    } solid_fill;
    struct {
        ssx_xaa_header header;
        ssx_xaa_copy copy;
    } screen_copy;
    struct {
        ssx_xaa_header header;
        ssx_xaa_cpu_to_screen upload;
    } cpu_to_screen;
    struct {
        ssx_xaa_header header;
        ssx_xaa_fill_rect fill;
    } fill_rect;
    struct {
        ssx_xaa_header header;
        ssx_xaa_line_cmd line;
    } line;
    struct {
        ssx_xaa_header header;
        ssx_xaa_stipple_cmd stipple;
    } stipple;
    struct {
        ssx_xaa_header header;
        ssx_xaa_triangle_cmd triangle;
    } triangle;
    struct {
        ssx_xaa_header header;
        ssx_xaa_bitblt bitblt;
        uint32_t src_surface_handle;
    } bitblt;
    struct {
        ssx_xaa_header header;
        ssx_xaa_bitmap bitmap;
    } cpu_blit;
};

/*
 * XAA InfoRec - Core acceleration struct, 64-byte aligned
 * This is the main struct that XAA uses for 2D acceleration
 */
struct __attribute__((aligned(64))) ssx_xaa_info {
    /* Acceleration flags */
    uint32_t acceleration_flags;
    
    /* Scratch pixel depth */
    int scratch_pixel_depth;
    
    /* Function pointers - XAA acceleration vectors */
    void (*SetupForSolidFill)(int color, int rop, uint32_t planemask);
    void (*SubsequentSolidFillRect)(int x, int y, int w, int h);
    void (*SubsequentSolidFillTrapezoid)(int x1, int y1, int x2, int y2, int h);
    void (*SubsequentSolidFillTrap)(int y, int h, int left, int dxL, int dyL, int eL, int dxR, int dyR, int eR);
    
    void (*SetupForScreenToScreenCopy)(int xdir, int ydir, int rop, uint32_t planemask);
    void (*SubsequentScreenToScreenCopy)(int srcX, int srcY, int dstX, int dstY, int w, int h);
    
    void (*SetupForCPUToScreen)(int x, int y, int rop, uint32_t planemask, int bpp);
    void (*SubsequentCPUToScreen)(int x, int y, int w, int h, int pitch, int bpp, const uint8_t *pixels);
    void (*SubsequentMonoCPUToScreen)(int x, int y, int w, int h, int pitch, const uint8_t *pixels, int fgcolor, int bgcolor);
    
    void (*SetupForLine)(int color, int rop, uint32_t planemask);
    void (*SubsequentBresenhamLine)(int x1, int y1, int e, int dx, int dy, int octant);
    void (*SubsequentSeg)(int x1, int y1, int x2, int y2);
    
    void (*SetupForStipple)(int rop, uint32_t planemask);
    void (*SubsequentStippleHalfRes)(int y, int h);
    void (*SubsequentStippledBresenhamLine)(int x1, int y1, int e, int dx, int dy, int octant, int stipple);
    void (*SubsequentStippledLine)(int x1, int y1, int x2, int y2);
    
    void (*SetupForFillSpans)(int rop, uint32_t planemask);
    void (*SubsequentFillSpans)(int x, int y, int *list, int *width, int count);
    
    void (*SetupForSetSpans)(int rop, uint32_t planemask);
    void (*SubsequentSetSpans)(int x, int y, char *list, int *width, int count, int fgcolor, int bgcolor);
    
    void (*SetupForImageRect)(int rop, uint32_t planemask);
    void (*SubsequentImageRect)(int x, int y, int w, int h, const uint8_t *image);
    
    void (*SetupForScanlineImageRect)(int rop, uint32_t planemask);
    void (*SubsequentScanlineImageRect)(int x, int y, int w, int h, const uint8_t *image, int image_width);
    
    void (*SetupForPolyPoint)(int rop, uint32_t planemask);
    void (*SubsequentPolyPoint)(int x, int y);
    void (*SubsequentPolyPointIn)(int x, int y);
    
    void (*SetupForTwoPointLine)(int rop, uint32_t planemask);
    void (*SubsequentTwoPointLine)(int x1, int y1, int x2, int y2, int realx1, int realy1);
    
    void (*SetupForDashedLine)(int fg, int bg, int rop, uint32_t planemask, uint32_t dash_offset);
    void (*SubsequentDashedLine)(int x1, int y1, int x2, int y2);
    
    void (*SetupForDashedTwoPointLine)(int fg, int bg, int rop, uint32_t planemask, uint32_t dash_offset);
    void (*SubsequentDashedTwoPointLine)(int x1, int y1, int x2, int y2);
    
    /* Triangle support */
    void (*SetupForTriangle)(int rop, uint32_t planemask);
    void (*SubsequentTriangle)(int x1, int y1, int x2, int y2, int x3, int y3);
    
    /* Synchronization */
    void (*Sync)(void);
    void (*Flush)(void);
    
    /* Fallback handlers */
    void (*CPUFallback)(int cmd, void *data, int x, int y, int w, int h);
    
    /* Performance tracking */
    uint64_t ops_queued;
    uint64_t ops_completed;
    uint64_t cache_hits;
    uint64_t cache_misses;
    
    /* GPU state */
    uint32_t current_rop;
    uint32_t current_planemask;
    uint32_t current_fg_color;
    uint32_t current_bg_color;
    
    /* Command submission */
    int (*submit_command)(union ssx_xaa_command *cmd);
    
    /* io_uring ring fd */
    int ring_fd;
    
    /* Reserved for future use */
    uint8_t reserved[64];
};

/*
 ═══════════════════════════════════════════════════════════════════════════════
  FUNCTION: ssx_xaa_init
 ───────────────────────────────────────────────────────────────────────────────
  The Sovereign's First Word. Initializes the XAA bridge and binds the 
  acceleration vectors to the GPU's 2D command path.
  
  @param info   - Pointer to XAAInfoRec (must be 64-byte aligned)
  @param ring_fd - File descriptor to io_uring ring (0x504E4943)
  
  EXTERN CLIENT (Redot): Format your XAAInfoRec exactly as defined above.
  Each field must land in L3 cache cleanly before submission.
 ═══════════════════════════════════════════════════════════════════════════════
 */
void ssx_xaa_init(struct ssx_xaa_info *info, int ring_fd);
void ssx_xaa_destroy(struct ssx_xaa_info *info);

/*
 ═══════════════════════════════════════════════════════════════════════════════
  FUNCTION: ssx_xaa_submit_command
 ───────────────────────────────────────────────────────────────────────────────
  Push raw XAA command through the 0x504E4943 ring to the GPU's 2D block.
  No state trackers. No Gallium context. Direct injection.
  
  @param info - XAAInfoRec with bound acceleration vectors
  @param cmd  - 64-byte aligned XAA command union
  
  Returns: 0 on sovereign success, negative errno on failure
  
  REDOT: This is your fire-and-forget path. Batch 100+ commands, then submit.
 ═══════════════════════════════════════════════════════════════════════════════
 */
int ssx_xaa_submit_command(struct ssx_xaa_info *info, union ssx_xaa_command *cmd);

/*
 ═══════════════════════════════════════════════════════════════════════════════
  FUNCTION: ssx_xaa_sync
 ───────────────────────────────────────────────────────────────────────────────
  Drain the 0x504E4943 ring completely. Wait for GPU to consume all pending
  commands before returning control to the X11 thread.
  
  @param info - XAAInfoRec tracking the operations
  
  Returns: 0 when ring is empty, negative errno on failure
 ═══════════════════════════════════════════════════════════════════════════════
 */
int ssx_xaa_sync(struct ssx_xaa_info *info);

/*
 * CPU fallback - runs entirely in L3 cache
 */
void ssx_xaa_cpu_fallback_solid_fill(struct ssx_xaa_info *info, int x, int y, int w, int h, int color);
void ssx_xaa_cpu_fallback_copy(struct ssx_xaa_info *info, int src_x, int src_y, int dst_x, int dst_y, int w, int h);
void ssx_xaa_cpu_fallback_blit(struct ssx_xaa_info *info, const ssx_xaa_bitmap *bmp, int x, int y);

#ifdef __cplusplus
}
#endif

#endif /* SSX_XAA_BRIDGE_H */
