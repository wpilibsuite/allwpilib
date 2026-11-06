// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

enum class Language { Cpp, Java, Python };

/**
 * Emits source code that constructs a @c wpi::math::BiquadFilter cascade
 * matching @a sections. Returns an empty string when @a sections is empty.
 *
 * @param sections  Cascade to serialize.
 * @param lang      Target language.
 * @param varName   Identifier for the emitted variable.
 */
std::string EmitCode(const Sections& sections, Language lang,
                     std::string_view varName = "filter");

}  // namespace wpi::filterdesigner
