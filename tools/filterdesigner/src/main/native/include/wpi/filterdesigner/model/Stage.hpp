// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include "wpi/math/filter/BiquadFilter.hpp"

namespace wpi::filterdesigner {

using Section = wpi::math::BiquadFilter::Section;
using Sections = std::vector<Section>;

/**
 * Every kind of stage the chain UI can hold. The first four (LP/HP/BP/BS) are
 * "classical" — they pick from the @ref Family enum below. Notch and
 * MovingAverage have a single fixed design and ignore @c Stage::family.
 */
enum class StageKind {
  LowPass,
  HighPass,
  BandPass,
  BandStop,
  Notch,
  MovingAverage
};

/**
 * Classical IIR family. Only meaningful for LP/HP/BP/BS — Notch and
 * MovingAverage ignore this field.
 */
enum class Family { Butterworth, Chebyshev1, Chebyshev2, Elliptic };

/** Per-stage design parameters. Sample rate is shared across all stages. */
struct Stage {
  StageKind kind = StageKind::LowPass;
  Family family = Family::Butterworth;
  int order = 4;
  int taps = 5;
  double f1 = 100.0;
  double f2 = 200.0;
  double q = 10.0;
  double passbandRippleDb = 1.0;  ///< Cheby1 and Elliptic.
  double stopbandAttenDb = 40.0;  ///< Cheby2 and Elliptic.
};

/**
 * @return true if @a kind is a classical family (LP/HP/BP/BS) that reads
 *         @c Stage::family. False for Notch / MovingAverage.
 */
constexpr bool KindUsesFamily(StageKind kind) {
  return kind == StageKind::LowPass || kind == StageKind::HighPass ||
         kind == StageKind::BandPass || kind == StageKind::BandStop;
}

}  // namespace wpi::filterdesigner
