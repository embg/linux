/*
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of https://github.com/facebook/zstd.
 * An additional grant of patent rights can be found in the PATENTS file in the
 * same directory.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation. This program is dual-licensed; you may select
 * either version 2 of the GNU General Public License ("GPL") or BSD license
 * ("BSD").
 */

#ifndef ZSTD_COMPAT_H
#define ZSTD_COMPAT_H

#include <linux/zstd.h>

#if defined(ZSTD_VERSION_NUMBER) && (ZSTD_VERSION_NUMBER >= 10406)
/*
 * This header provides backwards compatibility for the zstd-1.4.6 library
 * upgrade. This header allows us to upgrade the zstd library version without
 * modifying any callers. Then we will migrate callers from the compatibility
 * wrapper one at a time until none remain. At which point we will delete this
 * header.
 *
 * It is temporary and will be deleted once the upgrade is complete.
 */

#include <linux/zstd_errors.h>

static inline size_t ZSTD_CCtxWorkspaceBound(ZSTD_compressionParameters compression_params)
{
    return ZSTD_estimateCCtxSize_usingCParams(compression_params);
}

static inline size_t ZSTD_CStreamWorkspaceBound(ZSTD_compressionParameters compression_params)
{
    return ZSTD_estimateCStreamSize_usingCParams(compression_params);
}

static inline size_t ZSTD_DCtxWorkspaceBound(void)
{
    return ZSTD_estimateDCtxSize();
}

static inline size_t ZSTD_DStreamWorkspaceBound(unsigned long long window_size)
{
    return ZSTD_estimateDStreamSize(window_size);
}

static inline ZSTD_CCtx* ZSTD_initCCtx(void* wksp, size_t wksp_size)
{
    if (wksp == NULL)
        return NULL;
    return ZSTD_initStaticCCtx(wksp, wksp_size);
}

static inline ZSTD_CStream* ZSTD_initCStream_compat(ZSTD_parameters params, uint64_t pledged_src_size, void* wksp, size_t wksp_size)
{
    ZSTD_CStream* cstream;
    size_t ret;

    if (wksp == NULL)
        return NULL;

    cstream = ZSTD_initStaticCStream(wksp, wksp_size);
    if (cstream == NULL)
        return NULL;

    /* 0 means unknown in old API but means 0 in new API */
    if (pledged_src_size == 0)
        pledged_src_size = ZSTD_CONTENTSIZE_UNKNOWN;

    ret = ZSTD_initCStream_advanced(cstream, NULL, 0, params, pledged_src_size);
    if (ZSTD_isError(ret))
        return NULL;

    return cstream;
}
#define ZSTD_initCStream ZSTD_initCStream_compat

static inline ZSTD_DCtx* ZSTD_initDCtx(void* wksp, size_t wksp_size)
{
    if (wksp == NULL)
        return NULL;
    return ZSTD_initStaticDCtx(wksp, wksp_size);
}

static inline ZSTD_DStream* ZSTD_initDStream_compat(unsigned long long window_size, void* wksp, size_t wksp_size)
{
    if (wksp == NULL)
        return NULL;
    (void)window_size;
    return ZSTD_initStaticDStream(wksp, wksp_size);
}
#define ZSTD_initDStream ZSTD_initDStream_compat

typedef ZSTD_frameHeader ZSTD_frameParams;

static inline size_t ZSTD_getFrameParams(ZSTD_frameParams* frame_params, const void* src, size_t src_size)
{
    return ZSTD_getFrameHeader(frame_params, src, src_size);
}

static inline size_t ZSTD_compressCCtx_compat(ZSTD_CCtx* cctx, void* dst, size_t dst_capacity, const void* src, size_t src_size, ZSTD_parameters params)
{
    return ZSTD_compress_advanced(cctx, dst, dst_capacity, src, src_size, NULL, 0, params);
}
#define ZSTD_compressCCtx ZSTD_compressCCtx_compat

#endif /* ZSTD_VERSION_NUMBER >= 10406 */
#endif /* ZSTD_COMPAT_H */
