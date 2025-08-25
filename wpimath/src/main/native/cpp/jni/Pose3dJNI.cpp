// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "edu_wpi_first_math_jni_Pose3dJNI.h"
#include "frc/geometry/Pose3d.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_jni_Pose3dJNI
 * Method:    exp
 * Signature: (DDDDDD)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_wpi_first_math_jni_Pose3dJNI_exp
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

/*
 * Class:     edu_wpi_first_math_jni_Pose3dJNI
 * Method:    log
 * Signature: (DDDDDDD)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_wpi_first_math_jni_Pose3dJNI_log
  (JNIEnv* env, jclass, jdouble relX, jdouble relY, jdouble relZ, jdouble relQw,
   jdouble relQx, jdouble relQy, jdouble relQz)
{
  frc::Transform3d transform3d{
      units::meter_t{relX}, units::meter_t{relY}, units::meter_t{relZ},
      frc::Rotation3d{frc::Quaternion{relQw, relQx, relQy, relQz}}};

  frc::Twist3d result = transform3d.Log();

  return MakeJDoubleArray(
      env, {{result.dx.value(), result.dy.value(), result.dz.value(),
             result.rx.value(), result.ry.value(), result.rz.value()}});
}

}  // extern "C"
