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
 * @param reference Reference input of a controller.
 * @param measurement The current measurement.
 * @param minimumInput The minimum value expected from the input.
 * @param maximumInput The maximum value expected from the input.
 */
template <typename T>
T GetModulusError(T reference, T measurement, T minimumInput, T maximumInput) {
  T error = reference - measurement;
  T modulus = maximumInput - minimumInput;

  // Wrap error above maximum input
  int numMax = (error + maximumInput) / modulus;
  error -= numMax * modulus;

  // Wrap error below minimum input
  int numMin = (error + minimumInput) / modulus;
  error -= numMin * modulus;

  return error;
}

}  // namespace frc
