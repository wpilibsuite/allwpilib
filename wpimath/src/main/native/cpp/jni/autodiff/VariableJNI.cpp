// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <sleipnir/autodiff/variable.hpp>

#include "org_wpilib_math_autodiff_VariableJNI.h"

extern "C" {

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    createDefault
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_createDefault
  (JNIEnv* env, jclass)
{
  return reinterpret_cast<jlong>(new slp::Variable<double>{});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    createDouble
 * Signature: (D)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_createDouble
  (JNIEnv* env, jclass, jdouble value)
{
  return reinterpret_cast<jlong>(new slp::Variable<double>{value});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    createInt
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_createInt
  (JNIEnv* env, jclass, jint value)
{
  return reinterpret_cast<jlong>(new slp::Variable<double>{value});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_destroy
  (JNIEnv* env, jclass, jlong handle)
{
  delete reinterpret_cast<slp::Variable<double>*>(handle);
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    setValue
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_setValue
  (JNIEnv* env, jclass, jlong handle, jdouble value)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  lhsVar.set_value(value);
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    times
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_times
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable<double>*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable<double>{lhsVar * rhsVar});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    div
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_div
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable<double>*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable<double>{lhsVar / rhsVar});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    plus
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_plus
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable<double>*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable<double>{lhsVar + rhsVar});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    minus
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_minus
  (JNIEnv* env, jclass, jlong handle, jlong rhs)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  auto& rhsVar = *reinterpret_cast<slp::Variable<double>*>(rhs);
  return reinterpret_cast<jlong>(new slp::Variable<double>{lhsVar - rhsVar});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    unaryMinus
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_unaryMinus
  (JNIEnv* env, jclass, jlong handle)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  return reinterpret_cast<jlong>(new slp::Variable<double>{-lhsVar});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    value
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_value
  (JNIEnv* env, jclass, jlong handle)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  return lhsVar.value();
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    type
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_type
  (JNIEnv* env, jclass, jlong handle)
{
  auto& lhsVar = *reinterpret_cast<slp::Variable<double>*>(handle);
  return static_cast<jint>(lhsVar.type());
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    abs
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_abs
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{abs(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    acos
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_acos
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{acos(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    asin
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_asin
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{asin(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    atan
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_atan
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{atan(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    atan2
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_atan2
  (JNIEnv* env, jclass, jlong y, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  auto& yVar = *reinterpret_cast<slp::Variable<double>*>(y);
  return reinterpret_cast<jlong>(new slp::Variable<double>{atan2(yVar, xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    cbrt
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_cbrt
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{cbrt(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    cos
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_cos
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{cos(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    cosh
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_cosh
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{cosh(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    exp
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_exp
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{exp(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    hypot
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_hypot
  (JNIEnv* env, jclass, jlong x, jlong y)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  auto& yVar = *reinterpret_cast<slp::Variable<double>*>(y);
  return reinterpret_cast<jlong>(new slp::Variable<double>{hypot(xVar, yVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    pow
 * Signature: (JJ)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_pow
  (JNIEnv* env, jclass, jlong base, jlong power)
{
  auto& baseVar = *reinterpret_cast<slp::Variable<double>*>(base);
  auto& powerVar = *reinterpret_cast<slp::Variable<double>*>(power);
  return reinterpret_cast<jlong>(
      new slp::Variable<double>{pow(baseVar, powerVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    log
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_log
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{log(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    log10
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_log10
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{log10(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    signum
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_signum
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{sign(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    sin
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_sin
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{sin(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    sinh
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_sinh
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{sinh(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    sqrt
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_sqrt
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{sqrt(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    tan
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_tan
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{tan(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    tanh
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_tanh
  (JNIEnv* env, jclass, jlong x)
{
  auto& xVar = *reinterpret_cast<slp::Variable<double>*>(x);
  return reinterpret_cast<jlong>(new slp::Variable<double>{tanh(xVar)});
}

/*
 * Class:     org_wpilib_math_autodiff_VariableJNI
 * Method:    totalNativeMemoryUsage
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_math_autodiff_VariableJNI_totalNativeMemoryUsage
  (JNIEnv* env, jclass)
{
  return slp::global_pool_resource().blocks_in_use() *
         sizeof(slp::detail::Expression<double>);
}

}  // extern "C"
