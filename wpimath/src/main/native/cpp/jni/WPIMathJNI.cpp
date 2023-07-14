// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <exception>

#include <wpi/jni_util.h>

#include "Eigen/Cholesky"
#include "Eigen/Core"
#include "Eigen/Eigenvalues"
#include "Eigen/QR"
#include "edu_wpi_first_math_WPIMathJNI.h"
#include "frc/DARE.h"
#include "frc/geometry/Pose3d.h"
#include "frc/trajectory/TrajectoryUtil.h"
#include "unsupported/Eigen/MatrixFunctions"

using namespace wpi::java;

namespace {

/**
 * Returns true if (A, B) is a stabilizable pair.
 *
 * (A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is
 * uncontrollable if rank([λI - A, B]) < n where n is the number of states.
 *
 * @param A System matrix.
 * @param B Input matrix.
 */
bool IsStabilizable(const Eigen::Ref<const Eigen::MatrixXd>& A,
                    const Eigen::Ref<const Eigen::MatrixXd>& B) {
  Eigen::EigenSolver<Eigen::MatrixXd> es{A, false};

  for (int i = 0; i < A.rows(); ++i) {
    if (es.eigenvalues()[i].real() * es.eigenvalues()[i].real() +
            es.eigenvalues()[i].imag() * es.eigenvalues()[i].imag() <
        1) {
      continue;
    }

    Eigen::MatrixXcd E{A.rows(), A.rows() + B.cols()};
    E << es.eigenvalues()[i] * Eigen::MatrixXcd::Identity(A.rows(), A.rows()) -
             A,
        B;

    Eigen::ColPivHouseholderQR<Eigen::MatrixXcd> qr{E};
    if (qr.rank() < A.rows()) {
      return false;
    }
  }
  return true;
}

}  // namespace

static JClass pose3dCls;
static JClass quaternionCls;
static JClass rotation3dCls;
static JClass translation3dCls;
static JClass twist3dCls;

static const JClassInit classes[] = {
    {"edu/wpi/first/math/geometry/Pose3d", &pose3dCls},
    {"edu/wpi/first/math/geometry/Quaternion", &quaternionCls},
    {"edu/wpi/first/math/geometry/Rotation3d", &rotation3dCls},
    {"edu/wpi/first/math/geometry/Translation3d", &translation3dCls},
    {"edu/wpi/first/math/geometry/Twist3d", &twist3dCls}};

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  for (auto& c : classes) {
    *c.cls = JClass(env, c.name);
    if (!*c.cls) {
      return JNI_ERR;
    }
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }

  for (auto& c : classes) {
    c.cls->free(env);
  }
}

}  // extern "C"

frc::Pose3d FromJavaPose3d(JNIEnv* env, jobject pose) {
  static jfieldID poseTranslationField = nullptr;
  static jfieldID poseRotationField = nullptr;
  static jfieldID translationXField = nullptr;
  static jfieldID translationYField = nullptr;
  static jfieldID translationZField = nullptr;
  static jfieldID rotationQuaternionField = nullptr;
  static jfieldID quaternionWField = nullptr;
  static jmethodID quaternionXMethod = nullptr;
  static jmethodID quaternionYMethod = nullptr;
  static jmethodID quaternionZMethod = nullptr;

  if (!poseTranslationField) {
    poseTranslationField =
        env->GetFieldID(pose3dCls, "m_translation",
                        "Ledu/wpi/first/math/geometry/Translation3d;");
  }
  if (!poseRotationField) {
    poseRotationField = env->GetFieldID(
        pose3dCls, "m_rotation", "Ledu/wpi/first/math/geometry/Rotation3d;");
  }
  if (!translationXField) {
    translationXField = env->GetFieldID(translation3dCls, "m_x", "D");
  }
  if (!translationYField) {
    translationYField = env->GetFieldID(translation3dCls, "m_y", "D");
  }
  if (!translationZField) {
    translationZField = env->GetFieldID(translation3dCls, "m_z", "D");
  }
  if (!rotationQuaternionField) {
    rotationQuaternionField = env->GetFieldID(
        rotation3dCls, "m_q", "Ledu/wpi/first/math/geometry/Quaternion;");
  }
  if (!quaternionWField) {
    quaternionWField = env->GetFieldID(quaternionCls, "m_r", "D");
  }
  if (!quaternionXMethod) {
    quaternionXMethod = env->GetMethodID(quaternionCls, "getX", "()D");
  }
  if (!quaternionYMethod) {
    quaternionYMethod = env->GetMethodID(quaternionCls, "getY", "()D");
  }
  if (!quaternionZMethod) {
    quaternionZMethod = env->GetMethodID(quaternionCls, "getZ", "()D");
  }

  jobject translation = env->GetObjectField(pose, poseTranslationField);
  jdouble dx = env->GetDoubleField(translation, translationXField);
  jdouble dy = env->GetDoubleField(translation, translationYField);
  jdouble dz = env->GetDoubleField(translation, translationZField);
  jobject rotation = env->GetObjectField(pose, poseRotationField);
  jobject quaternion = env->GetObjectField(rotation, rotationQuaternionField);
  jdouble qw = env->GetDoubleField(quaternion, quaternionWField);
  jdouble qx = env->CallDoubleMethod(quaternion, quaternionXMethod);
  jdouble qy = env->CallDoubleMethod(quaternion, quaternionYMethod);
  jdouble qz = env->CallDoubleMethod(quaternion, quaternionZMethod);

  return {units::meter_t{dx}, units::meter_t{dy}, units::meter_t{dz},
          frc::Rotation3d{frc::Quaternion{qw, qx, qy, qz}}};
}

