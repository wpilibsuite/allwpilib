// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/QR>
#include <unsupported/Eigen/MatrixFunctions>
#include <wpi/jni_util.h>

#include "edu_wpi_first_math_jni_EigenJNI.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_jni_EigenJNI
 * Method:    exp
 * Signature: ([DI[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_EigenJNI_exp
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jdoubleArray dst)
{
  JSpan<const jdouble> arrayBody{env, src};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{arrayBody.data(), rows, rows};
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Aexp =
      Amat.exp();

  env->SetDoubleArrayRegion(dst, 0, rows * rows, Aexp.data());
}

/*
 * Class:     edu_wpi_first_math_jni_EigenJNI
 * Method:    pow
 * Signature: ([DID[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_EigenJNI_pow
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jdouble exponent,
   jdoubleArray dst)
{
  JSpan<const jdouble> arrayBody{env, src};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{arrayBody.data(), rows, rows};
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Apow =
      Amat.pow(exponent);

  env->SetDoubleArrayRegion(dst, 0, rows * rows, Apow.data());
}

/*
 * Class:     edu_wpi_first_math_jni_EigenJNI
 * Method:    rankUpdate
 * Signature: ([DI[DDZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_EigenJNI_rankUpdate
  (JNIEnv* env, jclass, jdoubleArray mat, jint rows, jdoubleArray vec,
   jdouble sigma, jboolean lowerTriangular)
{
  JSpan<jdouble> matBody{env, mat};
  JSpan<const jdouble> vecBody{env, vec};

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      L{matBody.data(), rows, rows};
  Eigen::Map<const Eigen::Vector<double, Eigen::Dynamic>> v{vecBody.data(),
                                                            rows};

  if (lowerTriangular == JNI_TRUE) {
    Eigen::internal::llt_inplace<double, Eigen::Lower>::rankUpdate(L, v, sigma);
  } else {
    Eigen::internal::llt_inplace<double, Eigen::Upper>::rankUpdate(L, v, sigma);
  }
}

/*
 * Class:     edu_wpi_first_math_jni_EigenJNI
 * Method:    solveFullPivHouseholderQr
 * Signature: ([DII[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_EigenJNI_solveFullPivHouseholderQr
  (JNIEnv* env, jclass, jdoubleArray A, jint Arows, jint Acols, jdoubleArray B,
   jint Brows, jint Bcols, jdoubleArray dst)
{
  JSpan<const jdouble> nativeA{env, A};
  JSpan<const jdouble> nativeB{env, B};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{nativeA.data(), Arows, Acols};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Bmat{nativeB.data(), Brows, Bcols};

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Xmat =
      Amat.fullPivHouseholderQr().solve(Bmat);

  env->SetDoubleArrayRegion(dst, 0, Brows * Bcols, Xmat.data());
}

}  // extern "C"
