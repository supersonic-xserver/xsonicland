/*
 * SonicMesa - ssX XAA io_uring Transport Layer
 * High-performance command submission for 5800X3D
 * 
 * Ring: 0x504E4943 (ASCII: "PNIC" - "PCI NIC" homage)
 * 
 * Copyright 2026 Collin Beyer, AzuriteShift, and ssX Contributors
 * SPDX-License-Identifier: ssX
 */

#ifndef SSX_XAA_IO_URING_H
#define SSX_XAA_IO_URING_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/io_uring.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ═══════════════════════════════════════════════════════════════════════════════
  THE 0x504E4943 RING - "SONIC"
 ───────────────────────────────────────────────────────────────────────────────
  Magic Number: 0x504E4943 (ASCII: 'P' 'N' 'I' 'C')
  
  This is the Sovereign's Highway. Not kernelspace. Not userspace.
  Direct hardware submission for zero-latency 2D operations.
  
  BATCH PROCESS:
  1. Redot accumulates 100+ XAA commands (text, window moves, UI)
  2. Commands are 64-byte aligned for L3 cache efficiency
  3. Batch submitted via single io_uring syscall
  4. GPU 2D engine consumes without blocking X11 thread
  
  FALLBACK: If GPU busy, CPU fallback executes entirely in L3.
 ═══════════════════════════════════════════════════════════════════════════════
 */
#define SSX_XAA_RING_MAGIC    0x504E4943

/*
 * Ring configuration
 */
#define SSX_XAA_RING_SIZE     256          /* Must be power of 2 */
#define SSX_XAA_CMD_QUEUE_SZ  1024         /* Command buffer size */
#define SSX_XAA_SQES_PER_OP   4            /* Max SQE per operation */

/*
 * XAA io_uring opcodes (using IORING_OP_WRITE_FIXED for zero-copy)
 */
enum ssx_xaa_uring_op {
    SSX_URING_OP_NOP          = 0,
    SSX_URING_OP_SUBMIT_CMD   = 1,    /* Submit XAA command */
    SSX_URING_OP_SYNC         = 2,    /* Wait for completion */
    SSX_URING_OP_FLUSH        = 3,    /* Flush all pending */
    SSX_URING_OP_MAP_BUFFER   = 4,    /* Map GPU buffer */
    SSX_URING_OP_UNMAP_BUFFER = 5,    /* Unmap GPU buffer */
};

/*
 * Command submission flags
 */
#define SSX_XAA_SUBMIT_FENCE   (1 << 0)   /* Return fence */
#define SSX_XAA_SUBMIT_BATCH   (1 << 1)   /* Batch with next */
#define SSX_XURING_SUBMIT_DRAIN (1 << 2)   /* Drain before return */

/*
 * io_uring context for XAA
 */
struct __attribute__((aligned(64))) ssx_xaa_ring_ctx {
    int ring_fd;
    uint32_t magic;
    uint32_t flags;
    
    /* Ring buffer pointers (mmap'd) */
    struct io_uring_sqe *sq_ring;
    struct io_uring_cqe *cq_ring;
    uint32_t *sq_array;
    uint8_t *cmd_buffer;
    
    /* Submission state */
    uint32_t sq_head;
    uint32_t sq_tail;
    uint32_t cq_head;
    uint32_t cq_tail;
    uint32_t sq_mask;
    uint32_t cq_mask;
    uint32_t sq_size;
    uint32_t cq_size;
    
    /* Performance counters */
    uint64_t commands_submitted;
    uint64_t commands_completed;
    uint64_t bytes_submitted;
    uint64_t fence_waits;
    
    /* Batch tracking */
    uint32_t batch_count;
    uint32_t batch_threshold;
    
    /* Synchronization */
    int32_t refcount;
};

/*
 * XAA command with metadata for io_uring
 */
struct __attribute__((aligned(64))) ssx_xaa_uring_cmd {
    uint64_t user_data;          /* For matching completion */
    uint32_t opcode;             /* ssx_xaa_uring_op */
    uint32_t flags;
    uint64_t timestamp_queued;   /* When submitted */
    uint64_t timestamp_start;    /* When execution started */
    uint64_t timestamp_complete; /* When completed */
    uint32_t surface_handle;     /* Target surface */
    uint32_t seq_num;            /* Sequence number */
    uint32_t cmd_size;           /* Size of XAA command */
    uint32_t result;             /* Completion result */
    
    /* Variable-length XAA command data follows */
    /* This must be 64-byte aligned for cache efficiency */
    uint8_t cmd_data[];
};

