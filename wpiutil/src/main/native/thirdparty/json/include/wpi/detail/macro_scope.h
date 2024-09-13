//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.2
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2022 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

#include <utility> // declval, pair
#include <wpi/detail/meta/detected.h>
#include <wpi/thirdparty/hedley/hedley.h>

// This file contains all internal macro definitions (except those affecting ABI)
// You MUST include macro_unscope.hpp at the end of json.hpp to undef all of them

#include <wpi/detail/abi_macros.h>

// exclude unsupported compilers
#if !defined(JSON_SKIP_UNSUPPORTED_COMPILER_CHECK)
    #if defined(__clang__)
        #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
            #error "unsupported Clang version - see https://github.com/nlohmann/json#supported-compilers"
        #endif
    #elif defined(__GNUC__) && !(defined(__ICC) || defined(__INTEL_COMPILER))
        #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40800
            #error "unsupported GCC version - see https://github.com/nlohmann/json#supported-compilers"
        #endif
    #endif
#endif

// C++ language standard detection
// if the user manually specified the used c++ version this is skipped
#if !defined(JSON_HAS_CPP_20) && !defined(JSON_HAS_CPP_17) && !defined(JSON_HAS_CPP_14) && !defined(JSON_HAS_CPP_11)
    #if (defined(__cplusplus) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
        #define JSON_HAS_CPP_20
        #define JSON_HAS_CPP_17
        #define JSON_HAS_CPP_14
    #elif (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
        #define JSON_HAS_CPP_17
        #define JSON_HAS_CPP_14
    #elif (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_HAS_CXX14) && _HAS_CXX14 == 1)
        #define JSON_HAS_CPP_14
    #endif
    // the cpp 11 flag is always specified because it is the minimal required version
    #define JSON_HAS_CPP_11
#endif

#ifdef __has_include
    #if __has_include(<version>)
        #include <version>
    #endif
#endif

#if !defined(JSON_HAS_FILESYSTEM) && !defined(JSON_HAS_EXPERIMENTAL_FILESYSTEM)
    #ifdef JSON_HAS_CPP_17
        #if defined(__cpp_lib_filesystem)
            #define JSON_HAS_FILESYSTEM 1
        #elif defined(__cpp_lib_experimental_filesystem)
            #define JSON_HAS_EXPERIMENTAL_FILESYSTEM 1
        #elif !defined(__has_include)
            #define JSON_HAS_EXPERIMENTAL_FILESYSTEM 1
        #elif __has_include(<filesystem>)
            #define JSON_HAS_FILESYSTEM 1
        #elif __has_include(<experimental/filesystem>)
            #define JSON_HAS_EXPERIMENTAL_FILESYSTEM 1
        #endif

        // std::filesystem does not work on MinGW GCC 8: https://sourceforge.net/p/mingw-w64/bugs/737/
        #if defined(__MINGW32__) && defined(__GNUC__) && __GNUC__ == 8
            #undef JSON_HAS_FILESYSTEM
            #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
        #endif

        // no filesystem support before GCC 8: https://en.cppreference.com/w/cpp/compiler_support
        #if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 8
            #undef JSON_HAS_FILESYSTEM
            #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
        #endif

        // no filesystem support before Clang 7: https://en.cppreference.com/w/cpp/compiler_support
        #if defined(__clang_major__) && __clang_major__ < 7
            #undef JSON_HAS_FILESYSTEM
            #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
        #endif

        // no filesystem support before MSVC 19.14: https://en.cppreference.com/w/cpp/compiler_support
        #if defined(_MSC_VER) && _MSC_VER < 1914
            #undef JSON_HAS_FILESYSTEM
            #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
        #endif

        // no filesystem support before iOS 13
        #if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED < 130000
            #undef JSON_HAS_FILESYSTEM
            #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
        #endif

        // no filesystem support before macOS Catalina
        #if defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && __MAC_OS_X_VERSION_MIN_REQUIRED < 101500
            #undef JSON_HAS_FILESYSTEM
            #undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
        #endif
    #endif
#endif

#ifndef JSON_HAS_EXPERIMENTAL_FILESYSTEM
    #define JSON_HAS_EXPERIMENTAL_FILESYSTEM 0
