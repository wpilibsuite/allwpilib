//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>

#include <wpi/detail/abi_macros.h>
#include <wpi/detail/conversions/from_json.h>
#include <wpi/detail/conversions/to_json.h>
#include <wpi/detail/meta/identity_tag.h>

WPI_JSON_NAMESPACE_BEGIN

/// @sa https://json.nlohmann.me/api/adl_serializer/
template<typename ValueType, typename>
struct adl_serializer
{
    /// @brief convert a JSON value to any value type
    /// @sa https://json.nlohmann.me/api/adl_serializer/from_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType && j, TargetType& val) noexcept(
        noexcept(::wpi::from_json(std::forward<BasicJsonType>(j), val)))
    -> decltype(::wpi::from_json(std::forward<BasicJsonType>(j), val), void())
    {
        ::wpi::from_json(std::forward<BasicJsonType>(j), val);
    }

    /// @brief convert a JSON value to any value type
    /// @sa https://json.nlohmann.me/api/adl_serializer/from_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType && j) noexcept(
    noexcept(::wpi::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {})))
    -> decltype(::wpi::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {}))
    {
        return ::wpi::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {});
    }

    /// @brief convert any value type to a JSON value
    /// @sa https://json.nlohmann.me/api/adl_serializer/to_json/
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto to_json(BasicJsonType& j, TargetType && val) noexcept(
        noexcept(::wpi::to_json(j, std::forward<TargetType>(val))))
    -> decltype(::wpi::to_json(j, std::forward<TargetType>(val)), void())
    {
        ::wpi::to_json(j, std::forward<TargetType>(val));
    }
};

WPI_JSON_NAMESPACE_END
