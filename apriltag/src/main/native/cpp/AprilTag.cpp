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

wpi::RawFrame AprilTag::Generate36h11AprilTagImage(int id) {
  apriltag_family_t* tagFamily = tag36h11_create();
  image_u8_t* image = apriltag_to_image(tagFamily, id);
  wpi::RawFrame markerFrame{};
  size_t totalDataSize = image->height * image->stride * sizeof(char);
  markerFrame.data = static_cast<char*>(
      std::calloc(image->height * image->stride, sizeof(char)));
  std::memcpy(markerFrame.data, image->buf, totalDataSize);
  markerFrame.dataLength = image->width;
  markerFrame.height = image->height;
  markerFrame.pixelFormat = WPI_PIXFMT_GRAY;
  markerFrame.width = image->stride;
  markerFrame.totalData = totalDataSize;
  image_u8_destroy(image);
  tag36h11_destroy(tagFamily);
  return markerFrame;
}

wpi::RawFrame AprilTag::Generate16h5AprilTagImage(int id) {
  apriltag_family_t* tagFamily = tag16h5_create();
  image_u8_t* image = apriltag_to_image(tagFamily, id);
  wpi::RawFrame markerFrame{};
  size_t totalDataSize = image->height * image->stride * sizeof(char);
  markerFrame.data = static_cast<char*>(
      std::calloc(image->height * image->stride, sizeof(char)));
  std::memcpy(markerFrame.data, image->buf, totalDataSize);
  markerFrame.dataLength = image->width;
  markerFrame.height = image->height;
  markerFrame.pixelFormat = WPI_PIXFMT_GRAY;
  markerFrame.width = image->stride;
  markerFrame.totalData = totalDataSize;
  image_u8_destroy(image);
  tag16h5_destroy(tagFamily);
  return markerFrame;
}

void frc::to_json(wpi::json& json, const AprilTag& apriltag) {
  json = wpi::json{{"ID", apriltag.ID}, {"pose", apriltag.pose}};
}

void frc::from_json(const wpi::json& json, AprilTag& apriltag) {
  apriltag.ID = json.at("ID").get<int>();
  apriltag.pose = json.at("pose").get<Pose3d>();
}
