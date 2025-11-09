// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "org_wpilib_math_jni_Ellipse2dJNI.h"
#include "wpi/math/geometry/Ellipse2d.hpp"
#include "wpi/util/array.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_math_jni_Ellipse2dJNI
 * Method:    nearest
 * Signature: (DDDDDDD[D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_math_jni_Ellipse2dJNI_nearest
  (JNIEnv* env, jclass, jdouble centerX, jdouble centerY, jdouble centerHeading,
   jdouble xSemiAxis, jdouble ySemiAxis, jdouble pointX, jdouble pointY,
   jdoubleArray nearestPoint)
{
  auto point =
      wpi::math::Ellipse2d{
          wpi::math::Pose2d{wpi::units::meter_t{centerX},
                            wpi::units::meter_t{centerY},
                            wpi::units::radian_t{centerHeading}},
          wpi::units::meter_t{xSemiAxis}, wpi::units::meter_t{ySemiAxis}}
          .Nearest({wpi::units::meter_t{pointX}, wpi::units::meter_t{pointY}});

  wpi::util::array buf{point.X().value(), point.Y().value()};
  env->SetDoubleArrayRegion(nearestPoint, 0, 2, buf.data());
}

}  // extern "C"
