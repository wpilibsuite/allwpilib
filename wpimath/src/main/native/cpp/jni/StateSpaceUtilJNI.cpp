// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <Eigen/Core>
#include <wpi/jni_util.h>

#include "edu_wpi_first_math_jni_StateSpaceUtilJNI.h"
#include "frc/StateSpaceUtil.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_jni_StateSpaceUtilJNI
 * Method:    isStabilizable
 * Signature: (II[D[D)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_math_jni_StateSpaceUtilJNI_isStabilizable
  (JNIEnv* env, jclass, jint states, jint inputs, jdoubleArray aSrc,
   jdoubleArray bSrc)
{
  JSpan<const jdouble> nativeA{env, aSrc};
  JSpan<const jdouble> nativeB{env, bSrc};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      A{nativeA.data(), states, states};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      B{nativeB.data(), states, inputs};

  bool isStabilizable =
      frc::IsStabilizable<Eigen::Dynamic, Eigen::Dynamic>(A, B);

  return isStabilizable;
}

}  // extern "C"