#endif

#ifndef JSON_HAS_FILESYSTEM
    #define JSON_HAS_FILESYSTEM 0
#endif

#ifndef JSON_HAS_THREE_WAY_COMPARISON
    #if defined(__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907L \
        && defined(__cpp_lib_three_way_comparison) && __cpp_lib_three_way_comparison >= 201907L
        #define JSON_HAS_THREE_WAY_COMPARISON 1
    #else
        #define JSON_HAS_THREE_WAY_COMPARISON 0
    #endif
#endif

#ifndef JSON_HAS_RANGES
    // ranges header shipping in GCC 11.1.0 (released 2021-04-27) has syntax error
    #if defined(__GLIBCXX__) && __GLIBCXX__ == 20210427
        #define JSON_HAS_RANGES 0
    #elif defined(__cpp_lib_ranges)
        #define JSON_HAS_RANGES 1
    #else
        #define JSON_HAS_RANGES 0
    #endif
#endif

#ifdef JSON_HAS_CPP_17
    #define JSON_INLINE_VARIABLE inline
#else
    #define JSON_INLINE_VARIABLE
#endif

#if JSON_HEDLEY_HAS_ATTRIBUTE(no_unique_address)
    #define JSON_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
    #define JSON_NO_UNIQUE_ADDRESS
#endif

// disable documentation warnings on clang
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdocumentation"
    #pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif

// allow disabling exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(JSON_NOEXCEPTION)
    #define JSON_THROW(exception) throw exception
    #define JSON_TRY try
    #define JSON_CATCH(exception) catch(exception)
    #define JSON_INTERNAL_CATCH(exception) catch(exception)
#else
    #include <cstdlib>
    #define JSON_THROW(exception) std::abort()
    #define JSON_TRY if(true)
    #define JSON_CATCH(exception) if(false)
    #define JSON_INTERNAL_CATCH(exception) if(false)
#endif

// override exception macros
#if defined(JSON_THROW_USER)
    #undef JSON_THROW
    #define JSON_THROW JSON_THROW_USER
#endif
#if defined(JSON_TRY_USER)
    #undef JSON_TRY
    #define JSON_TRY JSON_TRY_USER
#endif
#if defined(JSON_CATCH_USER)
    #undef JSON_CATCH
    #define JSON_CATCH JSON_CATCH_USER
    #undef JSON_INTERNAL_CATCH
    #define JSON_INTERNAL_CATCH JSON_CATCH_USER
#endif
#if defined(JSON_INTERNAL_CATCH_USER)
    #undef JSON_INTERNAL_CATCH
    #define JSON_INTERNAL_CATCH JSON_INTERNAL_CATCH_USER
#endif

// allow overriding assert
#if !defined(JSON_ASSERT)
    #include <cassert> // assert
    #define JSON_ASSERT(x) assert(x)
#endif

// allow to access some private functions (needed by the test suite)
#if defined(JSON_TESTS_PRIVATE)
    #define JSON_PRIVATE_UNLESS_TESTED public
#else
    #define JSON_PRIVATE_UNLESS_TESTED private
#endif

/*!
@brief macro to briefly define a mapping between an enum and JSON
@def WPI_JSON_SERIALIZE_ENUM
@since version 3.4.0
*/
#define WPI_JSON_SERIALIZE_ENUM(ENUM_TYPE, ...)                                            \
    template<typename BasicJsonType>                                                            \
    inline void to_json(BasicJsonType& j, const ENUM_TYPE& e)                                   \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [e](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool  \
        {                                                                                       \
            return ej_pair.first == e;                                                          \
        });                                                                                     \
        j = ((it != std::end(m)) ? it : std::begin(m))->second;                                 \
    }                                                                                           \
    template<typename BasicJsonType>                                                            \
    inline void from_json(const BasicJsonType& j, ENUM_TYPE& e)                                 \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [&j](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool \
        {                                                                                       \
            return ej_pair.second == j;                                                         \
        });                                                                                     \
        e = ((it != std::end(m)) ? it : std::begin(m))->first;                                  \
    }

