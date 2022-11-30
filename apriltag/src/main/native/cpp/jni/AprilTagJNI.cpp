// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/jni_util.h>

#include "edu_wpi_first_apriltag_jni_AprilTagJNI.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4200)
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "apriltag.h"
#ifdef _WIN32
#pragma warning(pop)
#endif

#include "tag36h11.h"
#include "tag25h9.h"
#include "tag16h5.h"
#include "tagCircle21h7.h"
#include "tagCircle49h12.h"
#include "tagCustom48h12.h"
#include "tagStandard41h12.h"
#include "tagStandard52h13.h"
#include "apriltag_pose.h"

#include <vector>
#include <algorithm>
#include <cmath>

using namespace wpi::java;

struct DetectorState {
  int id;
  apriltag_detector_t* td;
  apriltag_family_t* tf;
  void (*tf_destroy)(apriltag_family_t*);
};

static std::vector<DetectorState> detectors;

extern "C" {

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    aprilTagCreate
 * Signature: (Ljava/lang/String;DDIZZ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_aprilTagCreate
  (JNIEnv* env, jclass cls, jstring jstr, jdouble decimate, jdouble blur,
   jint threads, jboolean debug, jboolean refine_edges)
{
  // Initialize tag detector with options
  apriltag_family_t* tf = nullptr;
  // const char *famname = fam;
  const char* famname = env->GetStringUTFChars(jstr, nullptr);

  void (*tf_destroy_func)(apriltag_family_t*);

  if (!strcmp(famname, "tag36h11")) {
    tf = tag36h11_create();
    tf_destroy_func = tag36h11_destroy;
  } else if (!strcmp(famname, "tag25h9")) {
    tf = tag25h9_create();
    tf_destroy_func = tag25h9_destroy;
  } else if (!strcmp(famname, "tag16h5")) {
    tf = tag16h5_create();
    tf_destroy_func = tag16h5_destroy;
  } else if (!strcmp(famname, "tagCircle21h7")) {
    tf = tagCircle21h7_create();
    tf_destroy_func = tagCircle21h7_destroy;
  } else if (!strcmp(famname, "tagCircle49h12")) {
    tf = tagCircle49h12_create();
    tf_destroy_func = tagCircle49h12_destroy;
  } else if (!strcmp(famname, "tagStandard41h12")) {
    tf = tagStandard41h12_create();
    tf_destroy_func = tagStandard41h12_destroy;
  } else if (!strcmp(famname, "tagStandard52h13")) {
    tf = tagStandard52h13_create();
    tf_destroy_func = tagStandard52h13_destroy;
  } else if (!strcmp(famname, "tagCustom48h12")) {
    tf = tagCustom48h12_create();
    tf_destroy_func = tagCustom48h12_destroy;
  } else {
    std::printf("Unrecognized tag family name. Use e.g. \"tag36h11\".\n");
    env->ReleaseStringUTFChars(jstr, famname);
    return 0;
  }

  apriltag_detector_t* td = apriltag_detector_create();
  apriltag_detector_add_family(td, tf);
  td->quad_decimate = static_cast<float>(decimate);
  td->quad_sigma = static_cast<float>(blur);
  td->nthreads = threads;
  td->debug = debug;
  td->refine_edges = refine_edges;

  env->ReleaseStringUTFChars(jstr, famname);

  // std::printf("Looking for max\n");
  auto max = std::max_element(detectors.begin(), detectors.end(),
                              [](DetectorState& a, DetectorState& b) {
                                return a.id < b.id;
                              });  // detectors.size();
  int index = 0;
  if (max != detectors.end())
    index = max->id + 1;
  detectors.push_back({index, td, tf, tf_destroy_func});
  std::printf("Created detector at idx %i\n", index);
  return (jlong)index;
}

static JClass detectionClass;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  detectionClass = JClass(env, "edu/wpi/first/apriltag/jni/DetectionResult");

  if (!detectionClass) {
    std::printf("Couldn't find class!");
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

static jobject MakeJObject(JNIEnv* env, const apriltag_detection_t* detect,
                           apriltag_pose_t& pose1, apriltag_pose_t& pose2,
                           double error1, double error2) {
  // Constructor signature must match Java! I = int, F = float, [D = double
  // array
  static jmethodID constructor =
      env->GetMethodID(detectionClass, "<init>", "(IIF[DDD[D[D[DD[D[DD)V");

  if (!constructor) {
    return nullptr;
  }

  if (!detect) {
    return nullptr;
  }

  // We have to copy the homography matrix and coners into jdoubles
  jdouble h[9];  // = new jdouble[9]{};
  for (int i = 0; i < 9; i++) {
    h[i] = detect->H->data[i];
  }

  jdouble corners[8];  // = new jdouble[8]{};
  for (int i = 0; i < 4; i++) {
    corners[i * 2] = detect->p[i][0];
    corners[i * 2 + 1] = detect->p[i][1];
  }

  jdoubleArray harr = MakeJDoubleArray(env, {h, 9});
  jdoubleArray carr = MakeJDoubleArray(env, {corners, 8});

  // The rotation of the target is encoded as a 3 by 3 rotation matrix, we'll
  // convert to a row-major array
  jdouble pose1RotMat[9] = {0};
  jdouble pose2RotMat[9] = {0};

  for (int i = 0; i < 9; i++) {
    if (pose1.R) {
      pose1RotMat[i] = pose1.R->data[i];
    }
    if (pose2.R) {
      pose2RotMat[i] = pose2.R->data[i];
    }
  }

  // And translation a 3x1 vector (todo check axis order)
  jdouble pose1Trans[3] = {0};
  jdouble pose2Trans[3] = {0};
  for (int i = 0; i < 3; i++) {
    if (pose1.t) {
      pose1Trans[i] = pose1.t->data[i];
    }
    if (pose2.t) {
      pose2Trans[i] = pose2.t->data[i];
    }
  }

  jdoubleArray pose1rotArr = MakeJDoubleArray(env, {pose1RotMat, 9});
  jdoubleArray pose2rotArr = MakeJDoubleArray(env, {pose2RotMat, 9});
  jdoubleArray pose1transArr = MakeJDoubleArray(env, {pose1Trans, 3});
  jdoubleArray pose2transArr = MakeJDoubleArray(env, {pose2Trans, 3});
  jdouble err1 = error1;
  jdouble err2 = error2;

  // Actually call the constructor
  auto ret = env->NewObject(
      detectionClass, constructor, (jint)detect->id, (jint)detect->hamming,
      (jfloat)detect->decision_margin, harr, (jdouble)detect->c[0],
      (jdouble)detect->c[1], carr, pose1transArr, pose1rotArr, err1,
      pose2transArr, pose2rotArr, err2);

  // TODO we don't seem to need this... or at least, it doesnt leak rn
  // env->ReleaseDoubleArrayElements(harr, h, 0);
  // env->ReleaseDoubleArrayElements(carr, corners, 0);

  return ret;
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    aprilTagDetectInternal
 * Signature: (JJIIZDDDDDI)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_aprilTagDetectInternal
  (JNIEnv* env, jclass cls, jlong detectIdx, jlong pData, jint rows, jint cols,
   jboolean doPoseEstimation, jdouble tagWidthMeters, jdouble fx, jdouble fy,
   jdouble cx, jdouble cy, jint nIters)
{
  // No image, can't do anything
  if (!pData) {
    return nullptr;
  }

  // Make an image_u8_t header for the Mat data
  image_u8_t im = {static_cast<int32_t>(cols), static_cast<int32_t>(rows),
                   static_cast<int32_t>(cols),
                   reinterpret_cast<uint8_t*>(pData)};

  // Get our detector
  auto state =
      std::find_if(detectors.begin(), detectors.end(),
                   [&](DetectorState& s) { return s.id == detectIdx; });
  if (state == detectors.end()) {
    return nullptr;
  }

  // And run the detector on our new image
  zarray_t* detections = apriltag_detector_detect(state->td, &im);

  int size = zarray_size(detections);

  // Object array to return to Java
  jobjectArray jarr = env->NewObjectArray(size, detectionClass, nullptr);
  if (!jarr) {
    std::printf("Couldn't make array\n");
    return nullptr;
  }

  // Global pose
  apriltag_pose_t pose1;
  std::memset(&pose1, 0, sizeof(pose1));

  apriltag_pose_t pose2;
  std::memset(&pose2, 0, sizeof(pose2));

  // std::printf("Created array %llu! Got %i targets!\n", &jarr, size);
  //  Add our detected targets to the array
  for (int i = 0; i < size; ++i) {
    apriltag_detection_t* det = nullptr;
    zarray_get(detections, i, &det);

    if (det != nullptr) {
      double err1 =
          HUGE_VAL;  // Should get overwritten if pose estimation is happening
      double err2 = HUGE_VAL;
      if (doPoseEstimation) {
        // Feed results to the pose estimator
        apriltag_detection_info_t info{det, tagWidthMeters, fx, fy, cx, cy};
        estimate_tag_pose_orthogonal_iteration(&info, &err1, &pose1, &err2,
                                               &pose2, nIters);
      }

      jobject obj = MakeJObject(env, det, pose1, pose2, err1, err2);

      env->SetObjectArrayElement(jarr, i, obj);
    }
  }

  // Now that stuff's in our Java-side array, we can clean up native memory
  apriltag_detections_destroy(detections);

  return jarr;
}

/*
 * Class:     edu_wpi_first_apriltag_jni_AprilTagJNI
 * Method:    aprilTagDestroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_apriltag_jni_AprilTagJNI_aprilTagDestroy
  (JNIEnv* env, jclass clazz, jlong detectIdx)
{
  auto state =
      std::find_if(detectors.begin(), detectors.end(),
                   [&](DetectorState& s) { return s.id == detectIdx; });

  if (state == detectors.end()) {
    return;
  }

  if (state->td) {
    apriltag_detector_destroy(state->td);
    state->td = nullptr;
  }
  if (state->tf) {
    state->tf_destroy(state->tf);
    state->tf = nullptr;
  }

  detectors.erase(detectors.begin() + detectIdx);
}
}  // extern "C"
