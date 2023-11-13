// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTag.h"

#include <wpi/json.h>

#ifdef _WIN32
#pragma warning(disable : 4200)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-extensions"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "apriltag.h"
#include "tag16h5.h"
#include "tag36h11.h"

using namespace frc;

cv::Mat AprilTag::Generate36h11AprilTagImage(int id) {
  apriltag_family_t* tagFamily = tag36h11_create();
  image_u8_t* image = apriltag_to_image(tagFamily, id);
  cv::Mat_<uint8_t> markerImage{image->height, image->stride, image->buf};
  cv::Mat_<uint8_t> markerClone = markerImage.colRange(0, image->width).clone();
  image_u8_destroy(image);
  tag36h11_destroy(tagFamily);
  return markerClone;
}

cv::Mat AprilTag::Generate16h5AprilTagImage(int id) {
  apriltag_family_t* tagFamily = tag16h5_create();
  image_u8_t* image = apriltag_to_image(tagFamily, id);
  cv::Mat_<uint8_t> markerImage{image->height, image->stride, image->buf};
  cv::Mat_<uint8_t> markerClone = markerImage.colRange(0, image->width).clone();
  image_u8_destroy(image);
  tag16h5_destroy(tagFamily);
  return markerClone;
}

void frc::to_json(wpi::json& json, const AprilTag& apriltag) {
  json = wpi::json{{"ID", apriltag.ID}, {"pose", apriltag.pose}};
}

void frc::from_json(const wpi::json& json, AprilTag& apriltag) {
  apriltag.ID = json.at("ID").get<int>();
  apriltag.pose = json.at("pose").get<Pose3d>();
}
