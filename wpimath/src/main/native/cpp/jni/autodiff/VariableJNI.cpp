// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <sleipnir/autodiff/variable.hpp>
#include <wpi/jni_util.h>

#include "edu_wpi_first_math_autodiff_VariableJNI.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    createDefault
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_createDefault
  (JNIEnv* env, jclass)
{
  return reinterpret_cast<jlong>(new slp::Variable{});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    createDouble
 * Signature: (D)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_createDouble
  (JNIEnv* env, jclass, jdouble value)
{
  return reinterpret_cast<jlong>(new slp::Variable{value});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    createInt
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_createInt
  (JNIEnv* env, jclass, jint value)
{
  return reinterpret_cast<jlong>(new slp::Variable{value});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_destroy
  (JNIEnv* env, jclass, jlong handle)
{
  delete reinterpret_cast<slp::Variable*>(handle);
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    setValue
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_setValue
  (JNIEnv* env, jclass, jlong handle, jdouble value)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  lhsVar.set_value(value);
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    times
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_times
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable{lhsVar * rhsVar});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    div
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_div
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable{lhsVar / rhsVar});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    plus
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_plus
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable{lhsVar + rhsVar});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    minus
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_minus
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable{lhsVar - rhsVar});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    unaryMinus
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_unaryMinus
  (JNIEnv* env, jclass, jlong handle)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  return reinterpret_cast<jlong>(new slp::Variable{-lhsVar});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    value
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_value
  (JNIEnv* env, jclass, jlong handle)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  return lhsVar.value();
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    type
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_type
  (JNIEnv* env, jclass, jlong handle)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable*>(handle);
  return static_cast<int>(lhsVar.type());
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    abs
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_abs
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::abs(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    acos
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_acos
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::acos(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    asin
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_asin
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::asin(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    atan
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_atan
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::atan(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    atan2
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_atan2
  (JNIEnv* env, jclass, jlong y, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  auto& yVar = *reinterpret_cast<slp::Variable*>(y);
  return reinterpret_cast<jlong>(new slp::Variable{slp::atan2(yVar, xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    cbrt
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_cbrt
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::cbrt(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    cos
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_cos
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::cos(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    cosh
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_cosh
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::cosh(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    exp
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_exp
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::exp(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    hypot
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_hypot
  (JNIEnv* env, jclass, jlong x, jlong y)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  auto& yVar = *reinterpret_cast<slp::Variable*>(y);
  return reinterpret_cast<jlong>(new slp::Variable{slp::hypot(xVar, yVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    pow
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_pow
  (JNIEnv* env, jclass, jlong base, jlong power)
{
  auto& baseVar = *reinterpret_cast<slp::Variable*>(base);
  auto& powerVar = *reinterpret_cast<slp::Variable*>(power);
  return reinterpret_cast<jlong>(
      new slp::Variable{slp::pow(baseVar, powerVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    log
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_log
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::log(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    log10
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_log10
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::log10(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    signum
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_signum
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::sign(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    sin
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_sin
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::sin(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    sinh
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_sinh
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::sinh(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    sqrt
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_sqrt
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::sqrt(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    tan
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_tan
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::tan(xVar)});
}

/*
 * Class:     edu_wpi_first_math_autodiff_VariableJNI
 * Method:    tanh
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_autodiff_VariableJNI_tanh
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable*>(x);
  return reinterpret_cast<jlong>(new slp::Variable{slp::tanh(xVar)});
}

}  // extern "C"
