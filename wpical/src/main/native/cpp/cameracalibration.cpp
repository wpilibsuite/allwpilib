// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameracalibration.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <mrcal_wrapper.h>
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

static bool filter(std::vector<cv::Point2f> charuco_corners,
                   std::vector<int> charuco_ids,
                   std::vector<std::vector<cv::Point2f>> marker_corners,
                   std::vector<int> marker_ids, int board_width,
                   int board_height) {
  if (charuco_ids.empty() || charuco_corners.empty()) {
    return false;
  }

  if (charuco_corners.size() < 10 || charuco_ids.size() < 10) {
    return false;
  }

  for (int i = 0; i < charuco_ids.size(); i++) {
    if (charuco_ids.at(i) > (board_width - 1) * (board_height - 1) - 1) {
      return false;
    }
  }

  for (int i = 0; i < marker_ids.size(); i++) {
    if (marker_ids.at(i) > ((board_width * board_height) / 2) - 1) {
      return false;
    }
  }

  return true;
}

int cameracalibration::calibrate(const std::string& input_video,
                                 CameraModel& camera_model, float square_width,
                                 float marker_width, int board_width,
                                 int board_height, bool show_debug_window) {
  // ChArUco Board
  cv::aruco::Dictionary aruco_dict =
      cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
  cv::Ptr<cv::aruco::CharucoBoard> charuco_board = new cv::aruco::CharucoBoard(
      cv::Size(board_width, board_height), square_width * 0.0254,
      marker_width * 0.0254, aruco_dict);
  cv::aruco::CharucoDetector charuco_detector(*charuco_board);

  // Video capture
  cv::VideoCapture video_capture(input_video);
  cv::Size frame_shape;

  std::vector<std::vector<cv::Point2f>> all_charuco_corners;
  std::vector<std::vector<int>> all_charuco_ids;

  std::vector<std::vector<cv::Point3f>> all_obj_points;
  std::vector<std::vector<cv::Point2f>> all_img_points;

  while (video_capture.grab()) {
    cv::Mat frame;
    video_capture.retrieve(frame);

    cv::Mat debug_image = frame;

    if (frame.empty()) {
      break;
    }

    // Detect
    cv::Mat frame_gray;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

    frame_shape = frame_gray.size();

    std::vector<cv::Point2f> charuco_corners;
    std::vector<int> charuco_ids;
    std::vector<std::vector<cv::Point2f>> marker_corners;
    std::vector<int> marker_ids;

    std::vector<cv::Point3f> obj_points;
    std::vector<cv::Point2f> img_points;

    charuco_detector.detectBoard(frame_gray, charuco_corners, charuco_ids,
                                 marker_corners, marker_ids);

    if (!filter(charuco_corners, charuco_ids, marker_corners, marker_ids,
                board_width, board_height)) {
      continue;
    }

    charuco_board->matchImagePoints(charuco_corners, charuco_ids, obj_points,
                                    img_points);

    all_charuco_corners.push_back(charuco_corners);
    all_charuco_ids.push_back(charuco_ids);

    all_obj_points.push_back(obj_points);
    all_img_points.push_back(img_points);

    if (show_debug_window) {
      cv::aruco::drawDetectedMarkers(debug_image, marker_corners, marker_ids);
      cv::aruco::drawDetectedCornersCharuco(debug_image, charuco_corners,
                                            charuco_ids);
      cv::imshow("Frame", debug_image);
      if (cv::waitKey(1) == 'q') {
        break;
      }
    }
  }

  video_capture.release();
  if (show_debug_window) {
    cv::destroyAllWindows();
  }

  // Calibrate
  cv::Mat camera_matrix, dist_coeffs;
  std::vector<cv::Mat> r_vecs, t_vecs;
  std::vector<double> std_dev_intrinsics, std_dev_extrinsics, per_view_errors;
  double repError;

  try {
    // see https://stackoverflow.com/a/75865177
    int flags = cv::CALIB_RATIONAL_MODEL | cv::CALIB_USE_LU;
    repError = cv::calibrateCamera(
        all_obj_points, all_img_points, frame_shape, camera_matrix, dist_coeffs,
        r_vecs, t_vecs, cv::noArray(), cv::noArray(), cv::noArray(), flags);
  } catch (...) {
    std::cout << "calibration failed" << std::endl;
    return 1;
  }

  std::vector<double> matrix = {camera_matrix.begin<double>(),
                                camera_matrix.end<double>()};

  std::vector<double> distortion = {dist_coeffs.begin<double>(),
                                    dist_coeffs.end<double>()};

  camera_model.intrinsic_matrix = Eigen::Matrix<double, 3, 3>(matrix.data());
  camera_model.distortion_coefficients =
      Eigen::Matrix<double, 8, 1>(distortion.data());
  camera_model.avg_reprojection_error = repError;
  return 0;
}

