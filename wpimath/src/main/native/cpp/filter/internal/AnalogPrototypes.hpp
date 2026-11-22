// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Zpk.hpp"

namespace wpi::math::filter::internal {

/** Analog Butterworth low-pass prototype, cutoff 1 rad/s. */
Zpk ButterworthPrototype(int order);

/**
 * Analog Chebyshev type-I low-pass prototype, equiripple in passband,
 * cutoff 1 rad/s (the point at which the response first drops to -ripple dB).
 *
 * @param rippleDb Peak-to-peak passband ripple in dB (must be > 0).
 */
Zpk ChebyshevIPrototype(int order, double rippleDb);

/**
 * Analog Chebyshev type-II low-pass prototype, equiripple in stopband,
 * stopband-edge frequency 1 rad/s (the point at which the response first
 * reaches @a stopAttenDb of attenuation).
 *
 * @param stopAttenDb Stopband attenuation in dB (must be > 0).
 */
Zpk ChebyshevIIPrototype(int order, double stopAttenDb);

/**
 * Analog elliptic (Cauer) low-pass prototype: equiripple in both passband
 * and stopband. Cutoff is normalized to 1 rad/s (the point at which the
 * gain first drops below -rippleDb).
 *
 * @param order        Filter order (>= 1).
 * @param rippleDb     Peak-to-peak passband ripple in dB (> 0).
 * @param stopAttenDb  Stopband attenuation in dB (> @a rippleDb).
 */
Zpk EllipticPrototype(int order, double rippleDb, double stopAttenDb);

}  // namespace wpi::math::filter::internal