// Ugly macros to avoid uglier copy-paste when specializing basic_json. They
// may be removed in the future once the class is split.

#define WPI_BASIC_JSON_TPL_DECLARATION                                \
    template<template<typename, typename, typename...> class ObjectType,   \
             template<typename, typename...> class ArrayType,              \
             class StringType, class BooleanType, class NumberIntegerType, \
             class NumberUnsignedType, class NumberFloatType,              \
             template<typename> class AllocatorType,                       \
             template<typename, typename = void> class JSONSerializer,     \
             class BinaryType>

#define WPI_BASIC_JSON_TPL                                            \
    basic_json<ObjectType, ArrayType, StringType, BooleanType,             \
    NumberIntegerType, NumberUnsignedType, NumberFloatType,                \
    AllocatorType, JSONSerializer, BinaryType>

// Macros to simplify conversion from/to types

#define WPI_JSON_EXPAND( x ) x
#define WPI_JSON_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, NAME,...) NAME
#define WPI_JSON_PASTE(...) WPI_JSON_EXPAND(WPI_JSON_GET_MACRO(__VA_ARGS__, \
        WPI_JSON_PASTE64, \
        WPI_JSON_PASTE63, \
        WPI_JSON_PASTE62, \
        WPI_JSON_PASTE61, \
        WPI_JSON_PASTE60, \
        WPI_JSON_PASTE59, \
        WPI_JSON_PASTE58, \
        WPI_JSON_PASTE57, \
        WPI_JSON_PASTE56, \
        WPI_JSON_PASTE55, \
        WPI_JSON_PASTE54, \
        WPI_JSON_PASTE53, \
        WPI_JSON_PASTE52, \
        WPI_JSON_PASTE51, \
        WPI_JSON_PASTE50, \
        WPI_JSON_PASTE49, \
        WPI_JSON_PASTE48, \
        WPI_JSON_PASTE47, \
        WPI_JSON_PASTE46, \
        WPI_JSON_PASTE45, \
        WPI_JSON_PASTE44, \
        WPI_JSON_PASTE43, \
        WPI_JSON_PASTE42, \
        WPI_JSON_PASTE41, \
        WPI_JSON_PASTE40, \
        WPI_JSON_PASTE39, \
        WPI_JSON_PASTE38, \
        WPI_JSON_PASTE37, \
        WPI_JSON_PASTE36, \
        WPI_JSON_PASTE35, \
        WPI_JSON_PASTE34, \
        WPI_JSON_PASTE33, \
        WPI_JSON_PASTE32, \
        WPI_JSON_PASTE31, \
        WPI_JSON_PASTE30, \
        WPI_JSON_PASTE29, \
        WPI_JSON_PASTE28, \
        WPI_JSON_PASTE27, \
        WPI_JSON_PASTE26, \
        WPI_JSON_PASTE25, \
        WPI_JSON_PASTE24, \
        WPI_JSON_PASTE23, \
        WPI_JSON_PASTE22, \
        WPI_JSON_PASTE21, \
        WPI_JSON_PASTE20, \
        WPI_JSON_PASTE19, \
        WPI_JSON_PASTE18, \
        WPI_JSON_PASTE17, \
        WPI_JSON_PASTE16, \
        WPI_JSON_PASTE15, \
        WPI_JSON_PASTE14, \
        WPI_JSON_PASTE13, \
        WPI_JSON_PASTE12, \
        WPI_JSON_PASTE11, \
        WPI_JSON_PASTE10, \
        WPI_JSON_PASTE9, \
        WPI_JSON_PASTE8, \
        WPI_JSON_PASTE7, \
        WPI_JSON_PASTE6, \
        WPI_JSON_PASTE5, \
        WPI_JSON_PASTE4, \
        WPI_JSON_PASTE3, \
        WPI_JSON_PASTE2, \
        WPI_JSON_PASTE1)(__VA_ARGS__))
