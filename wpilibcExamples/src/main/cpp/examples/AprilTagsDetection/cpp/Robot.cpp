// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <span>
#include <sstream>
#include <string>
#include <thread>

#include <cameraserver/CameraServer.h>
#include <frc/TimedRobot.h>
#include <frc/apriltag/AprilTagDetection.h>
#include <frc/apriltag/AprilTagDetector.h>
#include <frc/apriltag/AprilTagPoseEstimator.h>
#include <frc/geometry/Transform3d.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <units/angle.h>
#include <units/length.h>

/**
 * This is a demo program showing the detection of AprilTags.
 * The image is acquired from the USB camera, then any detected AprilTags
 * are marked up on the image and sent to the dashboard.
 */
class Robot : public frc::TimedRobot {
#if defined(__linux__) || defined(_WIN32)

 private:
  static void VisionThread() {
    frc::AprilTagDetector detector;
    // look for tag16h5, don't correct any error bits
    detector.AddFamily("tag16h5", 0);

    // Set up Pose Estimator - parameters are for a Microsoft Lifecam HD-3000
    // (https://www.chiefdelphi.com/t/wpilib-apriltagdetector-sample-code/421411/21)
    frc::AprilTagPoseEstimator::Config poseEstConfig = {
        .tagSize = units::length::inch_t(6.0),
        .fx = 699.3778103158814,
        .fy = 677.7161226393544,
        .cx = 345.6059345433618,
        .cy = 207.12741326228522};
    frc::AprilTagPoseEstimator estimator =
        frc::AprilTagPoseEstimator(poseEstConfig);

    // Get the USB camera from CameraServer
    cs::UsbCamera camera = frc::CameraServer::StartAutomaticCapture();
    // Set the resolution
    camera.SetResolution(640, 480);

    // Get a CvSink. This will capture Mats from the Camera
    cs::CvSink cvSink = frc::CameraServer::GetVideo();
    // Setup a CvSource. This will send images back to the Dashboard
    cs::CvSource outputStream =
        frc::CameraServer::PutVideo("Detected", 640, 480);

    // Mats are very memory expensive. Lets reuse this Mat.
    cv::Mat mat;
    cv::Mat grayMat;

    // Instantiate once
    std::vector<int> tags;
    cv::Scalar outlineColor = cv::Scalar(0, 255, 0);
    cv::Scalar crossColor = cv::Scalar(0, 0, 255);

    while (true) {
      // Tell the CvSink to grab a frame from the camera and
      // put it
      // in the source mat.  If there is an error notify the
      // output.
      if (cvSink.GrabFrame(mat) == 0) {
        // Send the output the error.
        outputStream.NotifyError(cvSink.GetError());
        // skip the rest of the current iteration
        continue;
      }

      cv::cvtColor(mat, grayMat, cv::COLOR_BGR2GRAY);

      cv::Size g_size = grayMat.size();
      frc::AprilTagDetector::Results detections =
          detector.Detect(g_size.width, g_size.height, grayMat.data);

      // have not seen any tags yet
      tags.clear();

      for (const frc::AprilTagDetection* detection : detections) {
        // remember we saw this tag
        tags.push_back(detection->GetId());

        // draw lines around the tag
        for (int i = 0; i <= 3; i++) {
          int j = (i + 1) % 4;
          const frc::AprilTagDetection::Point pti = detection->GetCorner(i);
          const frc::AprilTagDetection::Point ptj = detection->GetCorner(j);
          line(mat, cv::Point(pti.x, pti.y), cv::Point(ptj.x, ptj.y),
              outlineColor, 2);
        }

        // mark the center of the tag
        const frc::AprilTagDetection::Point c = detection->GetCenter();
        int ll = 10;
        line(mat, cv::Point(c.x - ll, c.y), cv::Point(c.x + ll, c.y),
             crossColor, 2);
        line(mat, cv::Point(c.x, c.y - ll), cv::Point(c.x, c.y + ll),
             crossColor, 2);

        // identify the tag
        putText(mat, std::to_string(detection->GetId()),
                cv::Point(c.x + ll, c.y), cv::FONT_HERSHEY_SIMPLEX, 1,
                crossColor, 3);

        // determine pose
        frc::Transform3d pose = estimator.Estimate(*detection);

        // put pose into dashbaord
        std::stringstream dashboardString;
        dashboardString << "Translation: " << units::length::to_string(pose.X())
                        << ", " << units::length::to_string(pose.Y()) << ", "
                        << units::length::to_string(pose.Z());
        frc::Rotation3d rotation = pose.Rotation();
        dashboardString << "; Rotation: "
                        << units::angle::to_string(rotation.X()) << ", "
                        << units::angle::to_string(rotation.Y()) << ", " 
                        << units::angle::to_string(rotation.Z());
        frc::SmartDashboard::PutString(
            "pose_" + std::to_string(detection->GetId()),
            dashboardString.str());
      }

      // put list of tags onto dashboard
      std::stringstream tags_s;
      if (tags.size() > 0) {
        if (tags.size() > 1) {
          std::copy(tags.begin(), tags.end() - 1,
                    std::ostream_iterator<int>(tags_s, ","));
        }
        tags_s << tags.back();
      }
      frc::SmartDashboard::PutString("tags", tags_s.str());

      // Give the output stream a new image to display
      outputStream.PutFrame(mat);
    }
  }
#endif

  void RobotInit() override {
    // We need to run our vision program in a separate thread. If not, our robot
    // program will not run.
#if defined(__linux__) || defined(_WIN32)
    std::thread visionThread(VisionThread);
    visionThread.detach();
#else
    std::fputs("Vision only available on Linux or Windows.\n", stderr);
    std::fflush(stderr);
#endif
  }
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
