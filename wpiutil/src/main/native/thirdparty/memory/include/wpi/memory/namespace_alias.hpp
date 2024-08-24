// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

#ifndef WPI_MEMORY_NAMESPACE_ALIAS_HPP_INCLUDED
#define WPI_MEMORY_NAMESPACE_ALIAS_HPP_INCLUDED

/// \file
/// Convenient namespace alias.

/// \defgroup memory Memory Allocator Library
/// @{

/// \defgroup memory_core Core components

/// \defgroup memory_allocator Allocator implementations

/// \defgroup memory_adapter Adapters and Wrappers

/// \defgroup memory_storage Allocator storage

/// @}

/// \namespace wpi
/// Foonathan namespace.

/// \namespace wpi::memory
/// Memory namespace.

/// \namespace wpi::memory::literals
/// Literals namespace.

namespace wpi
{
    namespace memory
    {
    }
} // namespace wpi

namespace memory = wpi::memory;
///@}
#endif // WPI_MEMORY_NAMESPACE_ALIAS_HPP_INCLUDED
