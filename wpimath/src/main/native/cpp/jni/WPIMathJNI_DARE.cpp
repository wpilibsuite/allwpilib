// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <stdexcept>

#include <wpi/jni_util.h>

#include "WPIMathJNI_Exceptions.h"
#include "edu_wpi_first_math_WPIMathJNI.h"
#include "frc/DARE.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    dareDetailABQR
 * Signature: ([D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_dareDetailABQR
  (JNIEnv* env, jclass, jdoubleArray A, jdoubleArray B, jdoubleArray Q,
   jdoubleArray R, jint states, jint inputs, jdoubleArray S)
{
  JSpan<const jdouble> nativeA{env, A};
  JSpan<const jdouble> nativeB{env, B};
  JSpan<const jdouble> nativeQ{env, Q};
  JSpan<const jdouble> nativeR{env, R};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{nativeA.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Bmat{nativeB.data(), states, inputs};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Qmat{nativeQ.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Rmat{nativeR.data(), inputs, inputs};

  Eigen::MatrixXd RmatCopy{Rmat};
  auto R_llt = RmatCopy.llt();

  Eigen::MatrixXd result = frc::detail::DARE<Eigen::Dynamic, Eigen::Dynamic>(
      Amat, Bmat, Qmat, R_llt);

  env->SetDoubleArrayRegion(S, 0, states * states, result.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    dareDetailABQRN
 * Signature: ([D[D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_dareDetailABQRN
  (JNIEnv* env, jclass, jdoubleArray A, jdoubleArray B, jdoubleArray Q,
   jdoubleArray R, jdoubleArray N, jint states, jint inputs, jdoubleArray S)
{
  JSpan<const jdouble> nativeA{env, A};
  JSpan<const jdouble> nativeB{env, B};
  JSpan<const jdouble> nativeQ{env, Q};
  JSpan<const jdouble> nativeR{env, R};
  JSpan<const jdouble> nativeN{env, N};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{nativeA.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Bmat{nativeB.data(), states, inputs};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Qmat{nativeQ.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Rmat{nativeR.data(), inputs, inputs};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Nmat{nativeN.data(), states, inputs};

  Eigen::MatrixXd Rcopy{Rmat};
  auto R_llt = Rcopy.llt();

  Eigen::MatrixXd result = frc::detail::DARE<Eigen::Dynamic, Eigen::Dynamic>(
      Amat, Bmat, Qmat, R_llt, Nmat);

  env->SetDoubleArrayRegion(S, 0, states * states, result.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    dareABQR
 * Signature: ([D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_dareABQR
  (JNIEnv* env, jclass, jdoubleArray A, jdoubleArray B, jdoubleArray Q,
   jdoubleArray R, jint states, jint inputs, jdoubleArray S)
{
  JSpan<const jdouble> nativeA{env, A};
  JSpan<const jdouble> nativeB{env, B};
  JSpan<const jdouble> nativeQ{env, Q};
  JSpan<const jdouble> nativeR{env, R};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{nativeA.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Bmat{nativeB.data(), states, inputs};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Qmat{nativeQ.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Rmat{nativeR.data(), inputs, inputs};

  try {
    Eigen::MatrixXd result =
        frc::DARE<Eigen::Dynamic, Eigen::Dynamic>(Amat, Bmat, Qmat, Rmat);

    env->SetDoubleArrayRegion(S, 0, states * states, result.data());
  } catch (const std::invalid_argument& e) {
    illegalArgEx.Throw(env, e.what());
  }
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    dareABQRN
 * Signature: ([D[D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_dareABQRN
  (JNIEnv* env, jclass, jdoubleArray A, jdoubleArray B, jdoubleArray Q,
   jdoubleArray R, jdoubleArray N, jint states, jint inputs, jdoubleArray S)
{
  JSpan<const jdouble> nativeA{env, A};
  JSpan<const jdouble> nativeB{env, B};
  JSpan<const jdouble> nativeQ{env, Q};
  JSpan<const jdouble> nativeR{env, R};
  JSpan<const jdouble> nativeN{env, N};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{nativeA.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Bmat{nativeB.data(), states, inputs};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Qmat{nativeQ.data(), states, states};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Rmat{nativeR.data(), inputs, inputs};
  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Nmat{nativeN.data(), states, inputs};

  try {
    Eigen::MatrixXd result =
        frc::DARE<Eigen::Dynamic, Eigen::Dynamic>(Amat, Bmat, Qmat, Rmat, Nmat);

    env->SetDoubleArrayRegion(S, 0, states * states, result.data());
  } catch (const std::invalid_argument& e) {
    illegalArgEx.Throw(env, e.what());
  }
}

}  // extern "C"
