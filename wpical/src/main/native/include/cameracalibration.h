// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
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

namespace cameracalibration {
struct CameraModel {
  Eigen::Matrix<double, 3, 3> intrinsicMatrix;
  Eigen::Matrix<double, 8, 1> distortionCoefficients;
  double avgReprojectionError = -1;
};
CameraModel MrcalResultToCameraModel(mrcal_result& stats);
std::optional<cameracalibration::CameraModel> calibrate(
    const std::string& input_video, float square_width, float marker_width,
    int board_width, int board_height, bool show_debug_window);
std::optional<cameracalibration::CameraModel> calibrate(
    const std::string& input_video, float square_width, int board_width,
    int board_height, double imageWidthPixels, double imageHeightPixels,
    bool show_debug_window);

static bool filter(std::vector<cv::Point2f> charucoCorners,
                   std::vector<int> charucoIds,
                   std::vector<std::vector<cv::Point2f>> marker_corners,
                   std::vector<int> markerIds, int board_width,
                   int board_height) {
  if (charucoIds.empty() || charucoCorners.empty()) {
    return false;
  }

  if (charucoCorners.size() < 10 || charucoIds.size() < 10) {
    return false;
  }

  for (int i = 0; i < charucoIds.size(); i++) {
    if (charucoIds.at(i) > (board_width - 1) * (board_height - 1) - 1) {
      return false;
    }
  }

  for (int i = 0; i < markerIds.size(); i++) {
    if (markerIds.at(i) > ((board_width * board_height) / 2) - 1) {
      return false;
    }
  }

  return true;
}
class Worker {
 public:
  /**
   * @param squareWidth The width of the full square in meters
   * @param markerWidth The width of the marker in meters
   * @param boardWidth How many markers wide the board is
   * @param boardHeight How many markers tall the board is
   */
  Worker(std::function<std::optional<cv::Mat>()> getMat, float squareWidth,
         float markerWidth, int boardWidth, int boardHeight)
      : m_getMat(getMat),
        m_boardWidth(boardWidth),
        m_boardHeight(boardHeight),
        squareWidth(squareWidth * 0.0254),
        m_charucoBoard(std::make_unique<cv::aruco::CharucoBoard>(
            cv::Size(boardWidth, boardHeight), squareWidth * 0.0254,
            markerWidth * 0.0254, *m_arucoDict)),
        m_charucoDetector(
            std::make_unique<cv::aruco::CharucoDetector>(*m_charucoBoard)) {
    m_processingThread = std::thread([this] {
      while (!isDone) {
        auto mat = m_getMat();
        if (mat) {
          ProcessNextImage(*mat);
          ++(this->framesProcessed);
        } else {
          // Sleep
        }
      }
    });
  }

  Worker(const Worker& other) = delete;
  Worker& operator=(const Worker& rhs) = delete;
  Worker(Worker&& other) = delete;
  Worker& operator=(Worker&& rhs) = delete;
  ~Worker() {
    setDone(true);
    if (m_processingThread.joinable()) {
      m_processingThread.join();
    }
  }
  void ProcessNextImage(cv::Mat image) {
    cv::Size imageSize;
    imageSize = image.size();

    std::vector<cv::Point2f> charucoCorners;
    std::vector<int> charucoIds;
    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<int> markerIds;

    std::vector<cv::Point3f> objPoints;
    std::vector<cv::Point2f> imgPoints;

    std::vector<mrcal_point3_t> points((m_boardWidth - 1) *
                                       (m_boardHeight - 1));

    m_charucoDetector->detectBoard(image, charucoCorners, charucoIds,
                                   markerCorners, markerIds);

    if (!filter(charucoCorners, charucoIds, markerCorners, markerIds,
                m_boardWidth, m_boardHeight)) {
      return;
    }

    m_charucoBoard->matchImagePoints(charucoCorners, charucoIds, objPoints,
                                     imgPoints);

    for (int i = 0; i < charucoIds.size(); i++) {
      int id = charucoIds.at(i);
      points[id].x = imgPoints.at(i).x;
      points[id].y = imgPoints.at(i).y;
      points[id].z = 1.0f;
    }

    for (int i = 0; i < points.size(); i++) {
      if (points[i].z != 1.0f) {
        points[i].x = -1.0f;
        points[i].y = -1.0f;
        points[i].z = -1.0f;
      }
    }

    cv::Size boardSize(m_boardWidth - 1, m_boardHeight - 1);
    framesRtToref.push_back(getSeedPose(points.data(), boardSize, imageSize,
                                        squareWidth * 0.0254, 1000));
    observationBoards.insert(observationBoards.end(), points.begin(),
                             points.end());
  }

