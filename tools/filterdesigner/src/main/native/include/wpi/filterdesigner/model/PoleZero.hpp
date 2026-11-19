// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <complex>
#include <vector>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * Poles and zeros of a biquad cascade in the z-plane.
 *
 * Each second-order section with numerator @c b0 z^2 + b1 z + b2 contributes
 * up to two zeros (roots of that polynomial); its denominator
 * @c z^2 + a1 z + a2 contributes up to two poles. "Degenerate" sections (e.g.
 * a 1st-order filter stored as a biquad with @c b2 = a2 = 0) yield fewer
 * roots, so the returned counts are not necessarily @c 2 * sections.size().
 */
struct PoleZeroPlot {
  std::vector<std::complex<double>> poles;
  std::vector<std::complex<double>> zeros;
};

/**
 * Extracts poles and zeros from a cascade of biquad sections. Roots from
 * different sections are concatenated preserving multiplicity, so a cascade
 * with repeated poles/zeros will have duplicates in the result.
 */
PoleZeroPlot ComputePolesZeros(const Sections& sections);

}  // namespace wpi::filterdesigner
