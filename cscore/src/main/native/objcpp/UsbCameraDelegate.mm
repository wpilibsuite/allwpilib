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

  auto currentTime = wpi::Now();

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

  std::unique_ptr<cs::Image> image = cs::CreateImageFromBGRA(
      sharedThis.get(), width, height, rowBytes, reinterpret_cast<uint8_t*>(baseaddress));

  CVPixelBufferUnlockBaseAddress(imageBuffer, 0);

  sharedThis->objcPutFrame(std::move(image), currentTime);
}

@end
