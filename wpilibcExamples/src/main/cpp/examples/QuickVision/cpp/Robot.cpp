// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include <cameraserver/CameraServer.h>
#include <frc/TimedRobot.h>

/**
 * Uses the CameraServer class to automatically capture video from a USB webcam
 * and send it to the FRC dashboard without doing any vision processing. This is
 * the easiest way to get camera images to the dashboard. Just add this to the
 * robot class constructor.
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
#if defined(__linux__) || defined(_WIN32)
    frc::CameraServer::StartAutomaticCapture();
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