int cameracalibration::calibrate(const std::string& input_video,
                                 CameraModel& camera_model, float square_width,
                                 float marker_width, int board_width,
                                 int board_height, double imagerWidthPixels,
                                 double imagerHeightPixels,
                                 bool show_debug_window) {
  // ChArUco Board
  cv::aruco::Dictionary aruco_dict =
      cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
  cv::Ptr<cv::aruco::CharucoBoard> charuco_board = new cv::aruco::CharucoBoard(
      cv::Size(board_width, board_height), square_width * 0.0254,
      marker_width * 0.0254, aruco_dict);
  cv::aruco::CharucoDetector charuco_detector(*charuco_board);

  // Video capture
  cv::VideoCapture video_capture(input_video);
  cv::Size frame_shape;

  // Detection output
  std::vector<mrcal_point3_t> observation_boards;
  std::vector<mrcal_pose_t> frames_rt_toref;

  cv::Size boardSize(board_width - 1, board_height - 1);
  cv::Size imagerSize(imagerWidthPixels, imagerHeightPixels);

  while (video_capture.grab()) {
    cv::Mat frame;
    video_capture.retrieve(frame);

    cv::Mat debug_image = frame;

    if (frame.empty()) {
      break;
    }

    // Detect
    cv::Mat frame_gray;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

    frame_shape = frame_gray.size();

    std::vector<cv::Point2f> charuco_corners;
    std::vector<int> charuco_ids;
    std::vector<std::vector<cv::Point2f>> marker_corners;
    std::vector<int> marker_ids;

    std::vector<cv::Point3f> obj_points;
    std::vector<cv::Point2f> img_points;

    std::vector<mrcal_point3_t> points((board_width - 1) * (board_height - 1));

    charuco_detector.detectBoard(frame_gray, charuco_corners, charuco_ids,
                                 marker_corners, marker_ids);

    if (!filter(charuco_corners, charuco_ids, marker_corners, marker_ids,
                board_width, board_height)) {
      continue;
    }

    charuco_board->matchImagePoints(charuco_corners, charuco_ids, obj_points,
                                    img_points);

    for (int i = 0; i < charuco_ids.size(); i++) {
      int id = charuco_ids.at(i);
      points[id].x = img_points.at(i).x;
      points[id].y = img_points.at(i).y;
      points[id].z = 1.0f;
    }

    for (int i = 0; i < points.size(); i++) {
      if (points[i].z != 1.0f) {
        points[i].x = -1.0f;
        points[i].y = -1.0f;
        points[i].z = -1.0f;
      }
    }

    frames_rt_toref.push_back(
        getSeedPose(points.data(), boardSize, imagerSize, square_width, 1000));
    observation_boards.insert(observation_boards.end(), points.begin(),
                              points.end());

    if (show_debug_window) {
      cv::aruco::drawDetectedMarkers(debug_image, marker_corners, marker_ids);
      cv::aruco::drawDetectedCornersCharuco(debug_image, charuco_corners,
                                            charuco_ids);
      cv::imshow("Frame", debug_image);
      if (cv::waitKey(1) == 'q') {
        break;
      }
    }
  }

  video_capture.release();
  if (show_debug_window) {
    cv::destroyAllWindows();
  }

  if (observation_boards.empty()) {
    std::cout << "calibration failed" << std::endl;
    return 1;
  } else {
    std::cout << "points detected" << std::endl;
  }

  std::unique_ptr<mrcal_result> cal_result =
      mrcal_main(observation_boards, frames_rt_toref, boardSize,
                 square_width * 0.0254, imagerSize, 1000);

  auto& stats = *cal_result;

  // Camera matrix and distortion coefficients
  std::vector<double> camera_matrix = {
      // fx 0 cx
      stats.intrinsics[0], 0, stats.intrinsics[2],
      // 0 fy cy
      0, stats.intrinsics[1], stats.intrinsics[3],
      // 0 0 1
      0, 0, 1};

  std::vector<double> distortion_coefficients = {stats.intrinsics[4],
                                                 stats.intrinsics[5],
                                                 stats.intrinsics[6],
                                                 stats.intrinsics[7],
                                                 stats.intrinsics[8],
                                                 stats.intrinsics[9],
                                                 stats.intrinsics[10],
                                                 stats.intrinsics[11],
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 0.0};

  camera_model.intrinsic_matrix =
      Eigen::Matrix<double, 3, 3>(camera_matrix.data());
  camera_model.distortion_coefficients =
      Eigen::Matrix<double, 8, 1>(distortion_coefficients.data());
  camera_model.avg_reprojection_error = stats.rms_error;

  return 0;
}

