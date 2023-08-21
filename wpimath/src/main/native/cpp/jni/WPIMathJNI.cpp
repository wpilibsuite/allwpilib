// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <exception>
#include <stdexcept>

#include <wpi/jni_util.h>

#include "Eigen/Cholesky"
#include "Eigen/Core"
#include "Eigen/QR"
#include "edu_wpi_first_math_WPIMathJNI.h"
#include "frc/DARE.h"
#include "frc/geometry/Pose3d.h"
#include "frc/trajectory/TrajectoryUtil.h"
#include "unsupported/Eigen/MatrixFunctions"

using namespace wpi::java;

std::vector<double> GetElementsFromTrajectory(
    const frc::Trajectory& trajectory) {
  std::vector<double> elements;
  elements.reserve(trajectory.States().size() * 7);

  for (auto&& state : trajectory.States()) {
    elements.push_back(state.t.value());
    elements.push_back(state.velocity.value());
    elements.push_back(state.acceleration.value());
    elements.push_back(state.pose.X().value());
    elements.push_back(state.pose.Y().value());
    elements.push_back(state.pose.Rotation().Radians().value());
    elements.push_back(state.curvature.value());
  }

  return elements;
}

frc::Trajectory CreateTrajectoryFromElements(std::span<const double> elements) {
  // Make sure that the elements have the correct length.
  assert(elements.size() % 7 == 0);

  // Create a vector of states from the elements.
  std::vector<frc::Trajectory::State> states;
  states.reserve(elements.size() / 7);

  for (size_t i = 0; i < elements.size(); i += 7) {
    states.emplace_back(frc::Trajectory::State{
        units::second_t{elements[i]},
        units::meters_per_second_t{elements[i + 1]},
        units::meters_per_second_squared_t{elements[i + 2]},
        frc::Pose2d{units::meter_t{elements[i + 3]},
                    units::meter_t{elements[i + 4]},
                    units::radian_t{elements[i + 5]}},
        units::curvature_t{elements[i + 6]}});
  }

  return frc::Trajectory(states);
}

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
    jclass cls = env->FindClass("java/lang/IllegalArgumentException");
    if (cls) {
      env->ThrowNew(cls, e.what());
    }
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
    jclass cls = env->FindClass("java/lang/IllegalArgumentException");
    if (cls) {
      env->ThrowNew(cls, e.what());
    }
  }
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
  JSpan<const jdouble> arrayBody{env, src};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{arrayBody.data(), rows, rows};
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Aexp =
      Amat.exp();

  env->SetDoubleArrayRegion(dst, 0, rows * rows, Aexp.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    pow
 * Signature: ([DID[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_pow
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jdouble exponent,
   jdoubleArray dst)
{
  JSpan<const jdouble> arrayBody{env, src};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      Amat{arrayBody.data(), rows, rows};  // NOLINT
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Apow =
      Amat.pow(exponent);

  env->SetDoubleArrayRegion(dst, 0, rows * rows, Apow.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    expPose3d
 * Signature: (DDDDDDDDDDDDD)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_wpi_first_math_WPIMathJNI_expPose3d
  (JNIEnv* env, jclass, jdouble poseX, jdouble poseY, jdouble poseZ,
   jdouble poseQw, jdouble poseQx, jdouble poseQy, jdouble poseQz,
   jdouble twistDx, jdouble twistDy, jdouble twistDz, jdouble twistRx,
   jdouble twistRy, jdouble twistRz)
{
  frc::Pose3d pose{
      units::meter_t{poseX}, units::meter_t{poseY}, units::meter_t{poseZ},
      frc::Rotation3d{frc::Quaternion{poseQw, poseQx, poseQy, poseQz}}};
  frc::Twist3d twist{units::meter_t{twistDx},  units::meter_t{twistDy},
                     units::meter_t{twistDz},  units::radian_t{twistRx},
                     units::radian_t{twistRy}, units::radian_t{twistRz}};

  frc::Pose3d result = pose.Exp(twist);

  const auto& resultQuaternion = result.Rotation().GetQuaternion();
  return MakeJDoubleArray(
      env, {{result.X().value(), result.Y().value(), result.Z().value(),
             resultQuaternion.W(), resultQuaternion.X(), resultQuaternion.Y(),
             resultQuaternion.Z()}});
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    logPose3d
 * Signature: (DDDDDDDDDDDDDD)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_wpi_first_math_WPIMathJNI_logPose3d
  (JNIEnv* env, jclass, jdouble startX, jdouble startY, jdouble startZ,
   jdouble startQw, jdouble startQx, jdouble startQy, jdouble startQz,
   jdouble endX, jdouble endY, jdouble endZ, jdouble endQw, jdouble endQx,
   jdouble endQy, jdouble endQz)
{
  frc::Pose3d startPose{
      units::meter_t{startX}, units::meter_t{startY}, units::meter_t{startZ},
      frc::Rotation3d{frc::Quaternion{startQw, startQx, startQy, startQz}}};
  frc::Pose3d endPose{
      units::meter_t{endX}, units::meter_t{endY}, units::meter_t{endZ},
      frc::Rotation3d{frc::Quaternion{endQw, endQx, endQy, endQz}}};

  frc::Twist3d result = startPose.Log(endPose);

  return MakeJDoubleArray(
      env, {{result.dx.value(), result.dy.value(), result.dz.value(),
             result.rx.value(), result.ry.value(), result.rz.value()}});
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
  JSpan<const jdouble> nativeA{env, aSrc};
  JSpan<const jdouble> nativeB{env, bSrc};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      A{nativeA.data(), states, states};

  Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                 Eigen::RowMajor>>
      B{nativeB.data(), states, inputs};

  bool isStabilizable =
      frc::IsStabilizable<Eigen::Dynamic, Eigen::Dynamic>(A, B);

  return isStabilizable;
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    fromPathweaverJson
 * Signature: (Ljava/lang/String;)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_wpi_first_math_WPIMathJNI_fromPathweaverJson
  (JNIEnv* env, jclass, jstring path)
{
  try {
    auto trajectory =
        frc::TrajectoryUtil::FromPathweaverJson(JStringRef{env, path}.c_str());
    std::vector<double> elements = GetElementsFromTrajectory(trajectory);
    return MakeJDoubleArray(env, elements);
  } catch (std::exception& e) {
    jclass cls = env->FindClass("java/io/IOException");
    if (cls) {
      env->ThrowNew(cls, e.what());
    }
    return nullptr;
  }
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    toPathweaverJson
 * Signature: ([DLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_toPathweaverJson
  (JNIEnv* env, jclass, jdoubleArray elements, jstring path)
{
  try {
    auto trajectory =
        CreateTrajectoryFromElements(JSpan<const jdouble>{env, elements});
    frc::TrajectoryUtil::ToPathweaverJson(trajectory,
                                          JStringRef{env, path}.c_str());
  } catch (std::exception& e) {
    jclass cls = env->FindClass("java/io/IOException");
    if (cls) {
      env->ThrowNew(cls, e.what());
    }
  }
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    deserializeTrajectory
 * Signature: (Ljava/lang/String;)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_wpi_first_math_WPIMathJNI_deserializeTrajectory
  (JNIEnv* env, jclass, jstring json)
{
  try {
    auto trajectory = frc::TrajectoryUtil::DeserializeTrajectory(
        JStringRef{env, json}.c_str());
    std::vector<double> elements = GetElementsFromTrajectory(trajectory);
    return MakeJDoubleArray(env, elements);
  } catch (std::exception& e) {
    jclass cls = env->FindClass(
        "edu/wpi/first/math/trajectory/TrajectoryUtil$"
        "TrajectorySerializationException");
    if (cls) {
      env->ThrowNew(cls, e.what());
    }
    return nullptr;
  }
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    serializeTrajectory
 * Signature: ([D)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_math_WPIMathJNI_serializeTrajectory
  (JNIEnv* env, jclass, jdoubleArray elements)
{
  try {
    auto trajectory =
        CreateTrajectoryFromElements(JSpan<const jdouble>{env, elements});
    return MakeJString(env,
                       frc::TrajectoryUtil::SerializeTrajectory(trajectory));
  } catch (std::exception& e) {
    jclass cls = env->FindClass(
        "edu/wpi/first/math/trajectory/TrajectoryUtil$"
        "TrajectorySerializationException");
    if (cls) {
      env->ThrowNew(cls, e.what());
    }
    return nullptr;
  }
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    rankUpdate
 * Signature: ([DI[DDZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_rankUpdate
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
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    solveFullPivHouseholderQr
 * Signature: ([DII[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_solveFullPivHouseholderQr
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
