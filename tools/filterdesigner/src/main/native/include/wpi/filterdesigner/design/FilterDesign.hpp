// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <vector>

#include "wpi/math/filter/BiquadFilter.hpp"

namespace wpi::filterdesigner {

using Section = wpi::math::BiquadFilter::Section;
using Sections = std::vector<Section>;

/** Supported analog-prototype filter kinds. */
enum class FilterKind { LowPass, HighPass, BandPass, BandStop };

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

/**
 * Designs a Butterworth IIR filter as a cascade of biquad sections.
 *
 * Coefficients match @c scipy.signal.butter(order, Wn, btype, fs, output='sos')
 * to within ~1e-10 for LowPass/HighPass. BandPass/BandStop outputs are
 * numerically equivalent to scipy but may differ in section ordering / zero
 * pairing; the product response matches.
 *
 * @param kind  LowPass, HighPass, BandPass, or BandStop.
 * @param order Prototype order (>=1). For BandPass/BandStop, the resulting
 *              cascade has @c 2*order poles.
 * @param fs    Sample rate (Hz). Must be positive.
 * @param f1    Cutoff (Hz) for LP/HP, or low edge for BP/BS. Must satisfy
 *              @c 0 < f1 < fs/2.
 * @param f2    Only for BP/BS: high edge (Hz). Must satisfy
 *              @c f1 < f2 < fs/2.
 *
 * @return SOS cascade, or @c std::nullopt on invalid arguments.
 */
std::optional<Sections> DesignButterworth(FilterKind kind, int order, double fs,
                                          double f1, double f2 = 0.0);

/**
 * Designs a Chebyshev type-I IIR filter as a cascade of biquad sections.
 * Equiripple in the passband, monotonic in the stopband. Coefficients match
 * @c scipy.signal.cheby1(order, rp, Wn, btype, fs, output='sos').
 *
 * @param kind     LowPass, HighPass, BandPass, or BandStop.
 * @param order    Prototype order (>=1). For BP/BS the cascade has @c 2*order
 *                 poles.
 * @param fs       Sample rate (Hz). Must be positive.
 * @param f1       Cutoff (LP/HP) or low edge (BP/BS), in Hz. The cutoff is
 *                 the frequency at which the response first drops to
 *                 @c -rippleDb dB.
 * @param f2       Only for BP/BS: high edge (Hz). Must satisfy
 *                 @c f1 < f2 < fs/2.
 * @param rippleDb Peak-to-peak passband ripple in dB. Must be > 0; values
 *                 from ~0.1 to ~3 dB are typical.
 *
 * @return SOS cascade, or @c std::nullopt on invalid arguments.
 */
std::optional<Sections> DesignChebyshev1(FilterKind kind, int order, double fs,
                                         double f1, double f2, double rippleDb);

/**
 * Designs a Chebyshev type-II (inverse Chebyshev) IIR filter as a cascade of
 * biquad sections. Monotonic in the passband, equiripple in the stopband.
 * Coefficients match @c scipy.signal.cheby2(order, rs, Wn, btype, fs,
 * output='sos').
 *
 * @param kind             LowPass, HighPass, BandPass, or BandStop.
 * @param order            Prototype order (>=1). For BP/BS the cascade has
 *                         @c 2*order poles.
 * @param fs               Sample rate (Hz). Must be positive.
 * @param f1               Stopband edge (LP/HP) or low edge (BP/BS), in Hz.
 *                         For LP/HP this is the frequency at which the
 *                         response first reaches @a stopbandAttenDb of
 *                         attenuation.
 * @param f2               Only for BP/BS: high stopband edge (Hz). Must
 *                         satisfy @c f1 < f2 < fs/2.
 * @param stopbandAttenDb  Stopband attenuation in dB. Must be > 0; values
 *                         from ~20 to ~80 dB are typical.
 *
 * @return SOS cascade, or @c std::nullopt on invalid arguments.
 */
std::optional<Sections> DesignChebyshev2(FilterKind kind, int order, double fs,
                                         double f1, double f2,
                                         double stopbandAttenDb);

/**
 * Designs an elliptic (Cauer) IIR filter as a cascade of biquad sections.
 * Equiripple in both passband and stopband — the steepest transition for a
 * given order, at the cost of ripple everywhere. Coefficients match
 * @c scipy.signal.ellip(order, rp, rs, Wn, btype, fs, output='sos').
 *
 * @param kind             LowPass, HighPass, BandPass, or BandStop.
 * @param order            Filter order (>= 1).
 * @param fs               Sample rate (Hz). Must be positive.
 * @param f1               Cutoff (LP/HP) or low edge (BP/BS), in Hz. The
 *                         cutoff is the frequency at which the response
 *                         first drops to @c -rippleDb dB.
 * @param f2               Only for BP/BS: high edge (Hz).
 * @param rippleDb         Passband ripple in dB (> 0).
 * @param stopbandAttenDb  Stopband attenuation in dB (must exceed
 *                         @a rippleDb).
 *
 * @return SOS cascade, or @c std::nullopt on invalid arguments.
 */
std::optional<Sections> DesignElliptic(FilterKind kind, int order, double fs,
                                       double f1, double f2, double rippleDb,
                                       double stopbandAttenDb);

/**
 * Designs a second-order IIR notch at center frequency @a f0 (Hz) with
 * quality factor @a q. Matches @c scipy.signal.iirnotch.
 *
 * @return Single-section cascade, or @c std::nullopt on invalid arguments
 *         (non-positive @a fs / @a q, or @c f0 outside (0, fs/2)).
 */
std::optional<Sections> DesignNotch(double fs, double f0, double q);

/**
 * Designs an N-tap moving-average filter as a cascade of FIR biquads.
 *
 * Each section has @c a1 = a2 = 0 (all poles at the origin). The total
 * gain @c 1/taps is folded into the first section's numerator so the DC
 * gain of the cascade is 1.
 *
 * @param taps Length of the moving-average window. Must be >= 1.
 */
std::optional<Sections> DesignMovingAverage(int taps);

}  // namespace wpi::filterdesigner
