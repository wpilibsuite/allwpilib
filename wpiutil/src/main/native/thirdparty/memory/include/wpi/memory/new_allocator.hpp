// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef WPI_MEMORY_NEW_ALLOCATOR_HPP_INCLUDED
#define WPI_MEMORY_NEW_ALLOCATOR_HPP_INCLUDED

/// \file
/// Class \ref wpi::memory::new_allocator.

#include "detail/lowlevel_allocator.hpp"
#include "config.hpp"

#if WPI_MEMORY_EXTERN_TEMPLATE
#include "allocator_traits.hpp"
#endif

namespace wpi
{
    namespace memory
    {
        struct allocator_info;

        namespace detail
        {
            struct new_allocator_impl
            {
                static allocator_info info() noexcept;

                static void* allocate(std::size_t size, std::size_t) noexcept;

                static void deallocate(void* ptr, std::size_t size, std::size_t) noexcept;

                static std::size_t max_node_size() noexcept;
            };

            WPI_MEMORY_LL_ALLOCATOR_LEAK_CHECKER(new_allocator_impl,
                                                       new_alloator_leak_checker)
        } // namespace detail

        /// A stateless \concept{concept_rawallocator,RawAllocator} that allocates memory using (nothrow) <tt>operator new</tt>.
        /// If the operator returns \c nullptr, it behaves like \c new and loops calling \c std::new_handler,
        /// but instead of throwing a \c std::bad_alloc exception, it throws \ref out_of_memory.
        /// \ingroup allocator
        using new_allocator =
            WPI_IMPL_DEFINED(detail::lowlevel_allocator<detail::new_allocator_impl>);

#if WPI_MEMORY_EXTERN_TEMPLATE
        extern template class detail::lowlevel_allocator<detail::new_allocator_impl>;
        extern template class allocator_traits<new_allocator>;
#endif
    } // namespace memory
} // namespace wpi

#endif // WPI_MEMORY_NEW_ALLOCATOR_HPP_INCLUDED
