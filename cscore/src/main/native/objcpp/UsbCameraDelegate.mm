// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import "UsbCameraDelegate.h"
#include "UsbCameraImpl.h"

#include <wpi/timestamp.h>

@implementation UsbCameraDelegate

- (id)init {
  self = [super init];
  return self;
}

- (void)captureOutput:(AVCaptureOutput*)captureOutput
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection*)connection {
  (void)captureOutput;
  (void)sampleBuffer;
  (void)connection;

  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return;
  }

  // Buffer always comes in a 32BGRA
  auto imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

  CVPixelBufferLockBaseAddress(imageBuffer, 0);

  void* baseaddress = CVPixelBufferGetBaseAddress(imageBuffer);

  size_t width = CVPixelBufferGetWidth(imageBuffer);
  size_t height = CVPixelBufferGetHeight(imageBuffer);
  size_t rowBytes = CVPixelBufferGetBytesPerRow(imageBuffer);
  OSType pixelFormat = CVPixelBufferGetPixelFormatType(imageBuffer);

  if (rowBytes == 0) {
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    return;
  }

  if (pixelFormat != kCVPixelFormatType_32BGRA) {
    NSLog(@"Unknown Pixel Format %u", pixelFormat);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    return;
  }

  size_t currSize = width * 3 * height;

  std::unique_ptr<Image> dest = cv::CreateImageFromBGRA(
      this, width, height, rowBytes, baseaddress);

  CVPixelBufferUnlockBaseAddress(imageBuffer, 0);

  sharedThis->objcPutFrame(std::move(image), wpi::Now());
}

@end
