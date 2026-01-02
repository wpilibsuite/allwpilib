// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameracalibration.hpp"

#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <mrcal_wrapper.h>
#include <opencv2/core/eigen.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

static bool filter(std::vector<cv::Point2f> charucoCorners,
                   std::vector<int> charucoIds, std::vector<int> markerIds,
                   int boardWidth, int boardHeight) {
  if (charucoIds.empty() || charucoCorners.empty()) {
    return false;
  }

  if (charucoCorners.size() < 10 || charucoIds.size() < 10) {
    return false;
  }

  for (int i = 0; i < charucoIds.size(); i++) {
    if (charucoIds.at(i) > (boardWidth - 1) * (boardHeight - 1) - 1) {
      return false;
    }
  }

  for (int i = 0; i < markerIds.size(); i++) {
    if (markerIds.at(i) > ((boardWidth * boardHeight) / 2) - 1) {
      return false;
    }
  }

  return true;
}

namespace wpical {
CameraModel MrcalResultToCameraModel(mrcal_result& stats) {
  // Camera matrix and distortion coefficients
  return {Eigen::Matrix<double, 3, 3, Eigen::RowMajor>{
              // fx 0 cx
              {stats.intrinsics[0], 0, stats.intrinsics[2]},
              // 0 fy cy
              {0, stats.intrinsics[1], stats.intrinsics[3]},
              // 0 0 1
              {0, 0, 1}},
          Eigen::Matrix<double, 8, 1>{
              stats.intrinsics[4], stats.intrinsics[5], stats.intrinsics[6],
              stats.intrinsics[7], stats.intrinsics[8], stats.intrinsics[9],
              stats.intrinsics[10], stats.intrinsics[11]},
          stats.rms_error};
}

/**
 * Container for data that's shared between threads.
 */
class Data {
 public:
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

  std::optional<CameraModel> cameraModel;
  std::queue<cv::Mat> queue;
  wpi::util::mutex mutex;
};

void Data::AddFrame(cv::Mat&& mat) {
  std::scoped_lock lock(mutex);
  queue.push(std::move(mat));
}

std::optional<cv::Mat> Data::GetFrame() {
  std::scoped_lock lock(mutex);
  if (queue.empty()) {
    return std::nullopt;
  } else {
    cv::Mat mat = queue.front();
    queue.pop();
    return mat;
  }
}

class Worker {
 public:
  /**
   * @param data A pointer to the queue of Mats.
   * @param squareWidth The width of the full square in meters
   * @param markerWidth The width of the marker in meters
   * @param boardWidth How many markers wide the board is
   * @param boardHeight How many markers tall the board is
   */
  explicit Worker(std::shared_ptr<Data> data, float squareWidth,
                  float markerWidth, int boardWidth, int boardHeight);

  ~Worker();

  void ProcessNextImage(cv::Mat image);

  auto GetData() {
    return std::make_pair(m_observationBoards, m_framesRtToref);
  }

  int GetProcessedFrameCount();

