#import "UsbCameraDelegate.h"

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

  NSLog(@"Got Capture");
}

@end
