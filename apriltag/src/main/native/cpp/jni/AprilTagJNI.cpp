// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <cstring>

#include <wpi/jni_util.h>

#include "edu_wpi_first_apriltag_jni_AprilTagJNI.h"
#include "frc/apriltag/AprilTagDetector.h"

#ifdef _WIN32
#pragma warning(disable : 4200)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "apriltag.h"

using namespace frc;
using namespace wpi::java;

static JavaVM* jvm = nullptr;

static JClass detectionCls;
static JClass poseEstimateCls;
static JClass quaternionCls;
static JClass rotation3dCls;
static JClass transform3dCls;
static JClass translation3dCls;
static JException illegalArgEx;
static JException nullPointerEx;

static const JClassInit classes[] = {
    {"edu/wpi/first/apriltag/AprilTagDetection", &detectionCls},
    {"edu/wpi/first/apriltag/AprilTagDetection$PoseEstimate", &poseEstimateCls},
    {"edu/wpi/first/math/geometry/Quaternion", &quaternionCls},
    {"edu/wpi/first/math/geometry/Rotation3d", &rotation3dCls},
    {"edu/wpi/first/math/geometry/Transform3d", &transform3dCls},
    {"edu/wpi/first/math/geometry/Translation3d", &translation3dCls}};

static const JExceptionInit exceptions[] = {
    {"java/lang/IllegalArgumentException", &illegalArgEx},
    {"java/lang/NullPointerException", &nullPointerEx}};

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  // Cache references to classes
  for (auto& c : classes) {
    *c.cls = JClass(env, c.name);
    if (!*c.cls) {
      std::fprintf(stderr, "could not load class %s\n", c.name);
      return JNI_ERR;
    }
  }

  for (auto& c : exceptions) {
    *c.cls = JException(env, c.name);
    if (!*c.cls) {
      std::fprintf(stderr, "could not load exception %s\n", c.name);
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
  // Delete global references
  for (auto& c : classes) {
    c.cls->free(env);
  }
  for (auto& c : exceptions) {
    c.cls->free(env);
  }
  jvm = nullptr;
}

}  // extern "C"

//
// Conversions from Java objects to C++
//

static matd_t* FromJavaHomography(JNIEnv* env, jdoubleArray homography) {
  if (!homography) {
    nullPointerEx.Throw(env, "homography cannot be null");
    return nullptr;
  }

  JDoubleArrayRef harr{env, homography};
  if (harr.size() != 9) {
    illegalArgEx.Throw(env, "homography array must be size 9");
    return nullptr;
  }
  matd_t* hmat = matd_create(3, 3);
  std::memcpy(hmat->data, harr.array().data(), 9 * sizeof(double));
  return hmat;
}

static bool FromJavaCorners(JNIEnv* env, jdoubleArray corners,
                            apriltag_detection_t* det) {
  if (!corners) {
    nullPointerEx.Throw(env, "corners cannot be null");
    return false;
  }

  JDoubleArrayRef carr{env, corners};
  if (carr.size() != 8) {
    illegalArgEx.Throw(env, "corners array must be size 8");
    return false;
  }

  auto arr = carr.array();
  for (int i = 0; i < 4; i++) {
    det->p[i][0] = arr[i * 2];
    det->p[i][1] = arr[i * 2 + 1];
  }
  return true;
}

//
// Conversions from C++ to Java objects
//

static jobject MakeJObject(JNIEnv* env, const AprilTagDetection& detect) {
  static jmethodID constructor = env->GetMethodID(
      detectionCls, "<init>", "(Ljava/lang/String;IIF[DDD[D)V");
  if (!constructor) {
    return nullptr;
  }

  JLocal<jstring> fam{env, MakeJString(env, detect.GetFamily())};

  // we need the raw data structure for maximum efficiency
  auto detectc = reinterpret_cast<const apriltag_detection_t*>(&detect);
  JLocal<jdoubleArray> harr{
      env, MakeJDoubleArray(
               env, {reinterpret_cast<const jdouble*>(detectc->H->data), 9})};

  jdouble corners[8];
  for (int i = 0; i < 4; i++) {
    corners[i * 2] = detectc->p[i][0];
    corners[i * 2 + 1] = detectc->p[i][1];
  }
  JLocal<jdoubleArray> carr{env, MakeJDoubleArray(env, corners)};

  return env->NewObject(detectionCls, constructor, fam.obj(),
                        static_cast<jint>(detect.GetId()),
                        static_cast<jint>(detect.GetHamming()),
                        static_cast<jfloat>(detect.GetDecisionMargin()),
                        harr.obj(), static_cast<jdouble>(detectc->c[0]),
                        static_cast<jdouble>(detectc->c[1]), carr.obj());
}

static jobjectArray MakeJObject(JNIEnv* env,
                                std::span<const AprilTagDetection* const> arr) {
  jobjectArray jarr = env->NewObjectArray(arr.size(), detectionCls, nullptr);
  if (!jarr) {
    return nullptr;
  }
  for (size_t i = 0; i < arr.size(); ++i) {
    JLocal<jobject> elem{env, MakeJObject(env, *arr[i])};
    env->SetObjectArrayElement(jarr, i, elem.obj());
  }
  return jarr;
}

