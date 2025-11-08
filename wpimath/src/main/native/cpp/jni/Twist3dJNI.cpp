// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "org_wpilib_math_jni_Twist3dJNI.h"
#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/math/geometry/Twist3d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_math_jni_Twist3dJNI
 * Method:    exp
 * Signature: (DDDDDD)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_org_wpilib_math_jni_Twist3dJNI_exp
  (JNIEnv* env, jclass, jdouble twistDx, jdouble twistDy, jdouble twistDz,
   jdouble twistRx, jdouble twistRy, jdouble twistRz)
{
  wpi::math::Twist3d twist{wpi::units::meter_t{twistDx},  wpi::units::meter_t{twistDy},
                     wpi::units::meter_t{twistDz},  wpi::units::radian_t{twistRx},
                     wpi::units::radian_t{twistRy}, wpi::units::radian_t{twistRz}};

  wpi::math::Transform3d result = twist.Exp();

  const auto& resultQuaternion = result.Rotation().GetQuaternion();
  return MakeJDoubleArray(
      env, {{result.X().value(), result.Y().value(), result.Z().value(),
             resultQuaternion.W(), resultQuaternion.X(), resultQuaternion.Y(),
             resultQuaternion.Z()}});
}

}  // extern "C"
