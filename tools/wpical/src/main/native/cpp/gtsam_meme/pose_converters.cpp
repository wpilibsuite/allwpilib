// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "pose_converters.h"

using namespace wpical;
using namespace gtsam;

Pose3 wpical::Pose3dToGtsamPose3(wpi::math::Pose3d pose) {
  const auto q = pose.Rotation().GetQuaternion();
  return Pose3{Rot3(q.W(), q.X(), q.Y(), q.Z()),
               Point3(pose.X().to<double>(), pose.Y().to<double>(),
                      pose.Z().to<double>())};
}

wpi::math::Pose3d wpical::GtsamToFrcPose3d(gtsam::Pose3 pose) {
  return wpi::math::Pose3d{wpi::math::Translation3d{units::meter_t{pose.x()},
                                                    units::meter_t{pose.y()},
                                                    units::meter_t{pose.z()}},
                           wpi::math::Rotation3d{pose.rotation().matrix()}};
}

wpi::math::Pose3d wpical::ToPose3d(const cv::Mat& tvec, const cv::Mat& rvec) {
  using namespace wpi::math;
  using namespace wpi::units;

  // cv::Mat R;
  // cv::Rodrigues(rvec, R); // R is 3x3
  // R = R.t();                 // rotation of inverse
  // cv::Mat tvecI = -R * tvec; // translation of inverse

  Eigen::Matrix<double, 3, 1> tv;
  tv[0] = tvec.at<double>(0, 0);
  tv[1] = tvec.at<double>(1, 0);
  tv[2] = tvec.at<double>(2, 0);
  Eigen::Matrix<double, 3, 1> rv;
  rv[0] = rvec.at<double>(0, 0);
  rv[1] = rvec.at<double>(1, 0);
  rv[2] = rvec.at<double>(2, 0);

  return Pose3d(Translation3d(meter_t{tv[0]}, meter_t{tv[1]}, meter_t{tv[2]}),
                Rotation3d(rv));
}
