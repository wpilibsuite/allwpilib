// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <vector>

#include <sleipnir/autodiff/variable.hpp>
#include <sleipnir/autodiff/variable_matrix.hpp>

#include "org_wpilib_math_autodiff_VariableMatrixJNI.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_math_autodiff_VariableMatrixJNI
 * Method:    solve
 * Signature: ([JI[JI)[J
 */
JNIEXPORT jlongArray JNICALL
Java_org_wpilib_math_autodiff_VariableMatrixJNI_solve
  (JNIEnv* env, jclass, jlongArray A, jint Acols, jlongArray B, jint Bcols)
{
  JSpan<const jlong> ASpan{env, A};
  slp::VariableMatrix<double> AObj(slp::detail::empty, ASpan.size() / Acols,
                                   Acols);
  for (size_t i = 0; i < ASpan.size(); ++i) {
    AObj[i] = *reinterpret_cast<slp::Variable<double>*>(ASpan[i]);
  }

  JSpan<const jlong> BSpan{env, B};
  slp::VariableMatrix<double> BObj(slp::detail::empty, BSpan.size() / Bcols,
                                   Bcols);
  for (size_t i = 0; i < BSpan.size(); ++i) {
    BObj[i] = *reinterpret_cast<slp::Variable<double>*>(BSpan[i]);
  }

  auto X = slp::solve(AObj, BObj);

  std::vector<jlong> varHandles;
  varHandles.reserve(X.size());
  for (auto& var : X) {
    varHandles.emplace_back(
        reinterpret_cast<jlong>(new slp::Variable<double>{var}));
  }
  return MakeJLongArray(env, varHandles);
}

}  // extern "C"
