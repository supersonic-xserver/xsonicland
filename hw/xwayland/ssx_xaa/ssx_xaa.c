/*
 * SonicMesa - ssX XAA Implementation (XSonicLand Port)
 * Direct-to-metal 2D acceleration for 5800X3D
 * 
 * Copyright 2026 Collin Beyer, AzuriteShift, and ssX Contributors
 * SPDX-License-Identifier: ssX
 */

#include "ssx_xaa_bridge.h"
#include "ssx_xaa_io_uring.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/io_uring.h>

/*
 * Global ring context - initialized once
 */
static struct ssx_xaa_ring_ctx *g_ring_ctx = NULL;
static uint32_t g_seq_num = 0;

/*
 * XAA Init - Initialize the ssX XAA bridge
 */
void ssx_xaa_init(struct ssx_xaa_info *info, int ring_fd)
{
    if (!info) return;
    
    memset(info, 0, sizeof(*info));
    
    info->ring_fd = ring_fd;
    
    /* Set default flags */
    info->acceleration_flags = 0x7FFFFFFF; /* All acceleration enabled */
    
    /* Performance counters */
    info->ops_queued = 0;
    info->ops_completed = 0;
    info->cache_hits = 0;
    info->cache_misses = 0;
}

/*
 * XAA Destroy - Cleanup
 */
void ssx_xaa_destroy(struct ssx_xaa_info *info)
{
    if (!info) return;
    
    memset(info, 0, sizeof(*info));
}

/*
 * Ring initialization - The Sovereign's Highway
 */
struct ssx_xaa_ring_ctx *ssx_xaa_ring_init(uint32_t ring_size, uint32_t cmd_buf_size)
{
    struct ssx_xaa_ring_ctx *ctx;
    struct io_uring_params params = {0};
    
    /* Validate ring size is power of 2 */
    if (ring_size & (ring_size - 1)) {
        fprintf(stderr, "ssX XAA: Ring size must be power of 2\n");
        return NULL;
    }
    
    /* Allocate context - 64-byte aligned for L3 cache efficiency */
    ctx = (struct ssx_xaa_ring_ctx *)memalign(64, sizeof(*ctx));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(*ctx));
    
    /* Set magic: 0x534F4E4943 "SONIC" - Sonic homage */
    ctx->magic = SSX_XAA_RING_MAGIC;
    ctx->flags = 0;
    
    /* Setup io_uring params - SQPOLL for asynchronous batch processing */
    params.flags = IORING_SETUP_SQPOLL;
    params.sq_thread_idle = 1000;  /* 1 second idle before sleep */
    
    /* Create io_uring ring */
    int ring_fd = io_uring_queue_init_params(ring_size, &params, 0);
    if (ring_fd < 0) {
        fprintf(stderr, "ssX XAA: io_uring_queue_init_params failed: %d\n", -ring_fd);
        free(ctx);
        return NULL;
    }
    
    ctx->ring_fd = ring_fd;
    
    /* Get ring sizes */
    ctx->sq_size = ring_size;
    ctx->cq_size = ring_size * 2;  /* CQ is typically 2x SQ */
    ctx->sq_mask = ring_size - 1;
    ctx->cq_mask = (ring_size * 2) - 1;
    
    /* Allocate command buffer - 64-byte aligned for L3 cache */
    ctx->cmd_buffer = (uint8_t *)memalign(64, cmd_buf_size);
    if (!ctx->cmd_buffer) {
        close(ctx->ring_fd);
        free(ctx);
        return NULL;
    }
    
    /* Initialize counters */
    ctx->commands_submitted = 0;
    ctx->commands_completed = 0;
    ctx->bytes_submitted = 0;
    ctx->fence_waits = 0;
    ctx->batch_count = 0;
    ctx->batch_threshold = 16;
    ctx->refcount = 1;
    
    return ctx;
}

/*
 * Ring shutdown
 */
