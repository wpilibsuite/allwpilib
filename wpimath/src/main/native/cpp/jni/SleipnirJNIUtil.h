// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <jni.h>

#include <vector>

#include <Eigen/SparseCore>
#include <sleipnir/autodiff/variable_matrix.hpp>
#include <wpi/jni_util.h>

namespace frc {

/**
 * Converts Eigen::SparseMatrix<double> into EJML DMatrixSparseCSC.
 *
 * @param env JNI environment.
 * @param mat Eigen sparse matrix to convert.
 * @return DMatrixSparseCSC instance.
 */
inline jobject VariableMatrixCppToJava(JNIEnv* env,
                                       const slp::VariableMatrix& mat) {
  // Find VariableMatrix class
  static wpi::java::JClass cls{env,
                               "edu/wpi/first/math/autodiff/VariableMatrix"};
  if (!cls) {
    return nullptr;
  }

  // Find VariableMatrix.fromHandles()
  static jmethodID fromHandlesMethodId = env->GetStaticMethodID(
      cls, "fromHandles", "(II[J)Ledu/wpi/first/math/autodiff/VariableMatrix;");
  if (!fromHandlesMethodId) {
    return nullptr;
  }

  // Make vector of handles from VariableMatrix
  std::vector<jlong> varHandles;
  varHandles.reserve(mat.size());
  for (auto& var : mat) {
    varHandles.emplace_back(reinterpret_cast<jlong>(new slp::Variable{var}));
  }
  auto handles = wpi::java::MakeJLongArray(env, varHandles);

  // Call VariableMatrix.fromHandles()
  return env->CallStaticObjectMethod(cls, fromHandlesMethodId, mat.rows(),
                                     mat.cols(), handles);
}

/**
 * Converts EJML matrix internal representation (e.g., DMatrixRMaj,
 * DMatrixSparseCSC) to EJML SimpleMatrix.
 *
 * @param env JNI environment.
 * @param mat EJML matrix object to convert.
 */
inline jobject DMatrixToSimpleMatrix(JNIEnv* env, jobject mat) {
  // Find SimpleMatrix class
  static wpi::java::JClass simpleCls{env, "org/ejml/simple/SimpleMatrix"};
  if (!simpleCls) {
    return nullptr;
  }

  // Make SimpleMatrix instance
  static jmethodID simpleCtor =
      env->GetMethodID(simpleCls, "<init>", "(Lorg/ejml/data/Matrix;)V");
  if (!simpleCtor) {
    return nullptr;
  }
  return env->NewObject(simpleCls, simpleCtor, mat);
}

/**
 * Converts Eigen::VectorXd into EJML SimpleMatrix.
 *
 * @param env JNI environment.
 * @param mat Eigen dense vector to convert.
 * @return SimpleMatrix instance.
 */
inline jobject EigenToEJML(JNIEnv* env, const Eigen::VectorXd& vec) {
  // Find DMatrixRMaj class
  static wpi::java::JClass cls{env, "org/ejml/data/DMatrixRMaj"};
  if (!cls) {
    return nullptr;
  }

  // Make DMatrixRMaj instance
  static jmethodID ctor = env->GetMethodID(cls, "<init>", "(II)V");
  if (!ctor) {
    return nullptr;
  }
  jobject obj = env->NewObject(cls, ctor, vec.rows(), vec.cols());

  // Set elements of DMatrixRMaj instance
  static jmethodID setMethodId = env->GetMethodID(cls, "set", "(IID)V");
  if (!setMethodId) {
    return nullptr;
  }
  for (int index = 0; index < vec.rows(); ++index) {
    env->CallVoidMethod(obj, setMethodId, index, 0, vec[index]);
  }

  return DMatrixToSimpleMatrix(env, obj);
}

/**
 * Converts Eigen::SparseVector<double> into EJML SimpleMatrix.
 *
 * @param env JNI environment.
 * @param mat Eigen sparse matrix to convert.
 * @return SimpleMatrix instance.
 */
inline jobject EigenToEJML(JNIEnv* env,
                           const Eigen::SparseVector<double>& vec) {
  // Find DMatrixSparseTriplet class
  static wpi::java::JClass tripletCls{env,
                                      "org/ejml/data/DMatrixSparseTriplet"};
  if (!tripletCls) {
    return nullptr;
  }

  // Make DMatrixSparseTriplet instance
  static jmethodID tripletCtor =
      env->GetMethodID(tripletCls, "<init>", "(III)V");
  if (!tripletCtor) {
    return nullptr;
  }
  jobject tripletObj = env->NewObject(tripletCls, tripletCtor, vec.rows(),
                                      vec.cols(), vec.nonZeros());

  // Find DMatrixSparseTriplet.addItem()
  static jmethodID addItemMethodId =
      env->GetMethodID(tripletCls, "addItem", "(IID)V");
  if (!addItemMethodId) {
    return nullptr;
  }

  // Add items to DMatrixSparseTriplet instance
  for (int k = 0; k < vec.outerSize(); ++k) {
    for (Eigen::SparseVector<double>::InnerIterator it{vec, k}; it; ++it) {
      env->CallVoidMethod(tripletObj, addItemMethodId, it.row(), it.col(),
                          it.value());
    }
  }

  // Find DMatrixSparseTriplet class
  static wpi::java::JClass convertCls{env, "org/ejml/ops/DConvertMatrixStruct"};
  if (!convertCls) {
    return nullptr;
  }

  // Turn DMatrixSparseTriplet into DMatrixSparseCSC
  static jmethodID convertMethodId = env->GetStaticMethodID(
      convertCls, "convert",
      "(Lorg/ejml/data/DMatrixSparseTriplet;Lorg/ejml/data/"
      "DMatrixSparseCSC;)Lorg/ejml/data/DMatrixSparseCSC;");
  if (!convertMethodId) {
    return nullptr;
  }
  jobject dmatrixObj =
      env->CallObjectMethod(convertCls, convertMethodId, tripletObj, nullptr);

  return DMatrixToSimpleMatrix(env, dmatrixObj);
}

/**
 * Converts Eigen::SparseMatrix<double> into EJML SimpleMatrix.
 *
 * @param env JNI environment.
 * @param mat Eigen sparse matrix to convert.
 * @return SimpleMatrix instance.
 */
inline jobject EigenToEJML(JNIEnv* env,
                           const Eigen::SparseMatrix<double>& mat) {
  // Find DMatrixSparseTriplet class
  static wpi::java::JClass tripletCls{env,
                                      "org/ejml/data/DMatrixSparseTriplet"};
  if (!tripletCls) {
    return nullptr;
  }

  // Find DMatrixSparseTriplet constructor
  static jmethodID tripletCtor =
      env->GetMethodID(tripletCls, "<init>", "(III)V");
  if (!tripletCtor) {
    return nullptr;
  }

  // Make DMatrixSparseTriplet instance
  jobject tripletObj = env->NewObject(tripletCls, tripletCtor, mat.rows(),
                                      mat.cols(), mat.nonZeros());

  // Find DMatrixSparseTriplet.addItem()
  static jmethodID addItemMethodId =
      env->GetMethodID(tripletCls, "addItem", "(IID)V");
  if (!addItemMethodId) {
    return nullptr;
  }

  // Add items to DMatrixSparseTriplet instance
  for (int k = 0; k < mat.outerSize(); ++k) {
    for (Eigen::SparseMatrix<double>::InnerIterator it{mat, k}; it; ++it) {
      env->CallVoidMethod(tripletObj, addItemMethodId, it.row(), it.col(),
                          it.value());
    }
  }

  // Find DMatrixSparseTriplet class
  static wpi::java::JClass convertCls{env, "org/ejml/ops/DConvertMatrixStruct"};
  if (!convertCls) {
    return nullptr;
  }

  // Turn DMatrixSparseTriplet into DMatrixSparseCSC
  static jmethodID convertMethodId = env->GetStaticMethodID(
      convertCls, "convert",
      "(Lorg/ejml/data/DMatrixSparseTriplet;Lorg/ejml/data/"
      "DMatrixSparseCSC;)Lorg/ejml/data/DMatrixSparseCSC;");
  jobject dmatrixObj =
      env->CallObjectMethod(convertCls, convertMethodId, tripletObj, nullptr);

  return DMatrixToSimpleMatrix(env, dmatrixObj);
}

}  // namespace frc
