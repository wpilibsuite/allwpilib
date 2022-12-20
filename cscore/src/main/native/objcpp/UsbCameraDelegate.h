#pragma once

#import <AVFoundation/AVFoundation.h>
#include <memory>

namespace cs {
class UsbCameraImpl;
}

@interface UsbCameraDelegate
    : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
// {
//     @public
//     UsbCameraImpl* m_source;
//     //IplImage* m_inputImageHeader;
//     //IplImage* m_outputImageHeader;
// }
@property (nonatomic) std::weak_ptr<cs::UsbCameraImpl> cppImpl;

- (void)captureOutput:(AVCaptureOutput*)captureOutput
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection*)connection;
@end
