// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Zpk.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"

namespace wpi::math::filter::internal {

/**
 * Pre-warp a digital cutoff frequency (Hz) for use as the analog-domain
 * cutoff that, after the bilinear transform at the same @a fs, maps back to
 * exactly that digital cutoff.
 */
double PreWarp(double fc, double fs);

/**
 * Bilinear transform of an analog ZPK to a digital ZPK at sample rate @a fs.
 * Analog zeros at infinity map to digital zeros at z = -1 (Nyquist).
 */
Zpk BilinearTransform(const Zpk& analog, double fs);

/**
 * Apply the kind-specific frequency transform (LP/HP/BP/BS) to an analog LP
 * prototype, run the bilinear transform at @a fs, and convert to a SOS
 * cascade. Shared by every classical IIR design factory (Butterworth,
 * Chebyshev I/II, Elliptic).
 *
 * Caller is responsible for validating inputs (positive fs, f1 in (0, fs/2),
 * and for BP/BS, f1 < f2 < fs/2). This helper does no validation itself.
 */
Sections DesignFromAnalogLp(const Zpk& analogLp,
                            wpi::math::BiquadFilter::Kind kind, double fs,
                            double f1, double f2);

}  // namespace wpi::math::filter::internal
