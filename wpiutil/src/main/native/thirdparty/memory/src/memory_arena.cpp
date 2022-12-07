// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "wpi/memory/memory_arena.hpp"

#include <new>

#include "wpi/memory/detail/align.hpp"

using namespace wpi::memory;
using namespace detail;

void memory_block_stack::push(allocated_mb block) noexcept
{
    WPI_MEMORY_ASSERT(block.size >= sizeof(node));
    WPI_MEMORY_ASSERT(is_aligned(block.memory, max_alignment));
    auto next = ::new (block.memory) node(head_, block.size - implementation_offset());
    head_     = next;
}

memory_block_stack::allocated_mb memory_block_stack::pop() noexcept
{
    WPI_MEMORY_ASSERT(head_);
    auto to_pop = head_;
    head_       = head_->prev;
    return {to_pop, to_pop->usable_size + implementation_offset()};
}

void memory_block_stack::steal_top(memory_block_stack& other) noexcept
{
    WPI_MEMORY_ASSERT(other.head_);
    auto to_steal = other.head_;
    other.head_   = other.head_->prev;

    to_steal->prev = head_;
    head_          = to_steal;
}

bool memory_block_stack::owns(const void* ptr) const noexcept
{
    auto address = static_cast<const char*>(ptr);
    for (auto cur = head_; cur; cur = cur->prev)
    {
        auto mem = static_cast<char*>(static_cast<void*>(cur));
        if (address >= mem && address < mem + cur->usable_size)
            return true;
    }
    return false;
}

std::size_t memory_block_stack::size() const noexcept
{
    std::size_t res = 0u;
    for (auto cur = head_; cur; cur = cur->prev)
        ++res;
    return res;
}

#if WPI_MEMORY_EXTERN_TEMPLATE
template class wpi::memory::memory_arena<static_block_allocator, true>;
template class wpi::memory::memory_arena<static_block_allocator, false>;
template class wpi::memory::memory_arena<virtual_block_allocator, true>;
template class wpi::memory::memory_arena<virtual_block_allocator, false>;

template class wpi::memory::growing_block_allocator<>;
template class wpi::memory::memory_arena<growing_block_allocator<>, true>;
template class wpi::memory::memory_arena<growing_block_allocator<>, false>;

template class wpi::memory::fixed_block_allocator<>;
template class wpi::memory::memory_arena<fixed_block_allocator<>, true>;
template class wpi::memory::memory_arena<fixed_block_allocator<>, false>;
#endif