#define WPI_JSON_PASTE2(func, v1) func(v1)
#define WPI_JSON_PASTE3(func, v1, v2) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE2(func, v2)
#define WPI_JSON_PASTE4(func, v1, v2, v3) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE3(func, v2, v3)
#define WPI_JSON_PASTE5(func, v1, v2, v3, v4) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE4(func, v2, v3, v4)
#define WPI_JSON_PASTE6(func, v1, v2, v3, v4, v5) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE5(func, v2, v3, v4, v5)
#define WPI_JSON_PASTE7(func, v1, v2, v3, v4, v5, v6) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE6(func, v2, v3, v4, v5, v6)
#define WPI_JSON_PASTE8(func, v1, v2, v3, v4, v5, v6, v7) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE7(func, v2, v3, v4, v5, v6, v7)
#define WPI_JSON_PASTE9(func, v1, v2, v3, v4, v5, v6, v7, v8) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE8(func, v2, v3, v4, v5, v6, v7, v8)
#define WPI_JSON_PASTE10(func, v1, v2, v3, v4, v5, v6, v7, v8, v9) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE9(func, v2, v3, v4, v5, v6, v7, v8, v9)
#define WPI_JSON_PASTE11(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE10(func, v2, v3, v4, v5, v6, v7, v8, v9, v10)
#define WPI_JSON_PASTE12(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE11(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define WPI_JSON_PASTE13(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE12(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)
#define WPI_JSON_PASTE14(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE13(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
#define WPI_JSON_PASTE15(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE14(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)
#define WPI_JSON_PASTE16(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE15(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)
#define WPI_JSON_PASTE17(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE16(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)
#define WPI_JSON_PASTE18(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE17(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)
#define WPI_JSON_PASTE19(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE18(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)
#define WPI_JSON_PASTE20(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE19(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)
#define WPI_JSON_PASTE21(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE20(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)
#define WPI_JSON_PASTE22(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE21(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21)
#define WPI_JSON_PASTE23(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE22(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22)
#define WPI_JSON_PASTE24(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE23(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23)
#define WPI_JSON_PASTE25(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE24(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24)
#define WPI_JSON_PASTE26(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE25(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25)
#define WPI_JSON_PASTE27(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE26(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26)
#define WPI_JSON_PASTE28(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE27(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27)
#define WPI_JSON_PASTE29(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE28(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28)
#define WPI_JSON_PASTE30(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE29(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29)
#define WPI_JSON_PASTE31(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE30(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30)
#define WPI_JSON_PASTE32(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE31(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31)
#define WPI_JSON_PASTE33(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE32(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32)
#define WPI_JSON_PASTE34(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE33(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33)
#define WPI_JSON_PASTE35(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE34(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34)
#define WPI_JSON_PASTE36(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE35(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35)
#define WPI_JSON_PASTE37(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE36(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36)
#define WPI_JSON_PASTE38(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE37(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37)
#define WPI_JSON_PASTE39(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE38(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38)
#define WPI_JSON_PASTE40(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE39(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39)
#define WPI_JSON_PASTE41(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE40(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40)
#define WPI_JSON_PASTE42(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE41(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41)
#define WPI_JSON_PASTE43(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE42(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42)
#define WPI_JSON_PASTE44(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE43(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43)
#define WPI_JSON_PASTE45(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE44(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44)
#define WPI_JSON_PASTE46(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE45(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45)
#define WPI_JSON_PASTE47(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE46(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46)
#define WPI_JSON_PASTE48(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE47(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47)
#define WPI_JSON_PASTE49(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE48(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48)
#define WPI_JSON_PASTE50(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE49(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49)
#define WPI_JSON_PASTE51(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE50(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50)
#define WPI_JSON_PASTE52(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE51(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51)
#define WPI_JSON_PASTE53(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE52(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52)
#define WPI_JSON_PASTE54(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE53(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53)
#define WPI_JSON_PASTE55(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE54(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54)
#define WPI_JSON_PASTE56(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE55(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55)
#define WPI_JSON_PASTE57(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE56(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56)
#define WPI_JSON_PASTE58(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE57(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57)
#define WPI_JSON_PASTE59(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE58(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58)
#define WPI_JSON_PASTE60(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE59(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59)
#define WPI_JSON_PASTE61(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE60(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60)
#define WPI_JSON_PASTE62(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE61(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61)
#define WPI_JSON_PASTE63(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE62(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62)
#define WPI_JSON_PASTE64(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63) WPI_JSON_PASTE2(func, v1) WPI_JSON_PASTE63(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63)