frc::Twist3d FromJavaTwist3d(JNIEnv* env, jobject twist) {
  static jfieldID twistDxField = nullptr;
  static jfieldID twistDyField = nullptr;
  static jfieldID twistDzField = nullptr;
  static jfieldID twistRxField = nullptr;
  static jfieldID twistRyField = nullptr;
  static jfieldID twistRzField = nullptr;

  if (!twistDxField) {
    twistDxField = env->GetFieldID(twist3dCls, "dx", "D");
  }
  if (!twistDyField) {
    twistDyField = env->GetFieldID(twist3dCls, "dy", "D");
  }
  if (!twistDzField) {
    twistDzField = env->GetFieldID(twist3dCls, "dz", "D");
  }
  if (!twistRxField) {
    twistRxField = env->GetFieldID(twist3dCls, "rx", "D");
  }
  if (!twistRyField) {
    twistRyField = env->GetFieldID(twist3dCls, "ry", "D");
  }
  if (!twistRzField) {
    twistRzField = env->GetFieldID(twist3dCls, "rz", "D");
  }

  jdouble twistDx = env->GetDoubleField(twist, twistDxField);
  jdouble twistDy = env->GetDoubleField(twist, twistDyField);
  jdouble twistDz = env->GetDoubleField(twist, twistDzField);
  jdouble twistRx = env->GetDoubleField(twist, twistRxField);
  jdouble twistRy = env->GetDoubleField(twist, twistRyField);
  jdouble twistRz = env->GetDoubleField(twist, twistRzField);

  return {units::meter_t{twistDx},  units::meter_t{twistDy},
          units::meter_t{twistDz},  units::radian_t{twistRx},
          units::radian_t{twistRy}, units::radian_t{twistRz}};
}

jobject MakeJObject(JNIEnv* env, const frc::Pose3d& pose) {
  static jmethodID pose3dCtor = nullptr;
  static jmethodID quaternionCtor = nullptr;
  static jmethodID rotation3dCtor = nullptr;
  static jmethodID translation3dCtor = nullptr;

  if (!pose3dCtor) {
    pose3dCtor =
        env->GetMethodID(pose3dCls, "<init>",
                         "(Ledu/wpi/first/math/geometry/Translation3d;Ledu/wpi/"
                         "first/math/geometry/Rotation3d;)V");
  }
  if (!quaternionCtor) {
    quaternionCtor = env->GetMethodID(quaternionCls, "<init>", "(DDDD)V");
  }
  if (!rotation3dCtor) {
    rotation3dCtor = env->GetMethodID(
        rotation3dCls, "<init>", "(Ledu/wpi/first/math/geometry/Quaternion;)V");
  }
  if (!translation3dCtor) {
    translation3dCtor = env->GetMethodID(translation3dCls, "<init>", "(DDD)V");
  }

  frc::Quaternion quaternion = pose.Rotation().GetQuaternion();

  double dx = pose.X().value();
  double dy = pose.Y().value();
  double dz = pose.Z().value();
  double qw = quaternion.W();
  double qx = quaternion.X();
  double qy = quaternion.Y();
  double qz = quaternion.Z();

  jobject jTranslation =
      env->NewObject(translation3dCls, translation3dCtor, dx, dy, dz);
  jobject jQuaternion =
      env->NewObject(quaternionCls, quaternionCtor, qw, qx, qy, qz);
  jobject jRotation =
      env->NewObject(rotation3dCls, rotation3dCtor, jQuaternion);
  jobject jPose =
      env->NewObject(pose3dCls, pose3dCtor, jTranslation, jRotation);

  return jPose;
}

