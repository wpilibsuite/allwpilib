//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.2
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2022 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

// This file contains all macro definitions affecting or depending on the ABI

#ifndef JSON_SKIP_LIBRARY_VERSION_CHECK
    #if defined(WPI_JSON_VERSION_MAJOR) && defined(WPI_JSON_VERSION_MINOR) && defined(WPI_JSON_VERSION_PATCH)
        #if WPI_JSON_VERSION_MAJOR != 3 || WPI_JSON_VERSION_MINOR != 11 || WPI_JSON_VERSION_PATCH != 2
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif

#define WPI_JSON_VERSION_MAJOR 3   // NOLINT(modernize-macro-to-enum)
#define WPI_JSON_VERSION_MINOR 11  // NOLINT(modernize-macro-to-enum)
#define WPI_JSON_VERSION_PATCH 2   // NOLINT(modernize-macro-to-enum)

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

#ifndef WPI_JSON_NAMESPACE
#define WPI_JSON_NAMESPACE               \
    wpi::WPI_JSON_NAMESPACE_CONCAT( \
            WPI_JSON_ABI_TAGS,           \
            WPI_JSON_NAMESPACE_VERSION)
#endif

#ifndef WPI_JSON_NAMESPACE_BEGIN
#define WPI_JSON_NAMESPACE_BEGIN \
    namespace wpi                \
    {
#endif

#ifndef WPI_JSON_NAMESPACE_END
#define WPI_JSON_NAMESPACE_END \
    }  // namespace wpi
#endif
