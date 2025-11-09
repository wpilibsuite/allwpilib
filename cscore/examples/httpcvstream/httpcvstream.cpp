// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include <opencv2/core/core.hpp>

#include "wpi/cs/cscore.h"
#include "wpi/cs/cscore_cv.hpp"
#include "wpi/util/print.hpp"

int main() {
  wpi::cs::HttpCamera camera{"httpcam", "http://localhost:8081/?action=stream"};
  camera.SetVideoMode(wpi::cs::VideoMode::kMJPEG, 320, 240, 30);
  wpi::cs::CvSink cvsink{"cvsink"};
  cvsink.SetSource(camera);
  wpi::cs::CvSource cvsource{"cvsource", wpi::cs::VideoMode::kMJPEG, 320, 240,
                             30};
  wpi::cs::MjpegServer cvMjpegServer{"cvhttpserver", 8083};
  cvMjpegServer.SetSource(cvsource);

  cv::Mat test;
  cv::Mat flip;
  for (;;) {
    uint64_t time = cvsink.GrabFrame(test);
    if (time == 0) {
      wpi::util::print("error: {}\n", cvsink.GetError());
      continue;
    }
    wpi::util::print("got frame at time {} size ({}, {})\n", time,
                     test.size().width, test.size().height);
    cv::flip(test, flip, 0);
    cvsource.PutFrame(flip);
  }
}
