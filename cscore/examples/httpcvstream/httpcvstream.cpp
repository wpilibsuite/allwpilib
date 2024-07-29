// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>

#include <opencv2/core/core.hpp>
#include <wpi/print.h>

#include "cscore.h"
#include "cscore_cv.h"

int main() {
  cs::HttpCamera camera{"httpcam", "http://localhost:8081/?action=stream"};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
  cs::CvSink cvsink{"cvsink"};
  cvsink.SetSource(camera);
  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 240, 30};
  cs::MjpegServer cvMjpegServer{"cvhttpserver", 8083};
  cvMjpegServer.SetSource(cvsource);

  cv::Mat test;
  cv::Mat flip;
  for (;;) {
    uint64_t time = cvsink.GrabFrame(test);
    if (time == 0) {
      wpi::print("error: {}\n", cvsink.GetError());
      continue;
    }
    wpi::print("got frame at time {} size ({}, {})\n", time, test.size().width,
               test.size().height);
    cv::flip(test, flip, 0);
    cvsource.PutFrame(flip);
  }
}
