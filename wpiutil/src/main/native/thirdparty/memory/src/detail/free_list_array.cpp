// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

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
