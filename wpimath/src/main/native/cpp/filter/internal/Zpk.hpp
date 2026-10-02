// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <complex>
#include <vector>

#include "wpi/math/filter/BiquadFilter.hpp"

namespace wpi::math::filter::internal {

using cplx = std::complex<double>;
using Section = wpi::math::BiquadFilter::Section;
using Sections = std::vector<Section>;

/**
 * Zeros/poles/gain representation of a rational transfer function:
 *   H(s) = gain · ∏(s - z_i) / ∏(s - p_j)        (analog)
 *   H(z) = gain · ∏(z - z_i) / ∏(z - p_j)        (digital)
 *
 * Complex roots must appear in conjugate pairs; that invariant is preserved
 * by every transform below.
 */
struct Zpk {
  std::vector<cplx> zeros;
  std::vector<cplx> poles;
  double gain = 1.0;
};

/** Analog LP→LP transform: cutoff 1 rad/s → cutoff @a wo rad/s. */
Zpk AnalogLpToLp(const Zpk& p, double wo);

/** Analog LP→HP transform: LP cutoff 1 → HP cutoff @a wo rad/s. */
Zpk AnalogLpToHp(const Zpk& p, double wo);

/**
 * Analog LP→BP transform centered at @a wo rad/s with bandwidth @a bw rad/s.
 * Each prototype pole becomes two; each prototype zero becomes two; plus
 * @c degree zeros at the origin.
 */
Zpk AnalogLpToBp(const Zpk& p, double wo, double bw);

/**
 * Analog LP→BS transform centered at @a wo rad/s with bandwidth @a bw rad/s.
 * Same fan-out as LpToBp; the added zeros go to ±j·wo instead of the origin.
 */
Zpk AnalogLpToBs(const Zpk& p, double wo, double bw);

/**
 * Pair conjugate poles (and zeros) into biquad sections. Sections are
 * ordered by ascending |pole| (innermost / least-aggressive first); the
 * overall gain is folded into the first section's numerator.
 */
Sections ZpkToSos(const Zpk& digital);

}  // namespace wpi::math::filter::internal
