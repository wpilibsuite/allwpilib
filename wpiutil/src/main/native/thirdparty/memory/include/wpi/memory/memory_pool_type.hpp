// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

#ifndef WPI_MEMORY_MEMORY_POOL_TYPE_HPP_INCLUDED
#define WPI_MEMORY_MEMORY_POOL_TYPE_HPP_INCLUDED

/// \file
/// The \c PoolType tag types.

#include <type_traits>

#include "detail/free_list.hpp"
#include "detail/small_free_list.hpp"
#include "config.hpp"

namespace wpi
{
    namespace memory
    {
        /// Tag type defining a memory pool optimized for nodes.
        /// It does not support array allocations that great and may trigger a growth even if there is enough memory.
        /// But it is the fastest pool type.
        /// \ingroup allocator
        struct node_pool : WPI_EBO(std::true_type)
        {
            using type = detail::node_free_memory_list;
        };

        /// Tag type defining a memory pool optimized for arrays.
        /// It keeps the nodes ordered inside the free list and searches the list for an appropriate memory block.
        /// Array allocations are still pretty slow, if the array gets big enough it can get slower than \c new.
        /// Node allocations are still fast, unless there is deallocation in random order.
        /// \note Use this tag type only if you really need to have a memory pool!
        /// \ingroup allocator
        struct array_pool : WPI_EBO(std::true_type)
        {
            using type = detail::array_free_memory_list;
        };

        /// Tag type defining a memory pool optimized for small nodes.
        /// The free list is intrusive and thus requires that each node has at least the size of a pointer.
        /// This tag type does not have this requirement and thus allows zero-memory-overhead allocations of small nodes.
        /// It is a little bit slower than \ref node_pool and does not support arrays.
        /// \ingroup allocator
        struct small_node_pool : WPI_EBO(std::false_type)
        {
            using type = detail::small_free_memory_list;
        };
    } // namespace memory
} // namespace wpi

#endif // WPI_MEMORY_MEMORY_POOL_TYPE_HPP_INCLUDED
