// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// Copyright (C) 2015-2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#pragma once

#include <cstddef>

//=== options ===//
#define WPI_MEMORY_CHECK_ALLOCATION_SIZE 1
#define WPI_MEMORY_IMPL_DEFAULT_ALLOCATOR heap_allocator
#ifdef NDEBUG
#define WPI_MEMORY_DEBUG_ASSERT 0
#define WPI_MEMORY_DEBUG_FILL 0
#define WPI_MEMORY_DEBUG_FENCE 0
#define WPI_MEMORY_DEBUG_LEAK_CHECK 0
#define WPI_MEMORY_DEBUG_POINTER_CHECK 0
#define WPI_MEMORY_DEBUG_DOUBLE_DEALLOC_CHECK 0
#else
#define WPI_MEMORY_DEBUG_ASSERT 1
#define WPI_MEMORY_DEBUG_FILL 1
#define WPI_MEMORY_DEBUG_FENCE 8
#define WPI_MEMORY_DEBUG_LEAK_CHECK 1
#define WPI_MEMORY_DEBUG_POINTER_CHECK 1
#define WPI_MEMORY_DEBUG_DOUBLE_DEALLOC_CHECK 1
#endif
#define WPI_MEMORY_EXTERN_TEMPLATE 1
#define WPI_MEMORY_TEMPORARY_STACK_MODE 2

#define WPI_MEMORY_NO_NODE_SIZE 1
