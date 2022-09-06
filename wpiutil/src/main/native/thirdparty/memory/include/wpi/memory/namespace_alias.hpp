// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef WPI_MEMORY_NAMESPACE_ALIAS_HPP_INCLUDED
#define WPI_MEMORY_NAMESPACE_ALIAS_HPP_INCLUDED

/// \file
/// Convenient namespace alias.

/// \defgroup core Core components

/// \defgroup allocator Allocator implementations

/// \defgroup adapter Adapters and Wrappers

/// \defgroup storage Allocator storage

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

#endif // WPI_MEMORY_NAMESPACE_ALIAS_HPP_INCLUDED
