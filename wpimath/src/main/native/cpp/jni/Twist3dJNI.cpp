// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "org_wpilib_math_jni_Twist3dJNI.h"
#include "frc/geometry/Transform3d.h"
#include "frc/geometry/Twist3d.h"
#include "units/angle.h"
#include "units/length.h"

using namespace wpi::java;

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
  frc::Twist3d twist{units::meter_t{twistDx},  units::meter_t{twistDy},
                     units::meter_t{twistDz},  units::radian_t{twistRx},
                     units::radian_t{twistRy}, units::radian_t{twistRz}};

  frc::Transform3d result = twist.Exp();

  const auto& resultQuaternion = result.Rotation().GetQuaternion();
  return MakeJDoubleArray(
      env, {{result.X().value(), result.Y().value(), result.Z().value(),
             resultQuaternion.W(), resultQuaternion.X(), resultQuaternion.Y(),
             resultQuaternion.Z()}});
}

}  // extern "C"
