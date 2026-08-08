// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>
#include <string>
#include <type_traits>
#include <vector>

#include "wpi/tunable/detail/TunableTypeValue.hpp"

namespace wpi::detail {

template <typename T>
struct always_false : std::false_type {};

struct TunableStructTag {};
struct TunableProtobufTag {};

template <typename T, bool Member = false>
constexpr TunableTypeValue GetTunableTypeValue() {
  if constexpr (std::same_as<T, bool>) {
    return Member ? TunableTypeValue::MEMBER_BOOLEAN
                  : TunableTypeValue::BOOLEAN;
  } else if constexpr (std::same_as<T, int32_t>) {
    return Member ? TunableTypeValue::MEMBER_INT32 : TunableTypeValue::INT32;
  } else if constexpr (std::same_as<T, int64_t>) {
    return Member ? TunableTypeValue::MEMBER_INT64 : TunableTypeValue::INT64;
  } else if constexpr (std::same_as<T, float>) {
    return Member ? TunableTypeValue::MEMBER_FLOAT : TunableTypeValue::FLOAT;
  } else if constexpr (std::same_as<T, double>) {
    return Member ? TunableTypeValue::MEMBER_DOUBLE : TunableTypeValue::DOUBLE;
  } else if constexpr (std::same_as<T, std::string>) {
    return Member ? TunableTypeValue::MEMBER_STRING : TunableTypeValue::STRING;
  } else if constexpr (std::same_as<T, std::vector<uint8_t>>) {
    return Member ? TunableTypeValue::MEMBER_RAW : TunableTypeValue::RAW;
  } else if constexpr (std::same_as<T, std::vector<bool>>) {
    return Member ? TunableTypeValue::MEMBER_BOOLEAN_ARRAY
                  : TunableTypeValue::BOOLEAN_ARRAY;
  } else if constexpr (std::same_as<T, std::vector<int32_t>>) {
    return Member ? TunableTypeValue::MEMBER_INT32_ARRAY
                  : TunableTypeValue::INT32_ARRAY;
  } else if constexpr (std::same_as<T, std::vector<int64_t>>) {
    return Member ? TunableTypeValue::MEMBER_INT64_ARRAY
                  : TunableTypeValue::INT64_ARRAY;
  } else if constexpr (std::same_as<T, std::vector<float>>) {
    return Member ? TunableTypeValue::MEMBER_FLOAT_ARRAY
                  : TunableTypeValue::FLOAT_ARRAY;
  } else if constexpr (std::same_as<T, std::vector<double>>) {
    return Member ? TunableTypeValue::MEMBER_DOUBLE_ARRAY
                  : TunableTypeValue::DOUBLE_ARRAY;
  } else if constexpr (std::same_as<T, std::vector<std::string>>) {
    return Member ? TunableTypeValue::MEMBER_STRING_ARRAY
                  : TunableTypeValue::STRING_ARRAY;
  } else {
    static_assert(always_false<T>::value, "Don't know how to serialize type");
  }
}

template <typename T>
concept TunableValueType =
    std::same_as<T, bool> || std::same_as<T, int32_t> ||
    std::same_as<T, int64_t> || std::same_as<T, float> ||
    std::same_as<T, double> || std::same_as<T, std::string> ||
    std::same_as<T, std::vector<uint8_t>> ||
    std::same_as<T, std::vector<bool>> ||
    std::same_as<T, std::vector<int32_t>> ||
    std::same_as<T, std::vector<int64_t>> ||
    std::same_as<T, std::vector<float>> ||
    std::same_as<T, std::vector<double>> ||
    std::same_as<T, std::vector<std::string>>;

}  // namespace wpi::detail