void ssx_xaa_ring_shutdown(struct ssx_xaa_ring_ctx *ctx)
{
    if (!ctx) return;
    
    if (--ctx->refcount > 0) return;
    
    if (ctx->cmd_buffer) {
        free(ctx->cmd_buffer);
        ctx->cmd_buffer = NULL;
    }
    
    if (ctx->ring_fd >= 0) {
        close(ctx->ring_fd);
        ctx->ring_fd = -1;
    }
    
    free(ctx);
}

/*
 * Submit command via io_uring - Zero-copy path
 */
int ssx_xaa_ring_submit(struct ssx_xaa_ring_ctx *ctx, const void *cmd, 
                       uint32_t cmd_size, uint32_t flags)
{
    struct io_uring_sqe *sqe;
    int ret;
    
    if (!ctx || !cmd || ctx->ring_fd < 0) return -EINVAL;
    
    /* Get submission queue entry */
    sqe = io_uring_get_sqe(ctx->ring_fd);
    if (!sqe) {
        /* Ring full, try to reap first */
        ret = io_uring_submit(ctx->ring_fd);
        if (ret < 0) return ret;
        
        sqe = io_uring_get_sqe(ctx->ring_fd);
        if (!sqe) return -EAGAIN;
    }
    
    /* Copy command to ring buffer - 64-byte aligned for cache efficiency */
    uint32_t slot = ctx->sq_tail & ctx->sq_mask;
    memcpy(ctx->cmd_buffer + (slot * 64), cmd, cmd_size);
    
    /* Prepare SQE for write with fixed buffer for zero-copy */
    io_uring_sqe_set_data(sqe, (void *)(uintptr_t)slot);
    io_uring_prep_write(sqe, -1, ctx->cmd_buffer + (slot * 64), cmd_size, 0);
    
    /* Set flags */
    if (flags & SSX_XAA_SUBMIT_FENCE) {
        sqe->flags |= IOSQE_IO_DRAIN;
    }
    if (flags & SSX_XAA_SUBMIT_BATCH) {
        sqe->flags |= IOSQE_IO_LINK;
    }
    
    /* Advance tail */
    ctx->sq_tail++;
    
    /* Update counters */
    ctx->commands_submitted++;
    ctx->bytes_submitted += cmd_size;
    
    /* Submit if not batching */
    if (!(flags & SSX_XAA_SUBMIT_BATCH)) {
        ret = io_uring_submit(ctx->ring_fd);
        if (ret < 0) return ret;
    }
    
    return 0;
}

/*
 * Submit batch of XAA commands - Optimized for terminal scrolling, window dragging
 */
int ssx_xaa_ring_submit_batch(struct ssx_xaa_ring_ctx *ctx, const void **cmds,
                              uint32_t *cmd_sizes, uint32_t count, uint32_t flags)
{
    struct io_uring_sqe *sqe = NULL;
    uint32_t i;
    int ret;
    
    if (!ctx || !cmds || !cmd_sizes || !count) return -EINVAL;
    
    for (i = 0; i < count; i++) {
        sqe = io_uring_get_sqe(ctx->ring_fd);
        if (!sqe) {
            /* Ring full, flush what we have */
            ret = io_uring_submit(ctx->ring_fd);
            if (ret < 0) return (i > 0) ? 0 : ret;
            
            sqe = io_uring_get_sqe(ctx->ring_fd);
            if (!sqe) return (i > 0) ? 0 : -EAGAIN;
        }
        
        /* Copy to ring buffer */
        uint32_t slot = (ctx->sq_tail + i) & ctx->sq_mask;
        memcpy(ctx->cmd_buffer + (slot * 64), cmds[i], cmd_sizes[i]);
        
        /* Prepare SQE */
        io_uring_sqe_set_data(sqe, (void *)(uintptr_t)slot);
        io_uring_prep_write(sqe, -1, ctx->cmd_buffer + (slot * 64), cmd_sizes[i], 0);
        
        /* Link all commands together for atomic batch */
        if (i < count - 1) {
            sqe->flags |= IOSQE_IO_LINK;
        }
    }
    
    ctx->sq_tail += count;
    ctx->commands_submitted += count;
    ctx->batch_count++;
    
    /* Submit batch */
    ret = io_uring_submit(ctx->ring_fd);
    if (ret < 0) return ret;
    
    return 0;
}

