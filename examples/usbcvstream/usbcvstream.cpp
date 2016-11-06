#include "cscore.h"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <stdio.h>

int main() {
  cs::USBCamera camera{"usbcam", 0};
  camera.SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
  cs::MJPEGServer mjpegServer{"httpserver", 8080};
  mjpegServer.SetSource(camera);
  cs::CvSink cvsink{"cvsink"};
  cvsink.SetSource(camera);
  cs::CvSource cvsource{"cvsource", cs::VideoMode::kMJPEG, 320, 240, 30};
  cs::MJPEGServer cvMjpegServer{"cvhttpserver", 8081};
  cvMjpegServer.SetSource(cvsource);

  cv::Mat test;
  cv::Mat flip;
  for (;;) {
    uint64_t time = cvsink.GrabFrame(test);
    std::cout << "got frame at time " << time << " size " << test.size() << std::endl;
    if (time == 0) continue;
    cv::flip(test, flip, 0);
    cvsource.PutFrame(flip);
  }
}
