// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <vector>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * Runs @a samples through a fresh @c BiquadFilter cascade built from
 * @a sections. The filter state starts at zero, so early samples exhibit
 * the usual transient response.
 *
 * @return Filtered samples, same length as @a samples. Empty @a sections
 *         returns @a samples unchanged.
 */
std::vector<double> ApplyFilter(std::span<const double> samples,
                                const Sections& sections);

}  // namespace wpi::filterdesigner