/*
 ═══════════════════════════════════════════════════════════════════════════════
  FUNCTION: ssx_xaa_ring_init
 ───────────────────────────────────────────────────────────────────────────────
  Initialize the 0x504E4943 ring for sovereign command submission.
  Creates io_uring with SQPOLL for asynchronous batch processing.
  
  @param ring_size     - Power of 2 (256, 512, 1024, 2048, 4096)
  @param cmd_buf_size  - Command buffer allocation (1024+ recommended)
  
  @return Ring context (64-byte aligned) or NULL on failure
  
  REDOT: Call this once at startup. Returns handle for all subsequent operations.
 ═══════════════════════════════════════════════════════════════════════════════
 */
struct ssx_xaa_ring_ctx *ssx_xaa_ring_init(uint32_t ring_size, uint32_t cmd_buf_size);

/*
 * Shutdown io_uring ring
 */
void ssx_xaa_ring_shutdown(struct ssx_xaa_ring_ctx *ctx);

/*
 * Submit XAA command via io_uring
 * Uses fixed buffers and prepared buffers for zero-copy
 * 
 * @param ctx  Ring context
 * @param cmd  XAA command (64-byte aligned)
 * @param cmd_size  Command size
 * @param flags  Submission flags
 * @return 0 on success, negative errno on error
 */
int ssx_xaa_ring_submit(struct ssx_xaa_ring_ctx *ctx, const void *cmd, 
                       uint32_t cmd_size, uint32_t flags);

/*
 * Submit batch of XAA commands
 * Optimizes for multiple 2D operations (text, window moves, etc.)
 * 
 * @param ctx  Ring context
 * @param cmds  Array of XAA commands
 * @param count  Number of commands
 * @param flags  Submission flags
 * @return 0 on success, negative errno on error
 */
int ssx_xaa_ring_submit_batch(struct ssx_xaa_ring_ctx *ctx, const void **cmds,
                              uint32_t *cmd_sizes, uint32_t count, uint32_t flags);

/*
 * Wait for command completion
 * 
 * @param ctx  Ring context
 * @param user_data  User data to match (or 0 for any)
 * @param timeout_ns  Timeout in nanoseconds (0 = infinite)
 * @return Completed command or NULL
 */
struct ssx_xaa_uring_cmd *ssx_xaa_ring_wait(struct ssx_xaa_ring_ctx *ctx,
                                             uint64_t user_data,
                                             uint64_t timeout_ns);

/*
 * Get completion without waiting
 * 
 * @param ctx  Ring context
 * @return Completed command or NULL if none available
 */
struct ssx_xaa_uring_cmd *ssx_xaa_ring_try_wait(struct ssx_xaa_ring_ctx *ctx);

/*
 * Drain all pending commands
 * 
 * @param ctx  Ring context
 * @return 0 on success, negative errno on error
 */
int ssx_xaa_ring_drain(struct ssx_xaa_ring_ctx *ctx);

/*
 * Register file descriptors for zero-copy
 * 
 * @param ctx  Ring context
 * @param fd  File descriptor to register
 * @return 0 on success, negative errno on error
 */
int ssx_xaa_ring_register_fd(struct ssx_xaa_ring_ctx *ctx, int fd);

/*
 * Get ring file descriptor
 * 
 * @param ctx  Ring context
 * @return File descriptor or -1
 */
static inline int ssx_xaa_ring_get_fd(struct ssx_xaa_ring_ctx *ctx)
{
    return ctx ? ctx->ring_fd : -1;
}

/*
 * Get queue depth
 * 
 * @param ctx  Ring context
 * @return Available submission slots
 */
static inline uint32_t ssx_xaa_ring_available(struct ssx_xaa_ring_ctx *ctx)
{
    if (!ctx) return 0;
    return (ctx->sq_tail - ctx->sq_head) & ctx->sq_mask;
}

/*
 * Performance monitoring
 */
struct ssx_xaa_ring_stats {
    uint64_t commands_submitted;
    uint64_t commands_completed;
    uint64_t bytes_submitted;
    uint64_t fence_waits;
    uint64_t batch_submissions;
    uint32_t avg_batch_size;
    uint32_t peak_queue_depth;
    double avg_latency_us;
    double max_latency_us;
};

void ssx_xaa_ring_get_stats(struct ssx_xaa_ring_ctx *ctx, 
                            struct ssx_xaa_ring_stats *stats);

#ifdef __cplusplus
}
#endif

#endif /* SSX_XAA_IO_URING_H */
