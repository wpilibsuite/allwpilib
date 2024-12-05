// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <mrcal_wrapper.h>

using namespace cv;

struct cmpByFilename {
  bool operator()(const std::string& a, const std::string& b) const {
    auto a2 = std::stoi(a.substr(3, a.length() - 7));
    auto b2 = std::stoi(b.substr(3, b.length() - 7));
    // std::cout << a2 << " _ " << b2 << std::endl;
    return a2 < b2;
  }
};

std::vector<double> calibrate(const std::string& fname, cv::Size boardSize,
                              cv::Size imagerSize) {
  std::ifstream file(fname);
  if (!file.is_open()) {
    std::cerr << "Unable to open file corners.vnl" << std::endl;
    return {};
  }

  std::string line;
  std::map<std::string, std::vector<mrcal_point3_t>> points;
  while (std::getline(file, line)) {
    if (line.starts_with("#")) {
      continue;
    }

    if (line.empty()) {
      continue;
    }

    try {
      std::istringstream iss(line);
      std::string filename, x_str, y_str, level_str;

      iss >> filename >> x_str >> y_str >> level_str;
      using std::stod;

      // From calibration.py:
      // if weight_column_kind == 'level': the 4th column is a decimation level
      // of the
      //   detected corner. If we needed to cut down the image resolution to
      //   detect a corner, its coordinates are known less precisely, and we use
      //   that information to weight the errors appropriately later. We set the
      //   output weight = 1/2^level. If the 4th column is '-' or <0, the given
      //   point was not detected, and should be ignored: we set weight = -1

      double level;
      if (level_str == "-") {
        level = -1;
      } else {
        level = stod(level_str);
      }
      double weight;
      if (level < 0) {
        weight = -1;
      } else {
        weight = std::pow(0.5, level);
      }
      points[filename].push_back(
          mrcal_point3_t{stod(x_str), stod(y_str), weight});
      // std::printf("put %s\n", *name);
    } catch (std::exception const& e) {
      std::perror(e.what());
    }
  }

  file.close();

  auto start = std::chrono::steady_clock::now();

  std::vector<mrcal_point3_t> observations_board;
  std::vector<mrcal_pose_t> frames_rt_toref;
  // Pre-allocate worst case
  size_t total_points = boardSize.width * boardSize.height * points.size();
  observations_board.reserve(total_points);
  frames_rt_toref.reserve(points.size());

  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  for (const auto& [key, value] : points) {
    if (value.size()) {
      auto ret = getSeedPose(value.data(), boardSize, imagerSize, 0.03, 1200);

      // Append to the Big List of board corners/levels
      observations_board.insert(observations_board.end(), value.begin(),
                                value.end());
      // And list of pose seeds
      frames_rt_toref.push_back(ret);
    } else {
      std::printf("No points for %s\n", key.c_str());
    }
  }

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Seed took: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     begin)
                   .count()
            << "[ms]" << std::endl;

  auto cal_result = mrcal_main(observations_board, frames_rt_toref, boardSize,
                               0.030, imagerSize, 1200);

  auto dt = std::chrono::steady_clock::now() - start;
  int dt_ms = dt.count();

  auto& stats = *cal_result;

  double max_error =
      *std::max_element(stats.residuals.begin(), stats.residuals.end());

  if (1) {
    std::printf("\n===============================\n\n");
    std::printf("RMS Reprojection Error: %.2f pixels\n", stats.rms_error);
    std::printf("Worst residual (by measurement): %.1f pixels\n", max_error);
    std::printf("Noutliers: %i of %zu (%.1f percent of the data)\n",
                stats.Noutliers_board, total_points,
                100.0 * stats.Noutliers_board / total_points);
    std::printf("calobject_warp: [%f, %f]\n", stats.calobject_warp.x2,
                stats.calobject_warp.y2);
    std::printf("dt, seeding + solve: %f ms\n", dt_ms / 1e6);
    std::printf("Intrinsics [%zu]: ", stats.intrinsics.size());
    for (auto i : stats.intrinsics)
      std::printf("%f ", i);
    std::printf("\n");
  }

  return stats.intrinsics;
}

const std::string projectRootPath = PROJECT_ROOT_PATH;

TEST(MrcalResultExactlyMatchesTest, lifecam_1280) {
  auto calculated_intrinsics{calibrate(
      projectRootPath + "/lifecam_1280p_10x10.vnl", {10, 10}, {1280, 720})};

  // ## generated with mrgingham --jobs 4 --gridn 10
  // /home/mmorley/photonvision/test-resources/calibrationSquaresImg/lifecam/2024-01-02_lifecam_1280/*.png
  // # generated on 2024-12-01 11:51:55 with   /usr/bin/mrcal-calibrate-cameras
  // --corners-cache corners.vnl --lensmodel LENSMODEL_OPENCV8 --focal 1200
  // --object-spacing 0.0254 --object-width-n 10 '*.png'
  std::vector<double> mrcal_cli_groundtruth_intrinsics{
      1130.892767,  1129.149453,    609.9417222,    349.6457279,
      0.1489878273, -1.348622726,   0.002839630852, 0.001135629909,
      2.560627057,  -0.03170208336, 0.0695788644,   -0.09547554864};

  for (int i = 0; i < 12; i++) {
    EXPECT_NEAR(mrcal_cli_groundtruth_intrinsics[i], calculated_intrinsics[i],
                1e-6);
  }
}