#define WPI_JSON_TO(v1) nlohmann_json_j[#v1] = nlohmann_json_t.v1;
#define WPI_JSON_FROM(v1) nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1);
#define WPI_JSON_FROM_WITH_DEFAULT(v1) nlohmann_json_t.v1 = nlohmann_json_j.value(#v1, nlohmann_json_default_obj.v1);

/*!
@brief macro
@def WPI_DEFINE_TYPE_INTRUSIVE
@since version 3.9.0
*/
#define WPI_DEFINE_TYPE_INTRUSIVE(Type, ...)  \
    friend void to_json(wpi::json& nlohmann_json_j, const Type& nlohmann_json_t) { WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_TO, __VA_ARGS__)) } \
    friend void from_json(const wpi::json& nlohmann_json_j, Type& nlohmann_json_t) { WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_FROM, __VA_ARGS__)) }

#define WPI_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Type, ...)  \
    friend void to_json(wpi::json& nlohmann_json_j, const Type& nlohmann_json_t) { WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_TO, __VA_ARGS__)) } \
    friend void from_json(const wpi::json& nlohmann_json_j, Type& nlohmann_json_t) { Type nlohmann_json_default_obj; WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) }

/*!
@brief macro
@def WPI_DEFINE_TYPE_NON_INTRUSIVE
@since version 3.9.0
*/
#define WPI_DEFINE_TYPE_NON_INTRUSIVE(Type, ...)  \
    inline void to_json(wpi::json& nlohmann_json_j, const Type& nlohmann_json_t) { WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const wpi::json& nlohmann_json_j, Type& nlohmann_json_t) { WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_FROM, __VA_ARGS__)) }

#define WPI_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Type, ...)  \
    inline void to_json(wpi::json& nlohmann_json_j, const Type& nlohmann_json_t) { WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const wpi::json& nlohmann_json_j, Type& nlohmann_json_t) { Type nlohmann_json_default_obj; WPI_JSON_EXPAND(WPI_JSON_PASTE(WPI_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) }


// inspired from https://stackoverflow.com/a/26745591
// allows to call any std function as if (e.g. with begin):
// using std::begin; begin(x);
//
// it allows using the detected idiom to retrieve the return type
// of such an expression
#define WPI_CAN_CALL_STD_FUNC_IMPL(std_name)                                 \
    namespace detail {                                                            \
    using std::std_name;                                                          \
    \
    template<typename... T>                                                       \
    using result_of_##std_name = decltype(std_name(std::declval<T>()...));        \
    }                                                                             \
    \
    namespace detail2 {                                                           \
    struct std_name##_tag                                                         \
    {                                                                             \
    };                                                                            \
    \
    template<typename... T>                                                       \
    std_name##_tag std_name(T&&...);                                              \
    \
    template<typename... T>                                                       \
    using result_of_##std_name = decltype(std_name(std::declval<T>()...));        \
    \
    template<typename... T>                                                       \
    struct would_call_std_##std_name                                              \
    {                                                                             \
        static constexpr auto const value = ::wpi::detail::                  \
                                            is_detected_exact<std_name##_tag, result_of_##std_name, T...>::value; \
    };                                                                            \
    } /* namespace detail2 */ \
    \
    template<typename... T>                                                       \
    struct would_call_std_##std_name : detail2::would_call_std_##std_name<T...>   \
    {                                                                             \
    }

#ifndef JSON_USE_IMPLICIT_CONVERSIONS
    #define JSON_USE_IMPLICIT_CONVERSIONS 1
#endif

#if JSON_USE_IMPLICIT_CONVERSIONS
    #define JSON_EXPLICIT
#else
    #define JSON_EXPLICIT explicit
#endif

#ifndef JSON_DISABLE_ENUM_SERIALIZATION
    #define JSON_DISABLE_ENUM_SERIALIZATION 0
#endif

#ifndef JSON_USE_GLOBAL_UDLS
    #define JSON_USE_GLOBAL_UDLS 1
#endif
