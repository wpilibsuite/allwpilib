// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

#include "wpi/memory/detail/free_list_array.hpp"

#include "wpi/memory/detail/assert.hpp"
#include "wpi/memory/detail/ilog2.hpp"

using namespace wpi::memory;
using namespace detail;

std::size_t log2_access_policy::index_from_size(std::size_t size) noexcept
{
    WPI_MEMORY_ASSERT_MSG(size, "size must not be zero");
    return ilog2_ceil(size);
}

std::size_t log2_access_policy::size_from_index(std::size_t index) noexcept
{
    return std::size_t(1) << index;
}
