// Copyright (C) 2015-2021 Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "wpi/memory/iteration_allocator.hpp"

using namespace wpi::memory;

#if WPI_MEMORY_EXTERN_TEMPLATE
template class wpi::memory::iteration_allocator<2>;
template class wpi::memory::allocator_traits<iteration_allocator<2>>;
template class wpi::memory::composable_allocator_traits<iteration_allocator<2>>;
#endif
