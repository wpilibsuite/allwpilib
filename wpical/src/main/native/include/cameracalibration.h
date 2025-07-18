// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <mrcal_wrapper.h>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <opencv2/videoio.hpp>
#include <wpi/json.h>
#include <wpi/mutex.h>
#include <wpi/print.h>

namespace wpical {
struct CameraModel {
  Eigen::Matrix<double, 3, 3> intrinsicMatrix;
  Eigen::Matrix<double, 8, 1> distortionCoefficients;
  double avgReprojectionError = -1;
};

CameraModel MrcalResultToCameraModel(mrcal_result& stats);

class Worker {
 public:
  /**
   * @param getMat A function that returns an optional Mat.
   * @param squareWidth The width of the full square in meters
   * @param markerWidth The width of the marker in meters
   * @param boardWidth How many markers wide the board is
   * @param boardHeight How many markers tall the board is
   */
  explicit Worker(std::function<std::optional<cv::Mat>()> getMat,
                  float squareWidth, float markerWidth, int boardWidth,
                  int boardHeight);

  ~Worker();

  void ProcessNextImage(cv::Mat image);

  auto GetData() {
    return std::make_pair(m_observationBoards, m_framesRtToref);
  }

  void Stop();

  int GetProcessedFrameCount() { return m_framesProcessed; }

 private:
  std::atomic_bool m_isDone{false};
  std::atomic_int m_framesProcessed{0};
  std::function<std::optional<cv::Mat>()> m_getMat;
  int m_boardWidth;
  int m_boardHeight;
  double m_squareWidth;
  std::vector<mrcal_point3_t> m_observationBoards;
  std::vector<mrcal_pose_t> m_framesRtToref;
  // ChArUco Board
  cv::aruco::Dictionary m_arucoDict =
      cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
  cv::aruco::CharucoBoard m_charucoBoard;
  cv::aruco::CharucoDetector m_charucoDetector;
  std::thread m_processingThread;
};

class CameraCalibrator {
 public:
  /**
   * @param squareWidth The width of the full square in meters
   * @param markerWidth The width of the marker in meters
   * @param boardWidth How many markers wide the board is
   * @param boardHeight How many markers tall the board is
   */
  CameraCalibrator(size_t numWorkers, double squareWidth, double markerWidth,
                   int boardWidth, int boardHeight);

  ~CameraCalibrator();

  /**
   * Opens the video at the path, and performs camera calibration using the
   * frames in the video.
   * @param videoPath The absolute path to the video.
   */
  void ProcessVideo(std::string& videoPath);

  bool IsFinished() { return m_isFinished; }

  /**
   * Gets the camera model.
   * @return std::nullopt if the camera calibrator is not finished processing or
   * if calibration failed. Returns CameraModel if it's finished and calibration
   * was successful. If calibration is successful, there will always be a valid
   * CameraModel before IsFinished returns true.
   */
  std::optional<CameraModel> GetCameraModel() { return m_cameraModel; }

  int TotalFramesProcessed();

  int TotalFrames() { return m_totalFrames.load(); }

  /**
   * Stops all worker threads.
   */
  void Stop();

 private:
  /**
   * Adds a frame to the queue for workers to grab from.
   * @param mat The mat to queue up.
   */
  void AddFrame(cv::Mat&& mat);

  /**
   * Returns a frame from the queue.
   * @return A frame, or std::nullopt if the queue is empty.
   */
  std::optional<cv::Mat> GetFrame();

  cv::Size m_cornerSize;
  double m_squareWidth;
  std::optional<CameraModel> m_cameraModel;
  std::atomic_bool m_isFinished{false};
  std::queue<cv::Mat> m_queue;
  std::atomic_int m_totalFrames;
  std::thread m_processingThread;
  wpi::mutex m_mutex;
  std::vector<std::unique_ptr<Worker>> m_workers;
};

void to_json(wpi::json& json, const CameraModel& cameraModel);

void from_json(const wpi::json& json, CameraModel& cameraModel);
}  // namespace wpical
