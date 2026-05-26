// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "jni/mrcal_jni.hpp"

#include <algorithm>
#include <cstdio>
#include <exception>
#include <iostream>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <wpi/util/jni_util.hpp>

#include "mrcal-uncertainty.hpp"
#include "mrcal_wrapper.hpp"

using namespace wpi::util::java;

// Cache MrCalResult class
static JClass detectionClass;
static jmethodID constructor;

static constexpr std::string_view JNI_BOOL{"Z"};
static constexpr std::string_view JNI_VOID{"V"};
static constexpr std::string_view JNI_INT{"I"};
static constexpr std::string_view JNI_DOUBLE{"D"};
static constexpr std::string_view JNI_DOUBLEARR{"[D"};
static constexpr std::string_view JNI_BOOLARR{"[Z"};

template <typename... Args>
constexpr std::string jni_make_method_sig(std::string_view retval,
                                          Args&&... args) {
  std::string result = "(";
  ((result += std::string_view(args)),
   ...);  // Ensure args are converted to string_view
  result += ")";
  result += retval;
  return result;
}

extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  detectionClass = JClass(env, "org/wpilib/mrcal/MrCalJNI$MrCalResult");

  if (!detectionClass) {
    std::printf("Couldn't find detection class!");
    return JNI_ERR;
  }

  // Find the constructor. Reference:
  // https://www.microfocus.com/documentation/extend-acucobol/925/BKITITJAVAS027.html
  constructor = env->GetMethodID(
      detectionClass, "<init>",
      jni_make_method_sig(JNI_VOID, JNI_BOOL, JNI_INT, JNI_INT, JNI_DOUBLEARR,
                          JNI_DOUBLEARR, JNI_DOUBLE, JNI_DOUBLEARR, JNI_DOUBLE,
                          JNI_DOUBLE, JNI_INT, JNI_BOOLARR)
          .c_str());

  if (!constructor) {
    std::printf("Couldn't find detection ctor!");
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

// Helper class for managing JNI array access with automatic cleanup. Thanks,
// Claude
template <typename T>
class JNIArrayView {
 public:
  JNIArrayView(JNIEnv* env, jdoubleArray jArray)
      : env_(env), jArray_(jArray), data_(nullptr), size_(0) {
    if (jArray) {
      size_ = env->GetArrayLength(jArray);
      data_ = env->GetDoubleArrayElements(jArray, nullptr);
    }
  }

  ~JNIArrayView() {
    if (data_) {
      env_->ReleaseDoubleArrayElements(jArray_, data_, JNI_ABORT);
    }
  }

  // Delete copy operations to prevent double-free
  JNIArrayView(const JNIArrayView&) = delete;
  JNIArrayView& operator=(const JNIArrayView&) = delete;

  bool isValid() const { return data_ != nullptr; }

  template <typename U = T>
  std::span<U> asSpan(jsize elementSize = sizeof(T)) {
    return std::span<U>(reinterpret_cast<U*>(data_),
                        size_ / (sizeof(U) / sizeof(double)));
  }

  std::span<double> asDoubleSpan() { return std::span<double>(data_, size_); }

 private:
  JNIEnv* env_;
  jdoubleArray jArray_;
  jdouble* data_;
  jsize size_;
};

extern "C" {

/*
 * Class:     org_wpilib_mrcal_MrCalJNI_mrcal_1calibrate
 * Method:    1camera
 * Signature: ([DIIDIID)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_mrcal_MrCalJNI_mrcal_1calibrate_1camera
  (JNIEnv* env, jclass, jdoubleArray observations_board, jint boardWidth,
   jint boardHeight, jdouble boardSpacing, jint imageWidth, jint imageHeight,
   jdouble focalLenGuessMM)
{
  try {
    // Pull out arrays. We rely on data being packed and aligned to make this
    // work! Observations should be [x, y, level]
    std::span<mrcal_point3_t> observations{
        reinterpret_cast<mrcal_point3_t*>(
            env->GetDoubleArrayElements(observations_board, 0)),
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

    if (!constructor) {
      return nullptr;
    }

    jdoubleArray intrinsics = MakeJDoubleArray(env, stats.intrinsics);
    jdoubleArray residuals = MakeJDoubleArray(env, stats.residuals);
    jboolean success = stats.success;
    jdouble rms_err = stats.rms_error;
    jdouble warp_x = stats.calobject_warp.x2;
    jdouble warp_y = stats.calobject_warp.y2;
    jint Noutliers = stats.Noutliers_board;

    jdoubleArray optimized_rt_toref = MakeJDoubleArray(
        env, std::span<double>(
                 reinterpret_cast<double*>(total_frames_rt_toref.data()),
                 total_frames_rt_toref.size() * sizeof(mrcal_pose_t) /
                     sizeof(double)));

    std::vector<jboolean> cornersUsedMask(observations.size());
    std::transform(observations.begin(), observations.end(),
                   cornersUsedMask.begin(),
                   [](const auto& pt) { return (jboolean)(pt.z > 0); });
    auto cornersUsedJarr = MakeJBooleanArray(env, cornersUsedMask);

    // Actually call the constructor
    auto ret =
        env->NewObject(detectionClass, constructor, success, boardWidth,
                       boardHeight, intrinsics, optimized_rt_toref, rms_err,
                       residuals, warp_x, warp_y, Noutliers, cornersUsedJarr);

    return ret;
  } catch (...) {
    std::cerr << "Calibration exception: " << what() << std::endl;

    static char buff[512];
    std::snprintf(buff, sizeof(buff), "%s", what().c_str());
    buff[sizeof(buff) - 1] = 0;
    env->ThrowNew(env->FindClass("java/lang/Exception"), buff);
    return nullptr;
  }
}

/*
 * Class:     org_wpilib_mrcal_MrCalJNI_undistort
 * Method:    1mrcal
 * Signature: (JJJIIIII)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_mrcal_MrCalJNI_undistort_1mrcal
  (JNIEnv*, jclass, jlong dstMat, jlong camMat, jlong distCoeffs,
   jint lensModelOrdinal, jint order, jint Nx, jint Ny, jint fov_x_deg)
{
  return undistort_mrcal(
      reinterpret_cast<cv::Mat*>(dstMat), reinterpret_cast<cv::Mat*>(camMat),
      reinterpret_cast<cv::Mat*>(distCoeffs),
      static_cast<CameraLensModel>(lensModelOrdinal),
      static_cast<uint16_t>(order), static_cast<uint16_t>(Nx),
      static_cast<uint16_t>(Ny), static_cast<uint16_t>(fov_x_deg));
}

/*
 * Class:     org_wpilib_mrcal_MrCalJNI_compute
 * Method:    1uncertainty
 * Signature: ([D[D[DIIDIIIIDD)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_org_wpilib_mrcal_MrCalJNI_compute_1uncertainty
  (JNIEnv* env, jclass, jdoubleArray jObservations, jdoubleArray jIntrinsics,
   jdoubleArray jRtRefFrames, jint boardWidth, jint boardHeight,
   jdouble boardSpacing, jint imageWidth, jint imageHeight,
   jint sampleGridWidth, jint sampleGridHeight, jdouble warpX, jdouble warpY)
{
  // Create RAII wrappers - automatic cleanup on scope exit
  JNIArrayView<mrcal_point3_t> observations(env, jObservations);
  JNIArrayView<double> intrinsics(env, jIntrinsics);
  JNIArrayView<mrcal_pose_t> rtFrames(env, jRtRefFrames);

  // Validate all arrays
  if (!observations.isValid() || !intrinsics.isValid() || !rtFrames.isValid()) {
    std::cout << "bad array in?" << std::endl;
    return nullptr;
  }

  // Setup parameters
  mrcal_calobject_warp_t warp{.x2 = warpX, .y2 = warpY};
  cv::Size imagerSize(imageWidth, imageHeight);
  cv::Size calobjectSize(boardWidth, boardHeight);
  cv::Size sampleRes(sampleGridWidth, sampleGridHeight);

  std::vector<mrcal_point3_t> result;
  try {
    result = compute_uncertainty(
        observations.asSpan<mrcal_point3_t>(), intrinsics.asDoubleSpan(),
        rtFrames.asSpan<mrcal_pose_t>(), warp, imagerSize, calobjectSize,
        boardSpacing, sampleRes);
  } catch (const std::exception& e) {
    std::cout << "exception thrown -- " << e.what() << std::endl;
    return nullptr;
  }

  jsize resultSize = result.size() * 3;
  jdoubleArray jResult = env->NewDoubleArray(resultSize);
  if (jResult == nullptr) {
    std::cout << "waah" << std::endl;
    return nullptr;
  }

  // point3's are just packed doubles so we can do trickery
  env->SetDoubleArrayRegion(jResult, 0, resultSize,
                            reinterpret_cast<const double*>(result.data()));
  return jResult;
}
}  // extern "C"
