// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/AprilTag.hpp"

#include <cstring>

#include "wpi/util/json.hpp"

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

static bool FamilyToImage(wpi::RawFrame* frame, apriltag_family_t* family,
                          int id) {
  image_u8_t* image = apriltag_to_image(family, id);
  size_t totalDataSize = image->height * image->stride;
  bool rv = frame->Reserve(totalDataSize);
  std::memcpy(frame->data, image->buf, totalDataSize);
  frame->size = totalDataSize;
  frame->width = image->width;
  frame->height = image->height;
  frame->stride = image->stride;
  frame->pixelFormat = WPI_PIXFMT_GRAY;
  image_u8_destroy(image);
  return rv;
}

bool AprilTag::Generate36h11AprilTagImage(wpi::RawFrame* frame, int id) {
  apriltag_family_t* tagFamily = tag36h11_create();
  bool rv = FamilyToImage(frame, tagFamily, id);
  tag36h11_destroy(tagFamily);
  return rv;
}

bool AprilTag::Generate16h5AprilTagImage(wpi::RawFrame* frame, int id) {
  apriltag_family_t* tagFamily = tag16h5_create();
  bool rv = FamilyToImage(frame, tagFamily, id);
  tag16h5_destroy(tagFamily);
  return rv;
}

void frc::to_json(wpi::json& json, const AprilTag& apriltag) {
  json = wpi::json{{"ID", apriltag.ID}, {"pose", apriltag.pose}};
}

void frc::from_json(const wpi::json& json, AprilTag& apriltag) {
  apriltag.ID = json.at("ID").get<int>();
  apriltag.pose = json.at("pose").get<Pose3d>();
}
