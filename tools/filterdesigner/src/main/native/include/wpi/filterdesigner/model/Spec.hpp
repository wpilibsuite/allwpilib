// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/** Schema major version emitted by @ref SerializeSpec. */
constexpr int kSpecVersion = 1;

/** A loaded filter spec — ready to swap into @c FilterDesignView. */
struct LoadedSpec {
  double sampleRate = 1000.0;
  std::vector<Stage> stages;
};

/**
 * Serializes a sample rate + stage list to a pretty-printed JSON string with
 * the @c kSpecVersion schema.
 */
std::string SerializeSpec(double sampleRate, std::span<const Stage> stages);

/** Result of @ref ParseSpec — either a loaded spec or a human-readable error.
 */
struct ParseResult {
  std::optional<LoadedSpec> spec;
  std::string error;
};

/**
 * Parses a JSON spec string. Missing optional per-stage fields fall back to
 * the @c Stage struct's defaults; unknown fields are ignored. Rejects:
 *   - malformed JSON,
 *   - missing or non-numeric top-level @c "sampleRate",
 *   - missing top-level @c "stages" array,
 *   - unknown @c "kind" or @c "family" strings,
 *   - LP/HP/BP/BS stages without a @c "family",
 *   - integer fields (@c order, @c taps) with non-integer values,
 *   - schema versions newer than @c kSpecVersion.
 */
ParseResult ParseSpec(std::string_view json);

/**
 * Serializes a spec and writes it to @a path. Overwrites if the file exists.
 *
 * @return Empty string on success, otherwise a human-readable error message
 *         (file could not be opened, write failed, etc.).
 */
std::string SaveSpecToFile(const std::string& path, double sampleRate,
                           std::span<const Stage> stages);

/**
 * Reads @a path and parses it as a JSON spec. Returns the same result shape
 * as @ref ParseSpec; an unreadable file produces a populated @c error.
 */
ParseResult LoadSpecFromFile(const std::string& path);

}  // namespace wpi::filterdesigner
