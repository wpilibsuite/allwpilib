// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cstdio>
#include <cstring>

#define WPI_RAWFRAME_JNI
#include <wpi/RawFrame.h>
#include <wpi/jni_util.h>

#include "edu_wpi_first_apriltag_jni_AprilTagJNI.h"
#include "frc/apriltag/AprilTag.h"
#include "frc/apriltag/AprilTagDetector.h"
#include "frc/apriltag/AprilTagPoseEstimator.h"

using namespace frc;
using namespace wpi::java;

static JavaVM* jvm = nullptr;

static JClass detectionCls;
static JClass detectorConfigCls;
static JClass detectorQTPCls;
static JClass poseEstimateCls;
static JClass quaternionCls;
static JClass rotation3dCls;
static JClass transform3dCls;
static JClass translation3dCls;
static JClass rawFrameCls;
static JException illegalArgEx;
static JException nullPointerEx;

static const JClassInit classes[] = {
    {"edu/wpi/first/apriltag/AprilTagDetection", &detectionCls},
    {"edu/wpi/first/apriltag/AprilTagDetector$Config", &detectorConfigCls},
    {"edu/wpi/first/apriltag/AprilTagDetector$QuadThresholdParameters",
     &detectorQTPCls},
    {"edu/wpi/first/apriltag/AprilTagPoseEstimate", &poseEstimateCls},
    {"edu/wpi/first/math/geometry/Quaternion", &quaternionCls},
    {"edu/wpi/first/math/geometry/Rotation3d", &rotation3dCls},
    {"edu/wpi/first/math/geometry/Transform3d", &transform3dCls},
    {"edu/wpi/first/math/geometry/Translation3d", &translation3dCls},
    {"edu/wpi/first/util/RawFrame", &rawFrameCls}};

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
// Conversions from Java to C++ objects
//

