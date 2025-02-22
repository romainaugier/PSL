/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#pragma once

#if !defined(__PSL)
#define __PSL

#if defined(_MSC_VER)
#define PSL_MSVC
#pragma warning(disable:4711) /* function selected for automatic inline expansion */
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#elif defined(__GNUC__)
#define PSL_GCC
#elif defined(__clang__)
#define PSL_CLANG
#endif /* defined(_MSC_VER) */

#define PSL_STRIFY(x) #x
#define PSL_STRIFY_MACRO(m) PSL_STRIFY(m)

#if !defined(PSL_VERSION_MAJOR)
#define PSL_VERSION_MAJOR 0
#endif /* !defined(PSL_VERSION_MAJOR) */

#if !defined(PSL_VERSION_MINOR)
#define PSL_VERSION_MINOR 0
#endif /* !defined(PSL_VERSION_MINOR) */

#if !defined(PSL_VERSION_PATCH)
#define PSL_VERSION_PATCH 0
#endif /* !defined(PSL_VERSION_PATCH) */

#if !defined(PSL_VERSION_REVISION)
#define PSL_VERSION_REVISION 0
#endif /* !defined(PSL_VERSION_REVISION) */

#define PSL_VERSION_STR PSL_STRIFY_MACRO(PSL_VERSION_MAJOR)"." \
                              PSL_STRIFY_MACRO(PSL_VERSION_MINOR)"." \
                              PSL_STRIFY_MACRO(PSL_VERSION_PATCH)"." \

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#if INTPTR_MAX == INT64_MAX || defined(__x86_64__)
#define PSL_X64
#define PSL_SIZEOF_PTR 8
#elif INTPTR_MAX == INT32_MAX
#define PSL_X86
#define PSL_SIZEOF_PTR 4
#endif /* INTPTR_MAX == INT64_MAX || defined(__x86_64__) */

#if defined(_WIN32)
#define PSL_WIN
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif /* !defined(WIN32_LEAN_AND_MEAN) */
#if defined(PSL_X64)
#define PSL_PLATFORM_STR "WIN64"
#else
#define PSL_PLATFORM_STR "WIN32"
#endif /* defined(PSL_x64) */
#elif defined(__linux__)
#define PSL_LINUX
#if defined(PSL_X64)
#define PSL_PLATFORM_STR "LINUX64"
#else
#define PSL_PLATFORM_STR "LINUX32"
#endif /* defined(PSL_X64) */
#endif /* defined(_WIN32) */

#if defined(PSL_WIN)
#if defined(PSL_MSVC)
#define PSL_EXPORT __declspec(dllexport)
#define PSL_IMPORT __declspec(dllimport)
#elif defined(PSL_GCC) || defined(PSL_CLANG)
#define PSL_EXPORT __attribute__((dllexport))
#define PSL_IMPORT __attribute__((dllimport))
#endif /* defined(PSL_MSVC) */
#elif defined(PSL_LINUX)
#define PSL_EXPORT __attribute__((visibility("default")))
#define PSL_IMPORT
#endif /* defined(PSL_WIN) */

#if defined(PSL_MSVC)
#define PSL_FORCE_INLINE __forceinline
#define PSL_LIB_ENTRY
#define PSL_LIB_EXIT
#elif defined(PSL_GCC)
#define PSL_FORCE_INLINE inline __attribute__((always_inline)) 
#define PSL_LIB_ENTRY __attribute__((constructor))
#define PSL_LIB_EXIT __attribute__((destructor))
#elif defined(PSL_CLANG)
#define PSL_FORCE_INLINE __attribute__((always_inline))
#define PSL_LIB_ENTRY __attribute__((constructor))
#define PSL_LIB_EXIT __attribute__((destructor))
#endif /* defined(PSL_MSVC) */

#if defined(PSL_BUILD_SHARED)
#define PSL_API PSL_EXPORT
#else
#define PSL_API PSL_IMPORT
#endif /* defined(PSL_BUILD_SHARED) */

#if defined(PSL_WIN)
#define PSL_FUNCTION __FUNCTION__
#elif defined(PSL_GCC) || defined(PSL_CLANG)
#define PSL_FUNCTION __PRETTY_FUNCTION__
#endif /* PSL_WIN */

#define CONCAT_(prefix, suffix)     prefix##suffix
#define CONCAT(prefix, suffix)      CONCAT_(prefix, suffix)

#define PSL_ASSERT(expr, message) if(!(expr)) { fprintf(stderr, "Assertion failed in file %s at line %d: %s", __FILE__, __LINE__, message); abort(); }

#define PSL_STATIC_ASSERT(expr)        \
    struct CONCAT(__outscope_assert_, __COUNTER__)      \
    {                                                   \
        char                                            \
        outscope_assert                                 \
        [2*(expr)-1];                                   \
                                                        \
    } CONCAT(__outscope_assert_, __COUNTER__)

#define PSL_NOT_IMPLEMENTED fprintf(stderr, "Function " PSL_FUNCTION " not implemented"); exit(1);

#if defined(PSL_MSVC)
#define PSL_PACKED_STRUCT(__struct__) __pragma(pack(push, 1)) __struct__ __pragma(pack(pop))
#elif defined(PSL_GCC) || defined(PSL_CLANG)
#define PSL_PACKED_STRUCT(__struct__) __struct__ __attribute__((__packed__))
#else
#define PSL_PACKED_STRUCT(__struct__) __struct__
#endif /* defined(PSL_MSVC) */

#if defined(PSL_MSVC)
#define dump_struct(s) 
#elif defined(PSL_CLANG)
#define dump_struct(s) __builtin_dump_struct(s, printf)
#elif defined(PSL_GCC)
#define dump_struct(s) 
#endif /* defined(PSL_MSVC) */

#if defined(DEBUG_BUILD)
#define PSL_DEBUG 1
#else
#define PSL_DEBUG 0
#endif /* defined(DEBUG_BUILD) */

#if defined(__cplusplus)
#define PSL_CPP_ENTER extern "C" {
#define PSL_CPP_END }
#else
#define PSL_CPP_ENTER
#define PSL_CPP_END
#endif /* defined(__cplusplus) */

#define PSL_ATEXIT_REGISTER(func, exit)                            \
        int res_##func = atexit(func);                                              \
        if(res_##func != 0)                                                         \
        {                                                                           \
            fprintf(stderr, "Cannot register function \""#func"\" in atexit");      \
            if(exit) return exit(1);                                                \
        }                                                                           

#endif /* !defined(__PSL) */
