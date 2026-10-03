// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>

#include "org_wpilib_math_jni_EigenJNI.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_math_jni_EigenJNI
 * Method:    exp
 * Signature: ([DI[D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_math_jni_EigenJNI_exp
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jdoubleArray dst)
{
  JSpan<const jdouble> arrayBody{env, src};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{arrayBody.data(), rows, rows};
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Aexp =
      Amat.exp();

  env->SetDoubleArrayRegion(dst, 0, rows * rows, Aexp.data());
}

/*
 * Class:     org_wpilib_math_jni_EigenJNI
 * Method:    pow
 * Signature: ([DID[D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_math_jni_EigenJNI_pow
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jdouble exponent,
   jdoubleArray dst)
{
  JSpan<const jdouble> arrayBody{env, src};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{arrayBody.data(), rows, rows};
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Apow =
      Amat.pow(exponent);

  env->SetDoubleArrayRegion(dst, 0, rows * rows, Apow.data());
}

}  // extern "C"