static AprilTagDetector::Config FromJavaDetectorConfig(JNIEnv* env,
                                                       jobject jconfig) {
  if (!jconfig) {
    return {};
  }
#define FIELD(name, sig)                                          \
  static jfieldID name##Field = nullptr;                          \
  if (!name##Field) {                                             \
    name##Field = env->GetFieldID(detectorConfigCls, #name, sig); \
  }

  FIELD(numThreads, "I");
  FIELD(quadDecimate, "F");
  FIELD(quadSigma, "F");
  FIELD(refineEdges, "Z");
  FIELD(decodeSharpening, "D");
  FIELD(debug, "Z");
  FIELD(roiX, "I");
  FIELD(roiY, "I");
  FIELD(roiWidth, "I");
  FIELD(roiHeight, "I");

#undef FIELD

#define FIELD(ctype, jtype, name) \
  .name = static_cast<ctype>(env->Get##jtype##Field(jconfig, name##Field))

  return {
      FIELD(int, Int, numThreads),
      FIELD(float, Float, quadDecimate),
      FIELD(float, Float, quadSigma),
      FIELD(bool, Boolean, refineEdges),
      FIELD(double, Double, decodeSharpening),
      FIELD(bool, Boolean, debug),
      FIELD(int, Int, roiX),
      FIELD(int, Int, roiY),
      FIELD(int, Int, roiWidth),
      FIELD(int, Int, roiHeight)
  };

#undef GET
#undef FIELD
}

static AprilTagDetector::QuadThresholdParameters FromJavaDetectorQTP(
    JNIEnv* env, jobject jparams) {
  if (!jparams) {
    return {};
  }
#define FIELD(name, sig)                                       \
  static jfieldID name##Field = nullptr;                       \
  if (!name##Field) {                                          \
    name##Field = env->GetFieldID(detectorQTPCls, #name, sig); \
  }

  FIELD(minClusterPixels, "I");
  FIELD(maxNumMaxima, "I");
  FIELD(criticalAngle, "D");
  FIELD(maxLineFitMSE, "F");
  FIELD(minWhiteBlackDiff, "I");
  FIELD(deglitch, "Z");

#undef FIELD

#define FIELD(ctype, jtype, name) \
  .name = static_cast<ctype>(env->Get##jtype##Field(jparams, name##Field))

  return {
      FIELD(int, Int, minClusterPixels),
      FIELD(int, Int, maxNumMaxima),
      .criticalAngle = units::radian_t{static_cast<double>(
          env->GetDoubleField(jparams, criticalAngleField))},
      FIELD(float, Float, maxLineFitMSE),
      FIELD(int, Int, minWhiteBlackDiff),
      FIELD(bool, Boolean, deglitch),
  };

#undef GET
#undef FIELD
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

  auto homography = detect.GetHomography();
  JLocal<jdoubleArray> harr{
      env, MakeJDoubleArray(
               env, {reinterpret_cast<const jdouble*>(homography.data()),
                     homography.size()})};

  double cornersBuf[8];
  auto corners = detect.GetCorners(cornersBuf);
  JLocal<jdoubleArray> carr{
      env,
      MakeJDoubleArray(env, {reinterpret_cast<const jdouble*>(corners.data()),
                             corners.size()})};

  auto center = detect.GetCenter();

  return env->NewObject(detectionCls, constructor, fam.obj(),
                        static_cast<jint>(detect.GetId()),
                        static_cast<jint>(detect.GetHamming()),
                        static_cast<jfloat>(detect.GetDecisionMargin()),
                        harr.obj(), static_cast<jdouble>(center.x),
                        static_cast<jdouble>(center.y), carr.obj());
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

static jobject MakeJObject(JNIEnv* env,
                           const AprilTagDetector::Config& config) {
  static jmethodID constructor =
      env->GetMethodID(detectorConfigCls, "<init>", "(IFFZDZIIII)V");
  if (!constructor) {
    return nullptr;
  }

  return env->NewObject(detectorConfigCls, constructor,
                        static_cast<jint>(config.numThreads),
                        static_cast<jfloat>(config.quadDecimate),
                        static_cast<jfloat>(config.quadSigma),
                        static_cast<jboolean>(config.refineEdges),
                        static_cast<jdouble>(config.decodeSharpening),
                        static_cast<jboolean>(config.debug),
                        static_cast<jint>(config.roiX),
                        static_cast<jint>(config.roiY),
                        static_cast<jint>(config.roiWidth),
                        static_cast<jint>(config.roiHeight)
                        );
}

static jobject MakeJObject(
    JNIEnv* env, const AprilTagDetector::QuadThresholdParameters& params) {
  static jmethodID constructor =
      env->GetMethodID(detectorQTPCls, "<init>", "(IIDFIZ)V");
  if (!constructor) {
    return nullptr;
  }

  return env->NewObject(detectorQTPCls, constructor,
                        static_cast<jint>(params.minClusterPixels),
                        static_cast<jint>(params.maxNumMaxima),
                        static_cast<jdouble>(params.criticalAngle),
                        static_cast<jfloat>(params.maxLineFitMSE),
                        static_cast<jint>(params.minWhiteBlackDiff),
                        static_cast<jboolean>(params.deglitch));
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

static jobject MakeJObject(JNIEnv* env, const AprilTagPoseEstimate& est) {
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
 * Method:    setDetectorConfig
 * Signature: (JLjava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setDetectorConfig
  (JNIEnv* env, jclass, jlong det, jobject config)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetConfig(
      FromJavaDetectorConfig(env, config));
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getDetectorConfig
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getDetectorConfig
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return nullptr;
  }
  return MakeJObject(env,
                     reinterpret_cast<AprilTagDetector*>(det)->GetConfig());
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    setDetectorQTP
 * Signature: (JLjava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_setDetectorQTP
  (JNIEnv* env, jclass, jlong det, jobject params)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return;
  }
  reinterpret_cast<AprilTagDetector*>(det)->SetQuadThresholdParameters(
      FromJavaDetectorQTP(env, params));
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    getDetectorQTP
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_getDetectorQTP
  (JNIEnv* env, jclass, jlong det)
{
  if (det == 0) {
    nullPointerEx.Throw(env, "det cannot be null");
    return nullptr;
  }
  return MakeJObject(
      env,
      reinterpret_cast<AprilTagDetector*>(det)->GetQuadThresholdParameters());
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
  if (!homography) {
    nullPointerEx.Throw(env, "homography cannot be null");
    return nullptr;
  }
  JSpan<const jdouble, 9> harr{env, homography};
  if (harr.size() != 9) {
    illegalArgEx.Throw(env, "homography array must be size 9");
    return nullptr;
  }

  AprilTagPoseEstimator estimator({units::meter_t{tagSize}, fx, fy, cx, cy});
  return MakeJObject(env, estimator.EstimateHomography(harr));
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
  // homography
  if (!homography) {
    nullPointerEx.Throw(env, "homography cannot be null");
    return nullptr;
  }
  JSpan<const jdouble, 9> harr{env, homography};
  if (harr.size() != 9) {
    illegalArgEx.Throw(env, "homography array must be size 9");
    return nullptr;
  }

  // corners
  if (!corners) {
    nullPointerEx.Throw(env, "corners cannot be null");
    return nullptr;
  }
  JSpan<const jdouble, 8> carr{env, corners};
  if (carr.size() != 8) {
    illegalArgEx.Throw(env, "corners array must be size 8");
    return nullptr;
  }

  AprilTagPoseEstimator estimator({units::meter_t{tagSize}, fx, fy, cx, cy});
  return MakeJObject(env,
                     estimator.EstimateOrthogonalIteration(harr, carr, nIters));
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
  // homography
  if (!homography) {
    nullPointerEx.Throw(env, "homography cannot be null");
    return nullptr;
  }
  JSpan<const jdouble, 9> harr{env, homography};
  if (harr.size() != 9) {
    illegalArgEx.Throw(env, "homography array must be size 9");
    return nullptr;
  }

  // corners
  if (!corners) {
    nullPointerEx.Throw(env, "corners cannot be null");
    return nullptr;
  }
  JSpan<const jdouble, 8> carr{env, corners};
  if (carr.size() != 8) {
    illegalArgEx.Throw(env, "corners array must be size 8");
    return nullptr;
  }

  AprilTagPoseEstimator estimator({units::meter_t{tagSize}, fx, fy, cx, cy});
  return MakeJObject(env, estimator.Estimate(harr, carr));
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    generate16h5AprilTagImage
 * Signature: (Ljava/lang/Object;JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_generate16h5AprilTagImage
  (JNIEnv* env, jclass, jobject frameObj, jlong framePtr, jint id)
{
  auto* frame = reinterpret_cast<wpi::RawFrame*>(framePtr);
  if (!frame) {
    nullPointerEx.Throw(env, "frame is null");
    return;
  }
  bool newData = AprilTag::Generate16h5AprilTagImage(frame, id);
  wpi::SetFrameData(env, rawFrameCls, frameObj, *frame, newData);
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    generate36h11AprilTagImage
 * Signature: (Ljava/lang/Object;JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_generate36h11AprilTagImage
  (JNIEnv* env, jclass, jobject frameObj, jlong framePtr, jint id)
{
  auto* frame = reinterpret_cast<wpi::RawFrame*>(framePtr);
  if (!frame) {
    nullPointerEx.Throw(env, "frame is null");
    return;
  }
  // function might reallocate
  bool newData = AprilTag::Generate36h11AprilTagImage(frame, id);
  wpi::SetFrameData(env, rawFrameCls, frameObj, *frame, newData);
}


}  // extern "C"
