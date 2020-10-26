/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <wpi/jni_util.h>

#include "Eigen/Core"
#include "Eigen/Eigenvalues"
#include "Eigen/QR"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "edu_wpi_first_math_WPIMathJNI.h"
#include "unsupported/Eigen/MatrixFunctions"

using namespace wpi::java;

bool check_stabilizable(const Eigen::Ref<const Eigen::MatrixXd>& A,
                        const Eigen::Ref<const Eigen::MatrixXd>& B) {
  // This function checks if (A,B) is a stabilizable pair.
  // (A,B) is stabilizable if and only if the uncontrollable eigenvalues of
  // A, if any, have absolute values less than one, where an eigenvalue is
  // uncontrollable if Rank[lambda * I - A, B] < n.
  int n = B.rows(), m = B.cols();
  Eigen::EigenSolver<Eigen::MatrixXd> es(A);
  for (int i = 0; i < n; i++) {
    if (es.eigenvalues()[i].real() * es.eigenvalues()[i].real() +
            es.eigenvalues()[i].imag() * es.eigenvalues()[i].imag() <
        1)
      continue;

    Eigen::MatrixXcd E(n, n + m);
    E << es.eigenvalues()[i] * Eigen::MatrixXcd::Identity(n, n) - A, B;
    Eigen::ColPivHouseholderQR<Eigen::MatrixXcd> qr(E);
    if (qr.rank() != n) {
      return false;
    }
  }

  return true;
}

extern "C" {

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    discreteAlgebraicRiccatiEquation
 * Signature: ([D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_discreteAlgebraicRiccatiEquation
  (JNIEnv* env, jclass, jdoubleArray A, jdoubleArray B, jdoubleArray Q,
   jdoubleArray R, jint states, jint inputs, jdoubleArray S)
{
  jdouble* nativeA = env->GetDoubleArrayElements(A, nullptr);
  jdouble* nativeB = env->GetDoubleArrayElements(B, nullptr);
  jdouble* nativeQ = env->GetDoubleArrayElements(Q, nullptr);
  jdouble* nativeR = env->GetDoubleArrayElements(R, nullptr);

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Amat{nativeA, states, states};
  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Bmat{nativeB, states, inputs};
  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Qmat{nativeQ, states, states};
  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Rmat{nativeR, inputs, inputs};

  Eigen::MatrixXd result =
      drake::math::DiscreteAlgebraicRiccatiEquation(Amat, Bmat, Qmat, Rmat);

  env->ReleaseDoubleArrayElements(A, nativeA, 0);
  env->ReleaseDoubleArrayElements(B, nativeB, 0);
  env->ReleaseDoubleArrayElements(Q, nativeQ, 0);
  env->ReleaseDoubleArrayElements(R, nativeR, 0);

  env->SetDoubleArrayRegion(S, 0, states * states, result.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    exp
 * Signature: ([DI[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_exp
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jdoubleArray dst)
{
  jdouble* arrayBody = env->GetDoubleArrayElements(src, nullptr);

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Amat{arrayBody, rows, rows};
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Aexp =
      Amat.exp();

  env->ReleaseDoubleArrayElements(src, arrayBody, 0);
  env->SetDoubleArrayRegion(dst, 0, rows * rows, Aexp.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    llt
 * Signature: ([DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_llt
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jint columns,
   jdoubleArray dst)
{
  jdouble* arrayBody = env->GetDoubleArrayElements(src, nullptr);

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      P{arrayBody, rows, columns};

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> U = P.llt().matrixL();

  env->ReleaseDoubleArrayElements(src, arrayBody, 0);
  env->SetDoubleArrayRegion(dst, 0, rows * columns, U.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    isStabilizable
 * Signature: (II[D[D)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_math_WPIMathJNI_isStabilizable
  (JNIEnv* env, jclass, jint states, jint inputs, jdoubleArray aSrc,
   jdoubleArray bSrc)
{
  jdouble* nativeA = env->GetDoubleArrayElements(aSrc, nullptr);
  jdouble* nativeB = env->GetDoubleArrayElements(bSrc, nullptr);

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      A{nativeA, states, states};

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      B{nativeB, states, inputs};

  bool isStabilizable = check_stabilizable(A, B);

  env->ReleaseDoubleArrayElements(aSrc, nativeA, 0);
  env->ReleaseDoubleArrayElements(bSrc, nativeB, 0);

  return isStabilizable;
}

}  // extern "C"