/*
 * Wait for command completion
 */
struct ssx_xaa_uring_cmd *ssx_xaa_ring_wait(struct ssx_xaa_ring_ctx *ctx,
                                            uint64_t user_data,
                                            uint64_t timeout_ns)
{
    struct io_uring_cqe *cqe;
    int ret;
    
    if (!ctx || ctx->ring_fd < 0) return NULL;
    
    if (timeout_ns > 0) {
        struct __kernel_timespec ts;
        ts.tv_sec = timeout_ns / 1000000000;
        ts.tv_nsec = timeout_ns % 1000000000;
        ret = io_uring_wait_cqe_timeout(ctx->ring_fd, &cqe, &ts);
    } else {
        ret = io_uring_wait_cqe(ctx->ring_fd, &cqe);
    }
    
    if (ret < 0) {
        if (ret == -EAGAIN || ret == -ETIME) return NULL;
        fprintf(stderr, "ssX XAA: wait_cqe failed: %d\n", ret);
        return NULL;
    }
    
    /* Mark cqe seen */
    io_uring_cqe_seen(ctx->ring_fd, cqe);
    
    ctx->commands_completed++;
    ctx->cq_head++;
    
    /* Return pointer to completed command in ring buffer */
    return (struct ssx_xaa_uring_cmd *)(ctx->cmd_buffer + ((uintptr_t)cqe->user_data * 64));
}

/*
 * Try to get completion without waiting
 */
struct ssx_xaa_uring_cmd *ssx_xaa_ring_try_wait(struct ssx_xaa_ring_ctx *ctx)
{
    struct io_uring_cqe *cqe;
    int ret;
    
    if (!ctx || ctx->ring_fd < 0) return NULL;
    
    ret = io_uring_peek_cqe(ctx->ring_fd, &cqe);
    if (ret < 0) return NULL;
    
    if (!cqe) return NULL;
    
    io_uring_cqe_seen(ctx->ring_fd, cqe);
    ctx->commands_completed++;
    ctx->cq_head++;
    
    return (struct ssx_xaa_uring_cmd *)(ctx->cmd_buffer + ((uintptr_t)cqe->user_data * 64));
}

/*
 * Drain all pending commands - Wait for GPU to consume all
 */
int ssx_xaa_ring_drain(struct ssx_xaa_ring_ctx *ctx)
{
    struct io_uring_cqe *cqe;
    int ret;
    
    if (!ctx || ctx->ring_fd < 0) return 0;
    
    /* Submit any pending */
    ret = io_uring_submit(ctx->ring_fd);
    if (ret < 0) return ret;
    
    /* Wait for all completions */
    while (ctx->sq_head != ctx->sq_tail) {
        ret = io_uring_wait_cqe(ctx->ring_fd, &cqe);
        if (ret < 0) {
            if (ret == -EINTR) continue;
            return ret;
        }
        
        io_uring_cqe_seen(ctx->ring_fd, cqe);
        ctx->commands_completed++;
        ctx->sq_head++;
    }
    
    return 0;
}

/*
 * Register file descriptor for zero-copy
 */
int ssx_xaa_ring_register_fd(struct ssx_xaa_ring_ctx *ctx, int fd)
{
    if (!ctx || ctx->ring_fd < 0) return -EINVAL;
    
    return io_uring_register(ctx->ring_fd, IORING_REGISTER_FILES, &fd, 1);
}

/*
 * Get ring statistics
 */
void ssx_xaa_ring_get_stats(struct ssx_xaa_ring_ctx *ctx, 
                            struct ssx_xaa_ring_stats *stats)
{
    if (!ctx || !stats) return;
    
    memset(stats, 0, sizeof(*stats));
    
    stats->commands_submitted = ctx->commands_submitted;
    stats->commands_completed = ctx->commands_completed;
    stats->bytes_submitted = ctx->bytes_submitted;
    stats->fence_waits = ctx->fence_waits;
    stats->batch_submissions = ctx->batch_count;
    stats->avg_batch_size = ctx->batch_count ? 
                            (uint32_t)(ctx->commands_submitted / ctx->batch_count) : 0;
}