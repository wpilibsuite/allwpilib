// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <utility>

#include <sleipnir/autodiff/hessian.hpp>
#include <sleipnir/autodiff/variable.hpp>
#include <wpi/jni_util.h>

#include "../SleipnirJNIUtil.h"
#include "edu_wpi_first_math_autodiff_HessianJNI.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_autodiff_HessianJNI
 * Method:    create
 * Signature: (J[J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_HessianJNI_create
  (JNIEnv* env, jclass, jlong variable, jlongArray wrt)
{
  auto& variableObj = *reinterpret_cast<slp::Variable*>(variable);

  JSpan<const jlong> wrtSpan{env, wrt};
  slp::VariableMatrix wrtObj(slp::VariableMatrix::empty, wrtSpan.size(), 1);
  for (size_t i = 0; i < wrtSpan.size(); ++i) {
    wrtObj[i] = *reinterpret_cast<slp::Variable*>(wrtSpan[i]);
  }

  return reinterpret_cast<jlong>(new slp::Hessian<Eigen::Lower | Eigen::Upper>{
      variableObj, std::move(wrtObj)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_HessianJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_autodiff_HessianJNI_destroy
  (JNIEnv* env, jclass, jlong handle)
{
  delete reinterpret_cast<slp::Hessian<Eigen::Lower | Eigen::Upper>*>(handle);
}

/*
 * Class:     edu_wpi_first_math_autodiff_HessianJNI
 * Method:    get
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_math_autodiff_HessianJNI_get
  (JNIEnv* env, jclass, jlong handle)
{
  auto& hessian =
      *reinterpret_cast<slp::Hessian<Eigen::Lower | Eigen::Upper>*>(handle);
  return frc::VariableMatrixCppToJava(env, hessian.get());
}

/*
 * Class:     edu_wpi_first_math_autodiff_HessianJNI
 * Method:    value
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_math_autodiff_HessianJNI_value
  (JNIEnv* env, jclass, jlong handle)
{
  auto& hessian =
      *reinterpret_cast<slp::Hessian<Eigen::Lower | Eigen::Upper>*>(handle);
  return frc::EigenToEJML(env, hessian.value());
}

}  // extern "C"
