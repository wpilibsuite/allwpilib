// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameracalibration.h"

#include <fstream>
#include <memory>
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
  std::vector<double> cameraMatrix = {
      // fx 0 cx
      stats.intrinsics[0], 0, stats.intrinsics[2],
      // 0 fy cy
      0, stats.intrinsics[1], stats.intrinsics[3],
      // 0 0 1
      0, 0, 1};

  std::vector<double> distortionCoefficients = {
      stats.intrinsics[4],  stats.intrinsics[5], stats.intrinsics[6],
      stats.intrinsics[7],  stats.intrinsics[8], stats.intrinsics[9],
      stats.intrinsics[10], stats.intrinsics[11]};

  return {Eigen::Matrix<double, 3, 3>(cameraMatrix.data()),
          Eigen::Matrix<double, 8, 1>(distortionCoefficients.data()),
          stats.rms_error};
}

Worker::Worker(std::function<std::optional<cv::Mat>()> getMat,
               float squareWidth, float markerWidth, int boardWidth,
               int boardHeight)
    : m_getMat(std::move(getMat)),
      m_boardWidth(boardWidth),
      m_boardHeight(boardHeight),
      m_squareWidth(squareWidth * 0.0254),
      m_charucoBoard(cv::Size(boardWidth, boardHeight), squareWidth * 0.0254,
                     markerWidth * 0.0254, m_arucoDict),
      m_charucoDetector(m_charucoBoard) {
  m_processingThread = std::thread([this] {
    while (!m_isDone) {
      std::optional<cv::Mat> mat = m_getMat();
      if (mat) {
        ProcessNextImage(*mat);
        ++m_framesProcessed;
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  });
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

void Worker::Stop() {
  m_isDone = true;
  if (m_processingThread.joinable()) {
    m_processingThread.detach();
  }
}

CameraCalibrator::CameraCalibrator(size_t numWorkers, double squareWidth,
                                   double markerWidth, int boardWidth,
                                   int boardHeight)
    : m_cornerSize(boardWidth - 1, boardHeight - 1),
      m_squareWidth(squareWidth) {
  for (size_t i = 0; i < numWorkers; i++) {
    m_workers.emplace_back(std::make_unique<Worker>(
        [this] { return this->GetFrame(); }, m_squareWidth, markerWidth,
        boardWidth, boardHeight));
  }
}

CameraCalibrator::~CameraCalibrator() {
  Stop();
}

std::optional<cv::Mat> CameraCalibrator::GetFrame() {
  std::scoped_lock lock(m_mutex);
  if (m_queue.empty()) {
    return std::nullopt;
  } else {
    cv::Mat mat = m_queue.front();
    m_queue.pop();
    return mat;
  }
}

void CameraCalibrator::AddFrame(cv::Mat&& mat) {
  std::scoped_lock lock(m_mutex);
  m_queue.push(std::move(mat));
}

int CameraCalibrator::TotalFramesProcessed() {
  int totalProcessedFrames = 0;
  for (auto& workers : m_workers) {
    totalProcessedFrames += workers->GetProcessedFrameCount();
  }
  return totalProcessedFrames;
}

void CameraCalibrator::ProcessVideo(std::string& videoPath) {
  cv::VideoCapture cap{videoPath};
  m_totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
  m_processingThread = std::thread([this, capture = std::move(cap)]() mutable {
    cv::Size frameShape{
        static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH)),
        static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT))};
    while (capture.grab() && !m_isFinished) {
      cv::Mat frame;
      capture.retrieve(frame);
      cv::Mat frameGray;
      cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
      AddFrame(std::move(frameGray));
    }
    while (TotalFramesProcessed() < this->m_totalFrames) {
      if (m_isFinished) {
        m_cameraModel = std::nullopt;
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (m_isFinished) {
      m_cameraModel = std::nullopt;
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
    auto result =
        mrcal_main(allObservationBoards, allFramesRtToref, m_cornerSize,
                   m_squareWidth * 0.0254, frameShape, 1000);
    m_cameraModel = MrcalResultToCameraModel(*result);
    m_isFinished = true;
  });
}

void CameraCalibrator::Stop() {
  for (auto& workers : m_workers) {
    m_isFinished = true;
    workers->Stop();
  }
  if (m_processingThread.joinable()) {
    m_processingThread.detach();
  }
}

void to_json(wpi::json& json, const CameraModel& cameraModel) {
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

void from_json(const wpi::json& json, CameraModel& cameraModel) {
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
