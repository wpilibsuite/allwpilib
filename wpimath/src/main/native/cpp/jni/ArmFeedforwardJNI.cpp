// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "org_wpilib_math_jni_ArmFeedforwardJNI.h"
#include "wpi/math/controller/ArmFeedforward.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_math_jni_ArmFeedforwardJNI
 * Method:    calculate
 * Signature: (DDDDDDDD)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_math_jni_ArmFeedforwardJNI_calculate
  (JNIEnv* env, jclass, jdouble ks, jdouble kv, jdouble ka, jdouble kg,
   jdouble currentAngle, jdouble currentVelocity, jdouble nextVelocity,
   jdouble dt)
{
  return wpi::math::ArmFeedforward{
      wpi::units::volt_t{ks}, wpi::units::volt_t{kg},
      wpi::units::unit_t<wpi::math::ArmFeedforward::kv_unit>{kv},
      wpi::units::unit_t<wpi::math::ArmFeedforward::ka_unit>{ka},
      wpi::units::second_t{dt}}
      .Calculate(wpi::units::radian_t{currentAngle},
                 wpi::units::radians_per_second_t{currentVelocity},
                 wpi::units::radians_per_second_t{nextVelocity})
      .value();
}

}  // extern "C"
