// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <utility>
#include <vector>

#include <sleipnir/autodiff/gradient.hpp>
#include <sleipnir/autodiff/variable.hpp>
#include <sleipnir/autodiff/variable_matrix.hpp>

#include "../SleipnirJNIUtil.hpp"
#include "org_wpilib_math_autodiff_GradientJNI.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_math_autodiff_GradientJNI
 * Method:    create
 * Signature: (J[J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_GradientJNI_create
  (JNIEnv* env, jclass, jlong variable, jlongArray wrt)
{
  auto& variableObj = *reinterpret_cast<slp::Variable<double>*>(variable);

  JSpan<const jlong> wrtSpan{env, wrt};
  slp::VariableMatrix<double> wrtObj(slp::detail::empty, wrtSpan.size(), 1);
  for (size_t i = 0; i < wrtSpan.size(); ++i) {
    wrtObj[i] = *reinterpret_cast<slp::Variable<double>*>(wrtSpan[i]);
  }

  return reinterpret_cast<jlong>(
      new slp::Gradient{variableObj, std::move(wrtObj)});
}

/*
 * Class:     org_wpilib_math_autodiff_GradientJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_math_autodiff_GradientJNI_destroy
  (JNIEnv* env, jclass, jlong handle)
{
  delete reinterpret_cast<slp::Gradient<double>*>(handle);
}

/*
 * Class:     org_wpilib_math_autodiff_GradientJNI
 * Method:    get
 * Signature: (J)[J
 */
JNIEXPORT jlongArray JNICALL
Java_org_wpilib_math_autodiff_GradientJNI_get
  (JNIEnv* env, jclass, jlong handle)
{
  auto& gradient = *reinterpret_cast<slp::Gradient<double>*>(handle);
  auto g = gradient.get();

  std::vector<jlong> varHandles;
  varHandles.reserve(g.size());
  for (auto& var : g) {
    varHandles.emplace_back(
        reinterpret_cast<jlong>(new slp::Variable<double>{var}));
  }

  return MakeJLongArray(env, varHandles);
}

/*
 * Class:     org_wpilib_math_autodiff_GradientJNI
 * Method:    value
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_math_autodiff_GradientJNI_value
  (JNIEnv* env, jclass, jlong handle)
{
  auto& gradient = *reinterpret_cast<slp::Gradient<double>*>(handle);
  return wpi::math::detail::GetTriplets(env, gradient.value());
}

}  // extern "C"
