#import "UsbCameraDelegate.h"
#include "UsbCameraImpl.h"

#include <wpi/timestamp.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

  NSLog(@"Got Capture");

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

  auto tmpMat = cv::Mat(height, width, CV_8UC4, baseaddress, rowBytes);
  auto image = sharedThis->AllocImage(cs::VideoMode::PixelFormat::kBGR, width,
                                      height, currSize);
  cv::cvtColor(tmpMat, image->AsMat(), cv::COLOR_BGRA2BGR);

  // std::cout << "Row bytes " <<rowBytes << std::endl;
  CVPixelBufferUnlockBaseAddress(imageBuffer, 0);

  sharedThis->objcPutFrame(std::move(image), wpi::Now());
}

@end
