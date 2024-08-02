#pragma once

#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/objdetect/aruco_board.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <mrcal_wrapper.h>

namespace cameracalibration
{
    nlohmann::json calibrate(const std::string &input_video, float square_width, float marker_width, int board_width, int board_height);
    nlohmann::json calibrate(const std::string &input_video, float square_width, int board_width, int board_height, double imagerWidthPixels, double imagerHeightPixels, double focal_length_guess);
}

#endif