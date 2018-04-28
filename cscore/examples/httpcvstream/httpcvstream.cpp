/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdio>
#include <iostream>

#include <opencv2/core/core.hpp>

#include "cscore.h"

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
      std::cout << "error: " << cvsink.GetError() << std::endl;
      continue;
    }
    std::cout << "got frame at time " << time << " size " << test.size()
              << std::endl;
    cv::flip(test, flip, 0);
    cvsource.PutFrame(flip);
  }
}