jobject MakeJObject(JNIEnv* env, const frc::Twist3d& twist) {
  static jmethodID twist3dCtor = nullptr;
  if (!twist3dCtor) {
    twist3dCtor = env->GetMethodID(twist3dCls, "<init>", "(DDDDDD)V");
  }

  return env->NewObject(twist3dCls, twist3dCtor, twist.dx.value(),
                        twist.dy.value(), twist.dz.value(), twist.rx.value(),
                        twist.ry.value(), twist.rz.value());
}

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
 * Method:    dare
 * Signature: ([D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_dare
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

  try {
    Eigen::MatrixXd result = frc::DARE(Amat, Bmat, Qmat, Rmat);

    env->ReleaseDoubleArrayElements(A, nativeA, 0);
    env->ReleaseDoubleArrayElements(B, nativeB, 0);
    env->ReleaseDoubleArrayElements(Q, nativeQ, 0);
    env->ReleaseDoubleArrayElements(R, nativeR, 0);

    env->SetDoubleArrayRegion(S, 0, states * states, result.data());
  } catch (const std::runtime_error& e) {
    jclass cls = env->FindClass("java/lang/RuntimeException");
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
 * Method:    pow
 * Signature: ([DID[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_WPIMathJNI_pow
  (JNIEnv* env, jclass, jdoubleArray src, jint rows, jdouble exponent,
   jdoubleArray dst)
{
  jdouble* arrayBody = env->GetDoubleArrayElements(src, nullptr);

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Amat{arrayBody, rows, rows};  // NOLINT
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Apow =
      Amat.pow(exponent);

  env->ReleaseDoubleArrayElements(src, arrayBody, 0);
  env->SetDoubleArrayRegion(dst, 0, rows * rows, Apow.data());
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    expPose3d
 * Signature: (Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_math_WPIMathJNI_expPose3d
  (JNIEnv* env, jclass, jobject startPose, jobject twist)
{
  frc::Pose3d startPoseCpp = FromJavaPose3d(env, startPose);
  frc::Twist3d twistCpp = FromJavaTwist3d(env, twist);

  frc::Pose3d result = startPoseCpp.Exp(twistCpp);

  return MakeJObject(env, result);
}

/*
 * Class:     edu_wpi_first_math_WPIMathJNI
 * Method:    logPose3d
 * Signature: (Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_math_WPIMathJNI_logPose3d
  (JNIEnv* env, jclass, jobject start, jobject end)
{
  frc::Pose3d startCpp = FromJavaPose3d(env, start);
  frc::Pose3d endCpp = FromJavaPose3d(env, end);

  frc::Twist3d result = startCpp.Log(endCpp);

  return MakeJObject(env, result);
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

  bool isStabilizable = IsStabilizable(A, B);

  env->ReleaseDoubleArrayElements(aSrc, nativeA, 0);
  env->ReleaseDoubleArrayElements(bSrc, nativeB, 0);

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
        CreateTrajectoryFromElements(JDoubleArrayRef{env, elements});
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
        CreateTrajectoryFromElements(JDoubleArrayRef{env, elements});
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
  jdouble* matBody = env->GetDoubleArrayElements(mat, nullptr);
  jdouble* vecBody = env->GetDoubleArrayElements(vec, nullptr);

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      L{matBody, rows, rows};
  Eigen::Map<Eigen::Vector<double, Eigen::Dynamic>> v{vecBody, rows};

  if (lowerTriangular == JNI_TRUE) {
    Eigen::internal::llt_inplace<double, Eigen::Lower>::rankUpdate(L, v, sigma);
  } else {
    Eigen::internal::llt_inplace<double, Eigen::Upper>::rankUpdate(L, v, sigma);
  }

  env->ReleaseDoubleArrayElements(mat, matBody, 0);
  env->ReleaseDoubleArrayElements(vec, vecBody, 0);
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
  jdouble* nativeA = env->GetDoubleArrayElements(A, nullptr);
  jdouble* nativeB = env->GetDoubleArrayElements(B, nullptr);

  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Amat{nativeA, Arows, Acols};
  Eigen::Map<
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Bmat{nativeB, Brows, Bcols};

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Xmat =
      Amat.fullPivHouseholderQr().solve(Bmat);

  env->ReleaseDoubleArrayElements(A, nativeA, 0);
  env->ReleaseDoubleArrayElements(B, nativeB, 0);
  env->SetDoubleArrayRegion(dst, 0, Brows * Bcols, Xmat.data());
}

}  // extern "C"