  void Stop();

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
};

Worker::Worker(std::shared_ptr<Data> data, float squareWidth, float markerWidth,
               int boardWidth, int boardHeight)
    : m_boardWidth(boardWidth),
      m_boardHeight(boardHeight),
      m_squareWidth(squareWidth * 0.0254),
      m_charucoBoard(cv::Size(boardWidth, boardHeight), squareWidth * 0.0254,
                     markerWidth * 0.0254, m_arucoDict),
      m_charucoDetector(m_charucoBoard) {
  std::thread([this, data] {
    while (!m_isDone) {
      std::optional<cv::Mat> mat = data->GetFrame();
      if (mat) {
        ProcessNextImage(*mat);
        ++m_framesProcessed;
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  }).detach();
}

Worker::~Worker() {
  Stop();
}

void Worker::ProcessNextImage(cv::Mat image) {
  cv::Size imageSize = image.size();

  std::vector<cv::Point2f> charucoCorners;
  std::vector<int> charucoIds;
  std::vector<std::vector<cv::Point2f>> markerCorners;
  std::vector<int> markerIds;

  std::vector<cv::Point3f> objPoints;
  std::vector<cv::Point2f> imgPoints;

  std::vector<mrcal_point3_t> points((m_boardWidth - 1) * (m_boardHeight - 1));

  m_charucoDetector.detectBoard(image, charucoCorners, charucoIds,
                                markerCorners, markerIds);

  if (!filter(charucoCorners, charucoIds, markerIds, m_boardWidth,
              m_boardHeight)) {
    return;
  }

  m_charucoBoard.matchImagePoints(charucoCorners, charucoIds, objPoints,
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
  m_framesRtToref.push_back(
      getSeedPose(points.data(), boardSize, imageSize, m_squareWidth, 1000));
  m_observationBoards.insert(m_observationBoards.end(), points.begin(),
                             points.end());
}

int Worker::GetProcessedFrameCount() {
  return m_framesProcessed;
}

void Worker::Stop() {
  m_isDone = true;
}

CameraCalibrator::CameraCalibrator(size_t numWorkers, double squareWidth,
                                   double markerWidth, int boardWidth,
                                   int boardHeight, std::string& videoPath)
    : m_state(std::make_shared<Data>()) {
  for (size_t i = 0; i < numWorkers; i++) {
    m_workers.emplace_back(std::make_unique<Worker>(
        m_state, squareWidth, markerWidth, boardWidth, boardHeight));
  }
  cv::VideoCapture cap{videoPath};
  m_totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
  std::thread([this, boardHeight, boardWidth, squareWidth, state = m_state,
               capture = std::move(cap)]() mutable {
    cv::Size frameShape{
        static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH)),
        static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT))};
    while (capture.grab() && !m_isFinished) {
      cv::Mat frame;
      capture.retrieve(frame);
      cv::Mat frameGray;
      cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
      state->AddFrame(std::move(frameGray));
    }
    while (TotalFramesProcessed() < TotalFrames() && !m_isFinished) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (m_isFinished) {
      state->cameraModel = std::nullopt;
      return;
    }
    std::vector<mrcal_point3_t> allObservationBoards;
    std::vector<mrcal_pose_t> allFramesRtToref;
    for (auto& worker : m_workers) {
      auto data = worker->GetData();
      allObservationBoards.insert(allObservationBoards.end(),
                                  data.first.begin(), data.first.end());
      allFramesRtToref.insert(allFramesRtToref.end(), data.second.begin(),
                              data.second.end());
    }
    if (allObservationBoards.empty()) {
      m_isFinished = true;
      return;
    }
    auto result = mrcal_main(allObservationBoards, allFramesRtToref,
                             cv::Size(boardWidth - 1, boardHeight - 1),
                             squareWidth * 0.0254, frameShape, 1000);
    state->cameraModel = MrcalResultToCameraModel(*result);
    m_isFinished = true;
  }).detach();
}

CameraCalibrator::~CameraCalibrator() {
  Stop();
}

bool CameraCalibrator::IsFinished() {
  return m_isFinished;
}

std::optional<CameraModel> CameraCalibrator::GetCameraModel() {
  return m_state->cameraModel;
}

int CameraCalibrator::TotalFramesProcessed() {
  int totalProcessedFrames = 0;
  for (auto& workers : m_workers) {
    totalProcessedFrames += workers->GetProcessedFrameCount();
  }
  return totalProcessedFrames;
}

int CameraCalibrator::TotalFrames() {
  return m_totalFrames;
}

void CameraCalibrator::Stop() {
  m_isFinished = true;
  for (auto& workers : m_workers) {
    workers->Stop();
  }
}

void to_json(wpi::util::json& json, const CameraModel& cameraModel) {
  std::vector<double> cameraMatrix(
      cameraModel.intrinsicMatrix.data(),
      cameraModel.intrinsicMatrix.data() + cameraModel.intrinsicMatrix.size());
  std::vector<double> distortionCoefficients(
      cameraModel.distortionCoefficients.data(),
      cameraModel.distortionCoefficients.data() +
          cameraModel.distortionCoefficients.size());
  json = {{"camera_matrix", cameraMatrix},
          {"distortion_coefficients", distortionCoefficients},
          {"avg_reprojection_error", cameraModel.avgReprojectionError}};
}

void from_json(const wpi::util::json& json, CameraModel& cameraModel) {
  bool isCalibdb = json.contains("camera");
  Eigen::Matrix3d cameraMatrix;
  std::vector<double> distortionCoeffs;
  auto mat = json.at("camera_matrix");
  auto distortionCoefficients = json.at("distortion_coefficients");
  if (isCalibdb) {
    // OpenCV format has data key
    if (mat.contains("data")) {
      auto data = mat.at("data").get<std::vector<double>>();
      cameraMatrix = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>{data.data()};
    } else {
      for (int i = 0; i < cameraMatrix.rows(); i++) {
        for (int j = 0; j < cameraMatrix.cols(); j++) {
          cameraMatrix(i, j) = mat[i][j];
        }
      }
    }

    // OpenCV format has data key
    if (distortionCoefficients.contains("data")) {
      distortionCoeffs =
          distortionCoefficients.at("data").get<std::vector<double>>();
    } else {
      distortionCoeffs = distortionCoefficients.get<std::vector<double>>();
    }
  } else {
    cameraMatrix = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>{
        mat.get<std::vector<double>>().data()};
    distortionCoeffs = distortionCoefficients.get<std::vector<double>>();
  }
  // CalibDB generates JSONs with 5 values. Just zero out the remaining 3 to get
  // it to 8
  distortionCoeffs.resize(8, 0);
  cameraModel = {cameraMatrix,
                 Eigen::Matrix<double, 8, 1>{distortionCoeffs.data()},
                 json.at("avg_reprojection_error")};
}
}  // namespace wpical
