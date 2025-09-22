// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "mrcal_jni.h"

#include <algorithm>
#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include <wpi/jni_util.h>

#include "mrcal_wrapper.h"

using namespace wpi::java;

// Cache MrCalResult class
JClass detectionClass;

extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  detectionClass = JClass(env, "org/photonvision/mrcal/MrCalJNI$MrCalResult");

  if (!detectionClass) {
    std::printf("Couldn't find class!");
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

}  // extern "C"

static std::string what(
    const std::exception_ptr& eptr = std::current_exception()) {
  if (!eptr) {
    throw std::bad_exception();
  }

  try {
    std::rethrow_exception(eptr);
  } catch (const std::exception& e) {
    return e.what();
  } catch (const std::string& e) {
    return e;
  } catch (const char* e) {
    return e;
  } catch (...) {
    return "who knows";
  }
}

/*
 * Class:     edu_wpi_first_mrcal_MrCalJNI_mrcal_1calibrate
 * Method:    1camera
 * Signature: ([DIIDIID)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_mrcal_MrCalJNI_mrcal_1calibrate_1camera
  (JNIEnv* env, jclass, jdoubleArray observations_board, jint boardWidth,
   jint boardHeight, jdouble boardSpacing, jint imageWidth, jint imageHeight,
   jdouble focalLenGuessMM)
{
  try {
    // Pull out arrays. We rely on data being packed and aligned to make this
    // work! Observations should be [x, y, level]
    std::span<mrcal_point3_t> observations{
        reinterpret_cast<mrcal_point3_t*>(
            env->GetDoubleArrayElements(observations_board, nullptr)),
        env->GetArrayLength(observations_board) / 3lu};

    size_t points_in_board = boardWidth * boardHeight;
    if (observations.size() % points_in_board != 0) {
      jclass exception_class = env->FindClass("java/lang/Exception");
      if (exception_class && env) {
        (env)->ExceptionClear();
        env->ThrowNew(exception_class,
                      "Observation list length does not match board size!");
        return {};
      } else {
        // ????
        std::cerr << "Observation list length does not match board size!\n";
      }
    }

    size_t boards_observed = observations.size() / points_in_board;

    const auto boardSize = cv::Size{boardWidth, boardHeight};
    const auto imagerSize = cv::Size{imageWidth, imageHeight};

    // down big list of observations/extrinsic guesses (one per board object)
    std::vector<mrcal_pose_t> total_frames_rt_toref;

    for (size_t i = 0; i < boards_observed; i++) {
      auto seed_pose =
          getSeedPose(&(*observations.begin()) + (i * points_in_board),
                      boardSize, imagerSize, boardSpacing, focalLenGuessMM);
      // std::printf("Seed pose %lu: r %f %f %f t %f %f %f\n", i, seed_pose.r.x,
      //             seed_pose.r.y, seed_pose.r.z, seed_pose.t.x, seed_pose.t.y,
      //             seed_pose.t.z);

      // Add to seed poses
      total_frames_rt_toref.push_back(seed_pose);
    }

    // Convert detection level to weights
    for (auto& o : observations) {
      double& level = o.z;
      if (level < 0) {
        o.z = -1;
      } else {
        o.z = std::pow(0.5, level);
      }
    }

    auto statsptr = mrcal_main(observations, total_frames_rt_toref, boardSize,
                               static_cast<double>(boardSpacing), imagerSize,
                               focalLenGuessMM);
    if (!statsptr) {
      return nullptr;
    }
    mrcal_result& stats = *statsptr;

    // Find the constructor. Reference:
    // https://www.microfocus.com/documentation/extend-acucobol/925/BKITITJAVAS027.html
    static jmethodID constructor =
        env->GetMethodID(detectionClass, "<init>", "(Z[DD[DDDI)V");
    if (!constructor) {
      return nullptr;
    }

    size_t Nintrinsics = stats.intrinsics.size();
    size_t Nresid = stats.residuals.size();

    jdoubleArray intrinsics = MakeJDoubleArray(env, stats.intrinsics);
    jdoubleArray residuals = MakeJDoubleArray(env, stats.residuals);
    jboolean success = stats.success;
    jdouble rms_err = stats.rms_error;
    jdouble warp_x = stats.calobject_warp.x2;
    jdouble warp_y = stats.calobject_warp.y2;
    jint Noutliers = stats.Noutliers_board;

    // Actually call the constructor (TODO)
    auto ret = env->NewObject(detectionClass, constructor, success, intrinsics,
                              rms_err, residuals, warp_x, warp_y, Noutliers);

    return ret;
  } catch (...) {
    std::cerr << "Calibration exception: " << what() << std::endl;

    static char buff[512];
    std::snprintf(buff, what().size(), "%s", what().c_str());
    env->ThrowNew(env->FindClass("java/lang/Exception"), buff);
    return NULL;
  }
}

/*
 * Class:     edu_wpi_first_mrcal_MrCalJNI_undistort
 * Method:    1mrcal
 * Signature: (JJJJIIIII)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_mrcal_MrCalJNI_undistort_1mrcal
  (JNIEnv*, jclass, jlong srcMat, jlong dstMat, jlong camMat, jlong distCoeffs,
   jint lensModelOrdinal, jint order, jint Nx, jint Ny, jint fov_x_deg)
{
  return undistort_mrcal(
      reinterpret_cast<cv::Mat*>(srcMat), reinterpret_cast<cv::Mat*>(dstMat),
      reinterpret_cast<cv::Mat*>(camMat),
      reinterpret_cast<cv::Mat*>(distCoeffs),
      static_cast<CameraLensModel>(lensModelOrdinal),
      static_cast<uint16_t>(order), static_cast<uint16_t>(Nx),
      static_cast<uint16_t>(Ny), static_cast<uint16_t>(fov_x_deg));
}
