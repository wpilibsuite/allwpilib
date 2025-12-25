// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <Eigen/Core>
#include <mrcal_wrapper.h>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <opencv2/videoio.hpp>

#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"

namespace wpical {
struct CameraModel {
  Eigen::Matrix<double, 3, 3, Eigen::RowMajor> intrinsicMatrix;
  Eigen::Matrix<double, 8, 1> distortionCoefficients;
  double avgReprojectionError = -1;
};

CameraModel MrcalResultToCameraModel(mrcal_result& stats);

class Data;
class Worker;

class CameraCalibrator {
 public:
  /**
   * Opens the video at the path, and performs camera calibration using the
   * frames in the video.
   *
   * @param numWorkers The number of threads to spawn.
   * @param squareWidth The width of the full square in meters.
   * @param markerWidth The width of the marker in meters.
   * @param boardWidth How many markers wide the board is.
   * @param boardHeight How many markers tall the board is.
   * @param videoPath The absolute path to the video.
   */
  CameraCalibrator(size_t numWorkers, double squareWidth, double markerWidth,
                   int boardWidth, int boardHeight, std::string& videoPath);

  ~CameraCalibrator();

  /**
   * Returns whether or not all frames have been processed or whether or not the
   * camera calibrator has been stopped.
   * @return true if all frames have been processed or if the camera calibrator
   * has been stopped, false otherwise.
   */
  bool IsFinished();

  /**
   * Gets the camera model.
   * @return std::nullopt if the camera calibrator is not finished processing or
   * if calibration failed. Returns CameraModel if it's finished and calibration
   * was successful. If calibration is successful, there will always be a valid
   * CameraModel before IsFinished returns true.
   */
  std::optional<CameraModel> GetCameraModel();

  /**
   * Returns the total number of frames that have been processed.
   * @return The total number of processed frames.
   */
  int TotalFramesProcessed();

  /**
   * Returns the total number of frames in the video.
   * @return The total number of frames.
   */
  int TotalFrames();

  /**
   * Stops all worker threads.
   */
  void Stop();

 private:
  // Ensures that shared state lives until everything else is destroyed
  std::shared_ptr<Data> m_state;

  std::atomic_bool m_isFinished{false};
  std::atomic_int m_totalFrames;
  std::vector<std::shared_ptr<Worker>> m_workers;
};

void to_json(wpi::util::json& json, const CameraModel& cameraModel);

void from_json(const wpi::util::json& json, CameraModel& cameraModel);
}  // namespace wpical
