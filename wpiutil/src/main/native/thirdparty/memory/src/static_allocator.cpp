// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "wpi/memory/static_allocator.hpp"

#include "wpi/memory/detail/debug_helpers.hpp"
#include "wpi/memory/error.hpp"
#include "wpi/memory/memory_arena.hpp"

using namespace wpi::memory;

void* static_allocator::allocate_node(std::size_t size, std::size_t alignment)
{
    auto mem = stack_.allocate(end_, size, alignment);
    if (!mem)
        WPI_THROW(out_of_fixed_memory(info(), size));
    return mem;
}

allocator_info static_allocator::info() const noexcept
{
    return {WPI_MEMORY_LOG_PREFIX "::static_allocator", this};
}

#if WPI_MEMORY_EXTERN_TEMPLATE
template class wpi::memory::allocator_traits<static_allocator>;
#endif

memory_block static_block_allocator::allocate_block()
{
    if (cur_ + block_size_ > end_)
        WPI_THROW(out_of_fixed_memory(info(), block_size_));
    auto mem = cur_;
    cur_ += block_size_;
    return {mem, block_size_};
}

void static_block_allocator::deallocate_block(memory_block block) noexcept
{
    detail::
        debug_check_pointer([&] { return static_cast<char*>(block.memory) + block.size == cur_; },
                            info(), block.memory);
    cur_ -= block_size_;
}

allocator_info static_block_allocator::info() const noexcept
{
    return {WPI_MEMORY_LOG_PREFIX "::static_block_allocator", this};
}
