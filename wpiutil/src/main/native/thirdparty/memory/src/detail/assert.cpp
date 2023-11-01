// Copyright (C) 2015-2023 Jonathan Müller and foonathan/memory contributors
// SPDX-License-Identifier: Zlib

#include "wpi/memory/detail/assert.hpp"

#if WPI_HOSTED_IMPLEMENTATION
#include <cstdio>
#endif

#include <cstdlib>

#include "wpi/memory/error.hpp"

using namespace wpi::memory;
using namespace detail;

void detail::handle_failed_assert(const char* msg, const char* file, int line,
                                  const char* fnc) noexcept
{
#if WPI_HOSTED_IMPLEMENTATION
    std::fprintf(stderr, "[%s] Assertion failure in function %s (%s:%d): %s.\n",
                 WPI_MEMORY_LOG_PREFIX, fnc, file, line, msg);
#endif
    std::abort();
}

void detail::handle_warning(const char* msg, const char* file, int line, const char* fnc) noexcept
{
#if WPI_HOSTED_IMPLEMENTATION
    std::fprintf(stderr, "[%s] Warning triggered in function %s (%s:%d): %s.\n",
                 WPI_MEMORY_LOG_PREFIX, fnc, file, line, msg);
#endif
}