static jobject MakeJObject(JNIEnv* env, const Translation3d& xlate) {
  static jmethodID constructor =
      env->GetMethodID(translation3dCls, "<init>", "(DDD)V");
  if (!constructor) {
    return nullptr;
  }

  return env->NewObject(
      translation3dCls, constructor, static_cast<jdouble>(xlate.X()),
      static_cast<jdouble>(xlate.Y()), static_cast<jdouble>(xlate.Z()));
}

static jobject MakeJObject(JNIEnv* env, const Quaternion& q) {
  static jmethodID constructor =
      env->GetMethodID(quaternionCls, "<init>", "(DDDD)V");
  if (!constructor) {
    return nullptr;
  }

  return env->NewObject(quaternionCls, constructor, static_cast<jdouble>(q.W()),
                        static_cast<jdouble>(q.X()),
                        static_cast<jdouble>(q.Y()),
                        static_cast<jdouble>(q.Z()));
}

static jobject MakeJObject(JNIEnv* env, const Rotation3d& rot) {
  static jmethodID constructor = env->GetMethodID(
      rotation3dCls, "<init>", "(Ledu/wpi/first/math/geometry/Quaternion;)V");
  if (!constructor) {
    return nullptr;
  }

  JLocal<jobject> q{env, MakeJObject(env, rot.GetQuaternion())};
  return env->NewObject(rotation3dCls, constructor, q.obj());
}

static jobject MakeJObject(JNIEnv* env, const Transform3d& xform) {
  static jmethodID constructor =
      env->GetMethodID(transform3dCls, "<init>",
                       "(Ledu/wpi/first/math/geometry/Translation3d;"
                       "Ledu/wpi/first/math/geometry/Rotation3d;)V");
  if (!constructor) {
    return nullptr;
  }

  JLocal<jobject> xlate{env, MakeJObject(env, xform.Translation())};
  JLocal<jobject> rot{env, MakeJObject(env, xform.Rotation())};
  return env->NewObject(transform3dCls, constructor, xlate.obj(), rot.obj());
}

static jobject MakeJObject(JNIEnv* env,
                           const AprilTagDetection::PoseEstimate& est) {
  static jmethodID constructor =
      env->GetMethodID(poseEstimateCls, "<init>",
                       "(Ledu/wpi/first/math/geometry/Transform3d;"
                       "Ledu/wpi/first/math/geometry/Transform3d;DD)V");
  if (!constructor) {
    return nullptr;
  }

  JLocal<jobject> pose1{env, MakeJObject(env, est.pose1)};
  JLocal<jobject> pose2{env, MakeJObject(env, est.pose2)};
  return env->NewObject(poseEstimateCls, constructor, pose1.obj(), pose2.obj(),
                        static_cast<jdouble>(est.error1),
                        static_cast<jdouble>(est.error2));
}