int cameracalibration::calibrate(const std::string& input_video,
                                 CameraModel& camera_model, float square_width,
                                 int board_width, int board_height,
                                 double imagerWidthPixels,
                                 double imagerHeightPixels,
                                 bool show_debug_window) {
  // Video capture
  cv::VideoCapture video_capture(input_video);

  // Detection output
  std::vector<mrcal_point3_t> observation_boards;
  std::vector<mrcal_pose_t> frames_rt_toref;

  cv::Size boardSize(board_width - 1, board_height - 1);
  cv::Size imagerSize(imagerWidthPixels, imagerHeightPixels);

  while (video_capture.grab()) {
    cv::Mat frame;
    video_capture.retrieve(frame);

    if (frame.empty()) {
      break;
    }

    // Detect
    cv::Mat frame_gray;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Point2f> corners;

    bool found = cv::findChessboardCorners(
        frame_gray, boardSize, corners,
        cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);

    if (found) {
      std::vector<mrcal_point3_t> current_points;
      for (int i = 0; i < corners.size(); i++) {
        current_points.push_back(
            mrcal_point3_t{{corners.at(i).x, corners.at(i).y, 1.0f}});
      }
      frames_rt_toref.push_back(getSeedPose(current_points.data(), boardSize,
                                            imagerSize, square_width * 0.0254,
                                            1000));
      observation_boards.insert(observation_boards.end(),
                                current_points.begin(), current_points.end());
    }
    if (show_debug_window) {
      cv::drawChessboardCorners(frame, boardSize, corners, found);
      cv::imshow("Checkerboard Detection", frame);
      if (cv::waitKey(30) == 'q') {
        break;
      }
    }
  }

  video_capture.release();
  if (show_debug_window) {
    cv::destroyAllWindows();
  }

  if (observation_boards.empty()) {
    std::cout << "calibration failed" << std::endl;
    return 1;
  } else {
    std::cout << "points detected" << std::endl;
  }

  std::unique_ptr<mrcal_result> cal_result =
      mrcal_main(observation_boards, frames_rt_toref, boardSize,
                 square_width * 0.0254, imagerSize, 1000);

  auto& stats = *cal_result;

  // Camera matrix and distortion coefficients
  std::vector<double> camera_matrix = {
      // fx 0 cx
      stats.intrinsics[0], 0, stats.intrinsics[2],
      // 0 fy cy
      0, stats.intrinsics[1], stats.intrinsics[3],
      // 0 0 1
      0, 0, 1};

  std::vector<double> distortion_coefficients = {stats.intrinsics[4],
                                                 stats.intrinsics[5],
                                                 stats.intrinsics[6],
                                                 stats.intrinsics[7],
                                                 stats.intrinsics[8],
                                                 stats.intrinsics[9],
                                                 stats.intrinsics[10],
                                                 stats.intrinsics[11],
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 0.0};

  // Save calibration output
  camera_model.intrinsic_matrix =
      Eigen::Matrix<double, 3, 3>(camera_matrix.data());
  camera_model.distortion_coefficients =
      Eigen::Matrix<double, 8, 1>(distortion_coefficients.data());
  camera_model.avg_reprojection_error = stats.rms_error;

  return 0;
}