  auto GetData() { return std::make_pair(observationBoards, framesRtToref); }
  void setDone(bool isDone) { this->isDone = isDone; }
  int GetProcessedFrameCount() { return framesProcessed; }

 private:
  std::atomic_bool isDone{false};
  std::atomic_int framesProcessed{0};
  std::function<std::optional<cv::Mat>()> m_getMat;
  int m_boardWidth;
  int m_boardHeight;
  double squareWidth;
  std::vector<mrcal_point3_t> observationBoards;
  std::vector<mrcal_pose_t> framesRtToref;
  std::vector<std::vector<cv::Point3f>> objPoints;
  std::vector<std::vector<cv::Point2f>> imgPoints;
  // ChArUco Board
  std::unique_ptr<cv::aruco::Dictionary> m_arucoDict =
      std::make_unique<cv::aruco::Dictionary>(
          cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000));
  std::unique_ptr<cv::aruco::CharucoBoard> m_charucoBoard;
  std::unique_ptr<cv::aruco::CharucoDetector> m_charucoDetector;
  std::thread m_processingThread;
};
class VideoProcessor {
 public:
  VideoProcessor(size_t numWorkers, double squareWidth, double markerWidth,
                 int boardWidth, int boardHeight)
      : boardSize(boardWidth - 1, boardHeight - 1), squareWidth(squareWidth) {
    for (size_t i = 0; i < numWorkers; i++) {
      m_workers.emplace_back(std::make_unique<Worker>(
          [this] { return this->GetFrame(); }, squareWidth, markerWidth,
          boardWidth, boardHeight));
    }
  }
  bool IsFinished() { return isFinished; }
  std::optional<cameracalibration::CameraModel> GetCameraModel() {
    return cameraModel;
  }
  std::optional<cv::Mat> GetFrame() {
    std::scoped_lock lock(m_mutex);
    if (m_queue.empty()) {
      return std::nullopt;
    } else {
      cv::Mat mat = m_queue.front();
      m_queue.pop();
      return mat;
    }
  }
  void DisplayProgressBar() {
    int totalProcessedFrames = 0;
    for (auto& workers : m_workers) {
      totalProcessedFrames += workers->GetProcessedFrameCount();
    }
    std::cout << totalProcessedFrames << "/" << m_totalFrames << " processed\n";
  }
  void ProcessVideo(std::string& videoPath, bool useMrcal) {
    m_processingThread =
        std::thread([=, this, capture = cv::VideoCapture{videoPath}]() mutable {
          this->m_totalFrames = capture.get(cv::CAP_PROP_FRAME_COUNT);
          cv::Size frameShape{
              static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT)),
              static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH))};
          while (capture.grab()) {
            cv::Mat frame;
            capture.retrieve(frame);
            cv::Mat frameGray;
            cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
            m_queue.push(std::move(frameGray));
          }
          for (auto& worker : m_workers) {
            allObservationBoards.insert(allObservationBoards.end(),
                                        worker->GetData().first.begin(),
                                        worker->GetData().first.end());
            allFramesRtToref.insert(allFramesRtToref.end(),
                                    worker->GetData().second.begin(),
                                    worker->GetData().second.end());
          }

          auto result =
              mrcal_main(allObservationBoards, allFramesRtToref, boardSize,
                         squareWidth * 0.0254, frameShape, 1000);
          cameraModel = MrcalResultToCameraModel(*result);
          isFinished = true;
        });
  }

 private:
  cv::Size boardSize;
  double squareWidth;
  std::optional<CameraModel> cameraModel;
  std::vector<mrcal_point3_t> allObservationBoards;
  std::vector<mrcal_pose_t> allFramesRtToref;
  std::vector<std::vector<cv::Point3f>> allObjPoints;
  std::vector<std::vector<cv::Point2f>> allImgPoints;
  std::atomic_bool isFinished{false};
  std::queue<cv::Mat> m_queue;
  std::atomic_int m_totalFrames;
  std::thread m_processingThread;
  wpi::mutex m_mutex;
  std::vector<std::unique_ptr<Worker>> m_workers;
};
void to_json(wpi::json& json, const CameraModel& cameraModel);

void from_json(const wpi::json& json, CameraModel& cameraModel);
}  // namespace cameracalibration
