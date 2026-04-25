// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <complex>
#include <optional>
#include <vector>

#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace wpi::filterdesigner::detail {

using cplx = std::complex<double>;

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

/** Analog Butterworth low-pass prototype, cutoff 1 rad/s. */
Zpk ButterworthPrototype(int order);

/**
 * Analog Chebyshev type-I low-pass prototype, equiripple in passband,
 * cutoff 1 rad/s (the point at which the response first drops to -ripple dB).
 * @param rippleDb Peak-to-peak passband ripple in dB (must be > 0).
 */
Zpk Chebyshev1Prototype(int order, double rippleDb);

/**
 * Analog Chebyshev type-II low-pass prototype, equiripple in stopband,
 * stopband-edge frequency 1 rad/s (the point at which the response first
 * reaches @a stopbandAttenDb of attenuation).
 * @param stopbandAttenDb Stopband attenuation in dB (must be > 0).
 */
Zpk Chebyshev2Prototype(int order, double stopbandAttenDb);

/**
 * Analog elliptic (Cauer) low-pass prototype: equiripple in both passband
 * and stopband. Cutoff is normalized to 1 rad/s (the point at which the
 * gain first drops below @c -rippleDb).
 *
 * @param order            Filter order (>= 1).
 * @param rippleDb         Peak-to-peak passband ripple in dB (> 0).
 * @param stopbandAttenDb  Stopband attenuation in dB (> @a rippleDb).
 */
Zpk EllipticPrototype(int order, double rippleDb, double stopbandAttenDb);

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
 * Same fan-out as LpToBp; the added zeros go to @c ±j·wo instead of the
 * origin.
 */
Zpk AnalogLpToBs(const Zpk& p, double wo, double bw);

/**
 * Bilinear transform of an analog ZPK to a digital ZPK at sample rate @a fs.
 * Analog zeros at infinity map to digital zeros at z = -1 (Nyquist).
 */
Zpk BilinearTransform(const Zpk& analog, double fs);

/**
 * Pre-warp a digital cutoff frequency (Hz) for use as the analog-domain
 * cutoff that, after the bilinear transform at the same @a fs, maps back to
 * exactly that digital cutoff.
 */
double PreWarp(double fc, double fs);

/**
 * Pair conjugate poles (and zeros) into biquad sections. Sections are
 * ordered by ascending |pole| (innermost / least-aggressive first); the
 * overall gain is folded into the first section's numerator.
 */
Sections ZpkToSos(const Zpk& digital);

/**
 * Apply the kind-specific frequency transform (LP/HP/BP/BS) to an analog LP
 * prototype, run the bilinear transform at @a fs, and convert to a SOS
 * cascade. Shared by every classical IIR design function (Butterworth,
 * Chebyshev I/II, Elliptic).
 *
 * @return @c std::nullopt if @a kind is BP/BS and @a f2 is not a valid upper
 *         edge (must satisfy @c f1 < f2 < fs/2). Other validation should be
 *         done by the caller before building @a analogLp.
 */
std::optional<Sections> DesignFromAnalogLp(const Zpk& analogLp, FilterKind kind,
                                           double fs, double f1, double f2);

}  // namespace wpi::filterdesigner::detail
