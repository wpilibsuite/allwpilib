/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cmath>
#include <type_traits>

#include <units/math.h>

namespace frc {

/**
 * Returns modulus of error where error is the difference between the reference
 * and a measurement.
 *
 * This implements modular subtraction defined as:
 *
 * e = (r mod m - x mod m) mod m
 *
 * with an offset in the modulus range for minimum input.
 *
 * @param reference Reference input of a controller.
 * @param measurement The current measurement.
 * @param minimumInput The minimum value expected from the input.
 * @param maximumInput The maximum value expected from the input.
 */
template <typename T>
T GetModulusError(T reference, T measurement, T minimumInput, T maximumInput) {
  T modulus = maximumInput - minimumInput;

  if constexpr (std::is_same_v<T, double>) {
    T error = std::fmod(reference, modulus) - std::fmod(measurement, modulus);

    // Moduli on the difference arguments establish a precondition for the
    // following modulus.
    return std::fmod(error - minimumInput, modulus) + minimumInput;
  } else if constexpr (std::is_same_v<T, int>) {
    T error = reference % modulus - measurement % modulus;

    // Moduli on the difference arguments establish a precondition for the
    // following modulus.
    return (error - minimumInput) % modulus + minimumInput;
  } else {
    T error = units::math::fmod(reference, modulus) -
              units::math::fmod(measurement, modulus);

    // Moduli on the difference arguments establish a precondition for the
    // following modulus.
    return units::math::fmod(error - minimumInput, modulus) + minimumInput;
  }
}

}  // namespace frc
