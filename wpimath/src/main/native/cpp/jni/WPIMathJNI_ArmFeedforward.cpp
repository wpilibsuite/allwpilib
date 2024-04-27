// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "edu_wpi_first_math_WPIMathJNI.h"
#include "frc/controller/ArmFeedforward.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    calculate
 * Signature: (DDDD)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_math_WPIMathJNI_ArmFeedforward_calculate(
    JNIEnv* env, jclass, jdouble ks, jdouble kv, jdouble ka, jdouble kg,
    jdouble currentAngle, jdouble currentVelocity, jdouble nextVelocity,
    jdouble dt) {
  using Acceleration = units::compound_unit<units::radians_per_second,
                                            units::inverse<units::second>>;
  using kv_unit =
      units::compound_unit<units::volts,
                           units::inverse<units::radians_per_second>>;
  using ka_unit =
      units::compound_unit<units::volts, units::inverse<Acceleration>>;

  auto armFeedForward = frc::ArmFeedforward{
      units::volt_t{ks}, units::volt_t{kg}, units::unit_t<kv_unit>{kv},
      units::unit_t<ka_unit>{ka}};

  units::volt_t voltage{armFeedForward.Calculate(
      units::radian_t{currentAngle},
      units::radians_per_second_t{currentVelocity},
      units::radians_per_second_t{nextVelocity}, units::second_t{dt})};
  return voltage.value();
}

}  // extern "C"
