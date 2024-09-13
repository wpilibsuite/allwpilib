// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

#include "wpi/memory/memory_pool.hpp"

#include "wpi/memory/debugging.hpp"

using namespace wpi::memory;

void detail::memory_pool_leak_handler::operator()(std::ptrdiff_t amount)
{
    get_leak_handler()({WPI_MEMORY_LOG_PREFIX "::memory_pool", this}, amount);
}

#if WPI_MEMORY_EXTERN_TEMPLATE
template class wpi::memory::memory_pool<node_pool>;
template class wpi::memory::memory_pool<array_pool>;
template class wpi::memory::memory_pool<small_node_pool>;

template class wpi::memory::allocator_traits<memory_pool<node_pool>>;
template class wpi::memory::allocator_traits<memory_pool<array_pool>>;
template class wpi::memory::allocator_traits<memory_pool<small_node_pool>>;

template class wpi::memory::composable_allocator_traits<memory_pool<node_pool>>;
template class wpi::memory::composable_allocator_traits<memory_pool<array_pool>>;
template class wpi::memory::composable_allocator_traits<memory_pool<small_node_pool>>;
#endif
