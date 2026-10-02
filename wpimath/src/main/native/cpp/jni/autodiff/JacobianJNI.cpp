// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <utility>
#include <vector>

#include <sleipnir/autodiff/jacobian.hpp>
#include <sleipnir/autodiff/variable.hpp>
#include <sleipnir/autodiff/variable_matrix.hpp>

#include "../SleipnirJNIUtil.hpp"
#include "org_wpilib_math_autodiff_JacobianJNI.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_math_autodiff_JacobianJNI
 * Method:    create
 * Signature: ([J[J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_JacobianJNI_create
  (JNIEnv* env, jclass, jlongArray variables, jlongArray wrt)
{
  JSpan<const jlong> variablesSpan{env, variables};
  slp::VariableMatrix<double> variablesObj(slp::detail::empty,
                                           variablesSpan.size(), 1);
  for (size_t i = 0; i < variablesSpan.size(); ++i) {
    variablesObj[i] =
        *reinterpret_cast<slp::Variable<double>*>(variablesSpan[i]);
  }

  JSpan<const jlong> wrtSpan{env, wrt};
  slp::VariableMatrix<double> wrtObj(slp::detail::empty, wrtSpan.size(), 1);
  for (size_t i = 0; i < wrtSpan.size(); ++i) {
    wrtObj[i] = *reinterpret_cast<slp::Variable<double>*>(wrtSpan[i]);
  }

  return reinterpret_cast<jlong>(
      new slp::Jacobian{std::move(variablesObj), std::move(wrtObj)});
}

/*
 * Class:     org_wpilib_math_autodiff_JacobianJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_math_autodiff_JacobianJNI_destroy
  (JNIEnv* env, jclass, jlong handle)
{
  delete reinterpret_cast<slp::Jacobian<double>*>(handle);
}

/*
 * Class:     org_wpilib_math_autodiff_JacobianJNI
 * Method:    get
 * Signature: (J)[J
 */
JNIEXPORT jlongArray JNICALL
Java_org_wpilib_math_autodiff_JacobianJNI_get
  (JNIEnv* env, jclass, jlong handle)
{
  auto& jacobian = *reinterpret_cast<slp::Jacobian<double>*>(handle);
  auto J = jacobian.get();

  std::vector<jlong> varHandles;
  varHandles.reserve(J.size());
  for (auto& var : J) {
    varHandles.emplace_back(
        reinterpret_cast<jlong>(new slp::Variable<double>{var}));
  }

  return MakeJLongArray(env, varHandles);
}

/*
 * Class:     org_wpilib_math_autodiff_JacobianJNI
 * Method:    value
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_math_autodiff_JacobianJNI_value
  (JNIEnv* env, jclass, jlong handle)
{
  auto& jacobian = *reinterpret_cast<slp::Jacobian<double>*>(handle);
  return wpi::math::detail::GetTriplets(env, jacobian.value());
}

}  // extern "C"
