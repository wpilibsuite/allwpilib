// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <jni.h>

#include <concepts>
#include <vector>

#include <Eigen/SparseCore>

#include "wpi/util/jni_util.hpp"

namespace wpi::math::detail {

/**
 * Converts Eigen sparse matrix to triplets.
 *
 * @param env JNI environment.
 * @param mat Eigen sparse matrix to convert.
 * @return NativeSparseTriplets instance.
 */
template <typename Derived>
  requires std::derived_from<Derived, Eigen::SparseCompressedBase<Derived>>
jobject GetTriplets(JNIEnv* env, const Derived& mat) {
  const int nonZeros = mat.nonZeros();

  std::vector<int> rows;
  rows.reserve(nonZeros);

  std::vector<int> cols;
  cols.reserve(nonZeros);

  std::vector<double> values;
  values.reserve(nonZeros);

  for (int k = 0; k < mat.outerSize(); ++k) {
    for (typename Derived::InnerIterator it{mat, k}; it; ++it) {
      rows.emplace_back(it.row());
      cols.emplace_back(it.col());
      values.emplace_back(it.value());
    }
  }

  // Find NativeSparseTriplets class
  static wpi::util::java::JClass cls{
      env, "org/wpilib/math/autodiff/NativeSparseTriplets"};
  if (!cls) {
    return nullptr;
  }

  // Find NativeSparseTriplets constructor
  static jmethodID ctor = env->GetMethodID(cls, "<init>", "([I[I[D)V");
  if (!ctor) {
    return nullptr;
  }

  return env->NewObject(cls, ctor, wpi::util::java::MakeJIntArray(env, rows),
                        wpi::util::java::MakeJIntArray(env, cols),
                        wpi::util::java::MakeJDoubleArray(env, values));
}

}  // namespace wpi::math::detail
