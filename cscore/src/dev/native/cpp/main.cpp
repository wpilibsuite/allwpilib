/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <opencv/cv.hpp>

#include "cscore.h"

using namespace cv;

int main() {
  CS_Status status = 0;

  auto usbSource = CS_CreateUsbCameraDev("Camera", 0, &status);

  auto sinkHandle = CS_CreateRawSink("RawSink", &status);

  CS_VideoMode videoMode;
  videoMode.fps = 30;
  videoMode.width = 640;
  videoMode.height = 360;
  videoMode.pixelFormat = CS_PixelFormat::CS_PIXFMT_GRAY;

  auto sourceHandle = CS_CreateRawSource("RawSource", &videoMode, &status);

  auto mjpegHandle = CS_CreateMjpegServer("Server", "", 1181, &status);

  CS_SetSinkSource(mjpegHandle, sourceHandle, &status);

  CS_SetSinkSource(sinkHandle, usbSource, &status);

  namedWindow("window", WINDOW_AUTOSIZE);

  cs::RawFrame frame;
  frame.pixelFormat = CS_PIXFMT_UNKNOWN;
  // frame->pixelFormat = CS_PixelFormat::CS_PIXFMT_BGR;

  while (true) {
    auto timeout = cs::GrabSinkFrameTimeout(sinkHandle, frame, 0.225, &status);

    if (timeout > 0) {
      cs::PutSourceFrame(sourceHandle, frame, &status);

      cv::Mat mat{frame.height, frame.width, CV_8UC1, frame.data};
      imshow("window", mat);
      waitKey(1);
    }
    std::cout << timeout << std::endl;
  }

  std::cout << cs::GetHostname() << std::endl;
}