extern "C" {

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    createDetector
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_createDetector
  (JNIEnv* env, jclass)
{
  return reinterpret_cast<jlong>(new AprilTagDetector);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    destroyDetector
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_destroyDetector
  (JNIEnv* env, jclass, jlong det)
{
  delete reinterpret_cast<AprilTagDetector*>(det);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setNumThreads
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setNumThreads
  (JNIEnv* env, jclass, jlong det, jint val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetNumThreads(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getNumThreads
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getNumThreads
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetNumThreads();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadDecimate
 * Signature: (JF)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadDecimate
  (JNIEnv* env, jclass, jlong det, jfloat val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadDecimate(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadDecimate
 * Signature: (J)F
 */
JNIEXPORT jfloat JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadDecimate
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetQuadDecimate();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadSigma
 * Signature: (JF)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadSigma
  (JNIEnv* env, jclass, jlong det, jfloat val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadSigma(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadSigma
 * Signature: (J)F
 */
JNIEXPORT jfloat JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadSigma
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetQuadSigma();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setRefineEdges
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setRefineEdges
  (JNIEnv* env, jclass, jlong det, jboolean val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetRefineEdges(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getRefineEdges
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getRefineEdges
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return false;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetRefineEdges();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setDecodeSharpening
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setDecodeSharpening
  (JNIEnv* env, jclass, jlong det, jdouble val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetDecodeSharpening(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getDecodeSharpening
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getDecodeSharpening
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetDecodeSharpening();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setDebug
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setDebug
  (JNIEnv* env, jclass, jlong det, jboolean val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetDebug(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getDebug
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getDebug
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return false;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetDebug();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadMinClusterPixels
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadMinClusterPixels
  (JNIEnv* env, jclass, jlong det, jint val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadMinClusterPixels(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadMinClusterPixels
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadMinClusterPixels
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetQuadMinClusterPixels();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadMaxNumMaxima
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadMaxNumMaxima
  (JNIEnv* env, jclass, jlong det, jint val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadMaxNumMaxima(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadMaxNumMaxima
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadMaxNumMaxima
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetQuadMaxNumMaxima();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadCriticalAngle
 * Signature: (JF)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadCriticalAngle
  (JNIEnv* env, jclass, jlong det, jfloat val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadCriticalAngle(
      units::radian_t{val});
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadCriticalAngle
 * Signature: (J)F
 */
JNIEXPORT jfloat JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadCriticalAngle
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)
      ->GetQuadCriticalAngle()
      .value();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadMaxLineFitMSE
 * Signature: (JF)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadMaxLineFitMSE
  (JNIEnv* env, jclass, jlong det, jfloat val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadMaxLineFitMSE(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadMaxLineFitMSE
 * Signature: (J)F
 */
JNIEXPORT jfloat JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadMaxLineFitMSE
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetQuadMaxLineFitMSE();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadMinWhiteBlackDiff
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadMinWhiteBlackDiff
  (JNIEnv* env, jclass, jlong det, jint val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadMinWhiteBlackDiff(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadMinWhiteBlackDiff
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadMinWhiteBlackDiff
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return 0;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetQuadMinWhiteBlackDiff();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setQuadDeglitch
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setQuadDeglitch
  (JNIEnv* env, jclass, jlong det, jboolean val)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadDeglitch(val);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getQuadDeglitch
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getQuadDeglitch
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return false;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->GetQuadDeglitch();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    addFamily
 * Signature: (JLjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_addFamily
  (JNIEnv* env, jclass, jlong det, jstring fam, jint bitsCorrected)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return false;
  }
  if (!fam) {
    nullPointerEx.Throw(env, "fam cannot be null");
    return false;
  }
  return reinterpret_cast<AprilTagDetector*>(det)->AddFamily(
      JStringRef{env, fam}, bitsCorrected);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    removeFamily
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_removeFamily
  (JNIEnv* env, jclass, jlong det, jstring fam)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  if (!fam) {
    nullPointerEx.Throw(env, "fam cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->RemoveFamily(JStringRef{env, fam});
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    clearFamilies
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_clearFamilies
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->ClearFamilies();
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    detect
 * Signature: (JIIIJ)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_detect
  (JNIEnv* env, jclass, jlong det, jint width, jint height, jint stride,
   jlong bufAddr)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return nullptr;
  }
  if (bufAddr == 0) {
    nullPointerEx.Throw(env, "bufAddr cannot be null");
    return nullptr;
  }
  return MakeJObject(
      env, reinterpret_cast<AprilTagDetector*>(det)->Detect(
               width, height, stride, reinterpret_cast<uint8_t*>(bufAddr)));
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    estimatePoseHomography
 * Signature: ([DDDDDD)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_estimatePoseHomography
  (JNIEnv* env, jclass, jdoubleArray homography, jdouble tagSize, jdouble fx,
   jdouble fy, jdouble cx, jdouble cy)
{
  // reconstruct a basic det--only need H
  apriltag_detection_t cdet;
  cdet.H = FromJavaHomography(env, homography);
  if (!cdet.H) {
    return nullptr;
  }
  AprilTagDetection det;
  std::memcpy(&det, &cdet, sizeof(det));

  auto res = det.EstimatePoseHomography({tagSize, fx, fy, cx, cy});

  matd_destroy(cdet.H);
  return MakeJObject(env, res);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    estimatePoseOrthogonalIteration
 * Signature: ([D[DDDDDDI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_estimatePoseOrthogonalIteration
  (JNIEnv* env, jclass, jdoubleArray homography, jdoubleArray corners,
   jdouble tagSize, jdouble fx, jdouble fy, jdouble cx, jdouble cy, jint nIters)
{
  // reconstruct a basic det--only need H and c
  apriltag_detection_t cdet;
  if (!FromJavaCorners(env, corners, &cdet)) {
    return nullptr;
  }
  cdet.H = FromJavaHomography(env, homography);
  if (!cdet.H) {
    return nullptr;
  }
  AprilTagDetection det;
  std::memcpy(&det, &cdet, sizeof(det));

  auto res =
      det.EstimatePoseOrthogonalIteration({tagSize, fx, fy, cx, cy}, nIters);

  matd_destroy(cdet.H);
  return MakeJObject(env, res);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    estimatePose
 * Signature: ([D[DDDDDD)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_estimatePose
  (JNIEnv* env, jclass, jdoubleArray homography, jdoubleArray corners,
   jdouble tagSize, jdouble fx, jdouble fy, jdouble cx, jdouble cy)
{
  // reconstruct a basic det--only need H and c
  apriltag_detection_t cdet;
  if (!FromJavaCorners(env, corners, &cdet)) {
    return nullptr;
  }
  cdet.H = FromJavaHomography(env, homography);
  if (!cdet.H) {
    return nullptr;
  }
  AprilTagDetection det;
  std::memcpy(&det, &cdet, sizeof(det));

  auto res = det.EstimatePose({tagSize, fx, fy, cx, cy});

  matd_destroy(cdet.H);
  return MakeJObject(env, res);
}

}  // extern "C"
