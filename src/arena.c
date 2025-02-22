/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#include "psl/arena.h"

#include <stdlib.h>
#include <string.h>

void psl_arena_init(Arena* arena, const size_t size)
{
    arena->ptr = malloc(size);
    arena->offset = 0;
    arena->capacity = size;
}

PSL_FORCE_INLINE bool psl_arena_check_resize(Arena* arena, 
                                                         const size_t new_size)
{
    return (arena->offset + new_size) >= arena->capacity;
}

void psl_arena_resize(Arena* arena)
{
    const size_t new_capacity = (size_t)((float)arena->capacity * ARENA_GROWTH_RATE);

    void* new_ptr = realloc(arena->ptr, new_capacity);

    PSL_ASSERT(new_ptr != NULL, "Error during arena reallocation");

    arena->ptr = new_ptr;
    arena->capacity = new_capacity;
}

void* psl_arena_push(Arena* arena, void* data, const size_t data_size)
{
    if(psl_arena_check_resize(arena, data_size))
    {
        psl_arena_resize(arena);
    }

    void* data_address = (void*)((char*)arena->ptr + arena->offset);
    
    if(data != NULL)
    {
        memcpy(data_address, data, data_size);
    }

    arena->offset += data_size;

    return data_address;
}

void* psl_arena_at(Arena* arena, const size_t offset)
{
    PSL_ASSERT(offset < arena->capacity, "offset too large");

    void* data_address = (char*)arena->ptr + offset;

    return data_address;
}

void psl_arena_destroy(Arena* arena)
{
    free(arena->ptr);
    arena->ptr = NULL;
    arena->offset = 0;
    arena->capacity = 0;
}