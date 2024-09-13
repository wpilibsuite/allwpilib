// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

#include "wpi/memory/config.hpp"
#if WPI_HOSTED_IMPLEMENTATION

#include "wpi/memory/malloc_allocator.hpp"

#include "wpi/memory/error.hpp"

using namespace wpi::memory;

allocator_info detail::malloc_allocator_impl::info() noexcept
{
    return {WPI_MEMORY_LOG_PREFIX "::malloc_allocator", nullptr};
}

#if WPI_MEMORY_EXTERN_TEMPLATE
template class detail::lowlevel_allocator<detail::malloc_allocator_impl>;
template class wpi::memory::allocator_traits<malloc_allocator>;
#endif

#endif // WPI_HOSTED_IMPLEMENTATION
