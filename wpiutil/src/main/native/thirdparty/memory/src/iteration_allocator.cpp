// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

#include "wpi/memory/iteration_allocator.hpp"

using namespace wpi::memory;

#if WPI_MEMORY_EXTERN_TEMPLATE
template class wpi::memory::iteration_allocator<2>;
template class wpi::memory::allocator_traits<iteration_allocator<2>>;
template class wpi::memory::composable_allocator_traits<iteration_allocator<2>>;
#endif
