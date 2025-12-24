//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

// This file contains all macro definitions affecting or depending on the ABI

#ifndef JSON_SKIP_LIBRARY_VERSION_CHECK
    #if defined(WPI_JSON_VERSION_MAJOR) && defined(WPI_JSON_VERSION_MINOR) && defined(WPI_JSON_VERSION_PATCH)
        #if WPI_JSON_VERSION_MAJOR != 3 || WPI_JSON_VERSION_MINOR != 11 || WPI_JSON_VERSION_PATCH != 3
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif

#define WPI_JSON_VERSION_MAJOR 3   // NOLINT(modernize-macro-to-enum)
#define WPI_JSON_VERSION_MINOR 11  // NOLINT(modernize-macro-to-enum)
#define WPI_JSON_VERSION_PATCH 3   // NOLINT(modernize-macro-to-enum)

#ifndef JSON_DIAGNOSTICS
    #define JSON_DIAGNOSTICS 0
#endif

#ifndef JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
    #define JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON 0
#endif

#if JSON_DIAGNOSTICS
    #define WPI_JSON_ABI_TAG_DIAGNOSTICS _diag
#else
    #define WPI_JSON_ABI_TAG_DIAGNOSTICS
#endif

#if JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
    #define WPI_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON _ldvcmp
#else
    #define WPI_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON
#endif

#ifndef WPI_JSON_NAMESPACE_NO_VERSION
    #define WPI_JSON_NAMESPACE_NO_VERSION 0
#endif

// Construct the namespace ABI tags component
#define WPI_JSON_ABI_TAGS_CONCAT_EX(a, b) json_abi ## a ## b
#define WPI_JSON_ABI_TAGS_CONCAT(a, b) \
    WPI_JSON_ABI_TAGS_CONCAT_EX(a, b)

#define WPI_JSON_ABI_TAGS                                       \
    WPI_JSON_ABI_TAGS_CONCAT(                                   \
            WPI_JSON_ABI_TAG_DIAGNOSTICS,                       \
            WPI_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON)

// Construct the namespace version component
#define WPI_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch) \
    _v ## major ## _ ## minor ## _ ## patch
#define WPI_JSON_NAMESPACE_VERSION_CONCAT(major, minor, patch) \
    WPI_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch)

#if WPI_JSON_NAMESPACE_NO_VERSION
#define WPI_JSON_NAMESPACE_VERSION
#else
#define WPI_JSON_NAMESPACE_VERSION                                 \
    WPI_JSON_NAMESPACE_VERSION_CONCAT(WPI_JSON_VERSION_MAJOR, \
                                           WPI_JSON_VERSION_MINOR, \
                                           WPI_JSON_VERSION_PATCH)
#endif

// Combine namespace components
#define WPI_JSON_NAMESPACE_CONCAT_EX(a, b) a ## b
#define WPI_JSON_NAMESPACE_CONCAT(a, b) \
    WPI_JSON_NAMESPACE_CONCAT_EX(a, b)

#ifndef WPI_JSON_NAMESPACE
#define WPI_JSON_NAMESPACE               \
    wpi::util::WPI_JSON_NAMESPACE_CONCAT( \
            WPI_JSON_ABI_TAGS,           \
            WPI_JSON_NAMESPACE_VERSION)
#endif

#ifndef WPI_JSON_NAMESPACE_BEGIN
#define WPI_JSON_NAMESPACE_BEGIN                \
    namespace wpi::util                               \
    {                                                \
    inline namespace WPI_JSON_NAMESPACE_CONCAT( \
                WPI_JSON_ABI_TAGS,              \
                WPI_JSON_NAMESPACE_VERSION)     \
    {
#endif

#ifndef WPI_JSON_NAMESPACE_END
#define WPI_JSON_NAMESPACE_END                                     \
    }  /* namespace (inline namespace) NOLINT(readability/namespace) */ \
    }  // namespace wpi::util
#endif
