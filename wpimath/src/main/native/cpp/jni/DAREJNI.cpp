// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <stdexcept>
#include <string>

#include <wpi/jni_util.h>

#include "Exceptions.h"
#include "edu_wpi_first_math_jni_DAREJNI.h"
#include "frc/DARE.h"
#include "frc/fmt/Eigen.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_math_jni_DAREJNI
 * Method:    dareNoPrecondABQR
 * Signature: ([D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_DAREJNI_dareNoPrecondABQR
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

  auto result =
      frc::DARE<Eigen::Dynamic, Eigen::Dynamic>(Amat, Bmat, Qmat, Rmat, false)
          .value();

  env->SetDoubleArrayRegion(S, 0, states * states, result.data());
}

/*
 * Class:     edu_wpi_first_math_jni_DAREJNI
 * Method:    dareNoPrecondABQRN
 * Signature: ([D[D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_DAREJNI_dareNoPrecondABQRN
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

  auto result = frc::DARE<Eigen::Dynamic, Eigen::Dynamic>(Amat, Bmat, Qmat,
                                                          Rmat, Nmat, false)
                    .value();

  env->SetDoubleArrayRegion(S, 0, states * states, result.data());
}

/*
 * Class:     edu_wpi_first_math_jni_DAREJNI
 * Method:    dareABQR
 * Signature: ([D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_DAREJNI_dareABQR
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

  if (auto result =
          frc::DARE<Eigen::Dynamic, Eigen::Dynamic>(Amat, Bmat, Qmat, Rmat)) {
    env->SetDoubleArrayRegion(S, 0, states * states, result.value().data());
    // K = (BᵀSB + R)⁻¹BᵀSA
  } else if (result.error() == frc::DAREError::QNotSymmetric ||
             result.error() == frc::DAREError::QNotPositiveSemidefinite) {
    illegalArgEx.Throw(
        env, fmt::format("{}\n\nQ =\n{}\n", to_string(result.error()), Qmat));
  } else if (result.error() == frc::DAREError::RNotSymmetric ||
             result.error() == frc::DAREError::RNotPositiveDefinite) {
    illegalArgEx.Throw(
        env, fmt::format("{}\n\nR =\n{}\n", to_string(result.error()), Rmat));
  } else if (result.error() == frc::DAREError::ABNotStabilizable) {
    illegalArgEx.Throw(env, fmt::format("{}\n\nA =\n{}\nB =\n{}\n",
                                        to_string(result.error()), Amat, Bmat));
  } else if (result.error() == frc::DAREError::ACNotDetectable) {
    illegalArgEx.Throw(env, fmt::format("{}\n\nA =\n{}\nQ =\n{}\n",
                                        to_string(result.error()), Amat, Qmat));
  }
}

/*
 * Class:     edu_wpi_first_math_jni_DAREJNI
 * Method:    dareABQRN
 * Signature: ([D[D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_jni_DAREJNI_dareABQRN
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

  if (auto result = frc::DARE<Eigen::Dynamic, Eigen::Dynamic>(Amat, Bmat, Qmat,
                                                              Rmat, Nmat)) {
    env->SetDoubleArrayRegion(S, 0, states * states, result.value().data());
  } else if (result.error() == frc::DAREError::QNotSymmetric ||
             result.error() == frc::DAREError::QNotPositiveSemidefinite) {
    illegalArgEx.Throw(
        env, fmt::format("{}\n\nQ =\n{}\n", to_string(result.error()), Qmat));
  } else if (result.error() == frc::DAREError::RNotSymmetric ||
             result.error() == frc::DAREError::RNotPositiveDefinite) {
    illegalArgEx.Throw(
        env, fmt::format("{}\n\nR =\n{}\n", to_string(result.error()), Rmat));
  } else if (result.error() == frc::DAREError::ABNotStabilizable) {
    illegalArgEx.Throw(
        env,
        fmt::format("{}\n\nA =\n{}\nB =\n{}\n", to_string(result.error()),
                    Amat - Bmat * Rmat.llt().solve(Nmat.transpose()), Bmat));
  } else if (result.error() == frc::DAREError::ACNotDetectable) {
    auto R_llt = Rmat.llt();
    illegalArgEx.Throw(
        env, fmt::format("{}\n\nA =\n{}\nQ =\n{}\n", to_string(result.error()),
                         Amat - Bmat * R_llt.solve(Nmat.transpose()),
                         Qmat - Nmat * R_llt.solve(Nmat.transpose())));
  }
}

}  // extern "C"
