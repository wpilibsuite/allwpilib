// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <expected>
#include <optional>
#include <string>
#include <string_view>

#include "wpi/util/json.hpp"

namespace wpi::nt::server {

/**
 * Decode exactly one MessagePack object into the JSON data model.
 * Binary values, non-string map keys, duplicate keys, nonfinite numbers, and
 * extensions cannot be translated and are rejected. Nesting and work are
 * bounded.
 *
 * @param data MessagePack bytes
 * @return decoded value, or an error description
 */
std::expected<wpi::util::json, std::string_view> RestDecodeMessagePack(
    std::string_view data);

/**
 * Validate exactly one bounded MessagePack object without translating its data.
 * Binary values and arbitrary map keys are accepted; extensions are
 * unsupported.
 *
 * @param data MessagePack bytes
 * @return true if valid
 */
bool RestValidateMessagePack(std::string_view data);

/**
 * Encode a JSON value as MessagePack, preserving numeric types.
 *
 * @param value value to encode
 * @return encoded bytes, or nullopt if nesting or node count exceeds the codec
 * limits
 */
std::optional<std::string> RestEncodeMessagePack(const wpi::util::json& value);

}  // namespace wpi::nt::server
