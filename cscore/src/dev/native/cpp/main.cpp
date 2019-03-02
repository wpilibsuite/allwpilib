/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <opencv2/core.hpp>

#include "cscore.h"

int main() {
  std::cout << cs::GetHostname() << std::endl;
  cs::UsbCamera cam{"Usb", 0};
  cs::MjpegServer serv{"UsbS", 1181};
  serv.SetSource(cam);
  // cs::HttpCamera httpCamera{"Camera",
  // "http://172.22.11.2:1181/IMAQdxStream.mjpg?name=IMAQdx%3AMicrosoft%20LifeCam%20HD-3000&fps=15&compression=30&resolution=320x240"};
  cs::HttpCamera httpCamera{"Camera",
                            "http://172.22.11.2:1181/"
                            "IMAQdxStream.mjpg?name=IMAQdx%3AMicrosoft%"
                            "20LifeCam%20HD-3000"};
  // cs::HttpCamera httpCamera{"Camera", "http://127.0.0.1:1181/stream.mjpg"};
  cs::CvSink sink{"Sink"};
  sink.SetSource(httpCamera);

  cv::Mat mat;

  while (true) {
    if (sink.GrabFrame(mat) != 0) {
      std::cout << "Grabbed" << std::endl;
    } else {
      std::cout << "Timeout" << std::endl;
    }
  }
}
