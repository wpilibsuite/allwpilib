#pragma once

#import <AVFoundation/AVFoundation.h>

@interface UsbCameraDelegate
    : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
// {
//     @public
//     UsbCameraImpl* m_source;
//     //IplImage* m_inputImageHeader;
//     //IplImage* m_outputImageHeader;
// }
- (void)captureOutput:(AVCaptureOutput*)captureOutput
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection*)connection;
@end
