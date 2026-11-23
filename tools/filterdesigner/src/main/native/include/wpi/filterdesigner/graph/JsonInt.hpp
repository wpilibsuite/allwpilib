// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string_view>

#include "wpi/util/json.hpp"

// Range-checked readers for the numeric scalars a design file carries. Every
// one of them refuses a value its target type cannot represent, so a caller
// keeps its default rather than casting undefined behaviour into a field.
namespace wpi::filterdesigner {

/**
 * Reads a JSON value as an int, or nothing if it is not one.
 *
 * Only a plain integer literal that fits in an int qualifies. The parser
 * types anything with a fraction or an exponent as a double, and converting
 * a double outside int's range is undefined, so a design file holding
 * "order": 1e100 or "taps": 2.5 must be refused up front rather than cast
 * and clamped afterwards.
 */
std::optional<int> ReadJsonInt(const wpi::util::json& value);

/**
 * Looks up @a key in @a obj and reads it with ReadJsonInt. Empty when the
 * key is absent or the value is not an in-range integer, so a caller keeps
 * its default in both cases.
 */
std::optional<int> ReadIntField(const wpi::util::json& obj,
                                std::string_view key);

/**
 * Reads a JSON value as a float, or nothing if it is not one.
 *
 * Any JSON number qualifies, but only if it is finite and small enough for
 * a float to hold: narrowing a larger double is undefined and in practice
 * yields an infinity, which then flows into geometry — a node position, a
 * plot size — that nothing downstream checks again. A number too large for
 * the parser's own double arrives as an infinity already, and is refused
 * the same way.
 */
std::optional<float> ReadJsonFloat(const wpi::util::json& value);

/**
 * Looks up @a key in @a obj and reads it with ReadJsonFloat. Empty when the
 * key is absent or the value is not a float the type can hold, so a caller
 * keeps its default in both cases.
 */
std::optional<float> ReadFloatField(const wpi::util::json& obj,
                                    std::string_view key);

}  // namespace wpi::filterdesigner
