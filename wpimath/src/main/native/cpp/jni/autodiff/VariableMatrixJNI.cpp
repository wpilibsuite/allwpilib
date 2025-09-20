// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <vector>

#include <sleipnir/autodiff/variable.hpp>
#include <sleipnir/autodiff/variable_matrix.hpp>
#include <wpi/jni_util.h>

#include "edu_wpi_first_math_autodiff_VariableMatrixJNI.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_autodiff_VariableMatrixJNI
 * Method:    solve
 * Signature: ([JI[JI)[J
 */
JNIEXPORT jlongArray JNICALL
Java_edu_wpi_first_math_autodiff_VariableMatrixJNI_solve
  (JNIEnv* env, jclass, jlongArray A, jint Acols, jlongArray B, jint Bcols)
{
  JSpan<const jlong> ASpan{env, A};
  slp::VariableMatrix AObj(slp::VariableMatrix::empty, ASpan.size() / Acols,
                           Acols);
  for (size_t i = 0; i < ASpan.size(); ++i) {
    AObj[i] = *reinterpret_cast<slp::Variable*>(ASpan[i]);
  }

  JSpan<const jlong> BSpan{env, B};
  slp::VariableMatrix BObj(slp::VariableMatrix::empty, BSpan.size() / Bcols,
                           Bcols);
  for (size_t i = 0; i < BSpan.size(); ++i) {
    BObj[i] = *reinterpret_cast<slp::Variable*>(BSpan[i]);
  }

  auto X = slp::solve(AObj, BObj);

  std::vector<jlong> varHandles;
  varHandles.reserve(X.size());
  for (auto& var : X) {
    varHandles.emplace_back(reinterpret_cast<jlong>(new slp::Variable{var}));
  }
  return MakeJLongArray(env, varHandles);
}

}  // extern "C"
