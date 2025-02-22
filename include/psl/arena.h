/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#pragma once

#if !defined(__PSL_ARENA)
#define __PSL_ARENA

#include "psl/psl.h"

#define ARENA_GROWTH_RATE 1.6180339887f

typedef struct
{
    void* ptr;
    size_t capacity;
    size_t offset;
} Arena;

PSL_API void psl_arena_init(Arena* arena, const size_t size);

PSL_API void psl_arena_resize(Arena* arena);

PSL_API void* psl_arena_push(Arena* arena, void* data, const size_t data_size);

PSL_API void* psl_arena_at(Arena* arena, const size_t offset);

PSL_API void psl_arena_destroy(Arena* arena);

#endif /* !defined(__PSL_ARENA) */