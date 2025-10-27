//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

#include <wpi/detail/macro_scope.h>

#if JSON_HAS_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
WPI_JSON_NAMESPACE_BEGIN
namespace detail
{
namespace std_fs = std::experimental::filesystem;
}  // namespace detail
WPI_JSON_NAMESPACE_END
#elif JSON_HAS_FILESYSTEM
#include <filesystem>
WPI_JSON_NAMESPACE_BEGIN
namespace detail
{
namespace std_fs = std::filesystem;
}  // namespace detail
WPI_JSON_NAMESPACE_END
#endif
