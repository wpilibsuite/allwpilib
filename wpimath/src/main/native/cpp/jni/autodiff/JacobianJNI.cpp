// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <utility>

#include <sleipnir/autodiff/jacobian.hpp>
#include <sleipnir/autodiff/variable.hpp>
#include <wpi/jni_util.h>

#include "../SleipnirJNIUtil.h"
#include "edu_wpi_first_math_autodiff_JacobianJNI.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_autodiff_JacobianJNI
 * Method:    create
 * Signature: ([J[J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_JacobianJNI_create
  (JNIEnv* env, jclass, jlongArray variables, jlongArray wrt)
{
  JSpan<const jlong> variablesSpan{env, variables};
  slp::VariableMatrix variablesObj(slp::VariableMatrix::empty,
                                   variablesSpan.size(), 1);
  for (size_t i = 0; i < variablesSpan.size(); ++i) {
    variablesObj[i] = *reinterpret_cast<slp::Variable*>(variablesSpan[i]);
  }

  JSpan<const jlong> wrtSpan{env, wrt};
  slp::VariableMatrix wrtObj(slp::VariableMatrix::empty, wrtSpan.size(), 1);
  for (size_t i = 0; i < wrtSpan.size(); ++i) {
    wrtObj[i] = *reinterpret_cast<slp::Variable*>(wrtSpan[i]);
  }

  return reinterpret_cast<jlong>(
      new slp::Jacobian{std::move(variablesObj), std::move(wrtObj)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_JacobianJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_autodiff_JacobianJNI_destroy
  (JNIEnv* env, jclass, jlong handle)
{
  delete reinterpret_cast<slp::Jacobian*>(handle);
}

/*
 * Class:     edu_wpi_first_math_autodiff_JacobianJNI
 * Method:    get
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_math_autodiff_JacobianJNI_get
  (JNIEnv* env, jclass, jlong handle)
{
  auto& jacobian = *reinterpret_cast<slp::Jacobian*>(handle);
  return frc::VariableMatrixCppToJava(env, jacobian.get());
}

/*
 * Class:     edu_wpi_first_math_autodiff_JacobianJNI
 * Method:    value
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_math_autodiff_JacobianJNI_value
  (JNIEnv* env, jclass, jlong handle)
{
  auto& jacobian = *reinterpret_cast<slp::Jacobian*>(handle);
  return frc::EigenToEJML(env, jacobian.value());
}

}  // extern "C"
