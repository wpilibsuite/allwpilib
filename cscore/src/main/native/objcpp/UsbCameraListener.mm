// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import <AVFoundation/AVFoundation.h>
#include "UsbCameraListener.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "Notifier.h"

using namespace cs;

@interface UsbCameraListenerImpl : NSObject
@property(nonatomic) Notifier* notifier;
@property BOOL started;
@property(nonatomic) dispatch_queue_t sessionQueue;

- (void)start;
- (void)stop;

@end

@implementation UsbCameraListenerImpl

- (id)init {
  self = [super init];
  self.sessionQueue =
      dispatch_queue_create("Camera Listener", DISPATCH_QUEUE_SERIAL);
  return self;
}

- (void)start {
  dispatch_async(self.sessionQueue, ^{
    if (self.started) {
      return;
    }
    self.started = YES;
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(camerasChanged:)
               name:AVCaptureDeviceWasConnectedNotification
             object:nil];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(camerasChanged:)
               name:AVCaptureDeviceWasDisconnectedNotification
             object:nil];
  });
}

- (void)stop {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (!self.started) {
      return;
    }
    self.started = NO;
    [[NSNotificationCenter defaultCenter]
        removeObserver:self
                  name:AVCaptureDeviceWasConnectedNotification
                object:nil];
    [[NSNotificationCenter defaultCenter]
        removeObserver:self
                  name:AVCaptureDeviceWasDisconnectedNotification
                object:nil];
  });
}

- (void)camerasChanged:(NSNotification*)notification {
  @autoreleasepool {
    dispatch_async(self.sessionQueue, ^{
      if (!self.started) {
        return;
      }

      AVCaptureDevice* device = notification.object;
      if ([device.deviceType
              isEqualToString:AVCaptureDeviceTypeBuiltInWideAngleCamera] ||
          [device.deviceType
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 140000
              isEqualToString:AVCaptureDeviceTypeExternal]) {
#else
              isEqualToString:AVCaptureDeviceTypeExternalUnknown]) {
#endif
        self.notifier->NotifyUsbCamerasChanged();
      }
    });
  }
}

@end

class UsbCameraListener::Impl {
 public:
  UsbCameraListenerImpl* listener;

  explicit Impl(Notifier& notifier) {
    listener = [[UsbCameraListenerImpl alloc] init];
    listener.notifier = &notifier;
  }
};

UsbCameraListener::UsbCameraListener(wpi::Logger&, Notifier& notifier)
    : m_impl{std::make_unique<Impl>(notifier)} {}

UsbCameraListener::~UsbCameraListener() {
  Stop();
}

void UsbCameraListener::Start() {
  [m_impl->listener start];
}

void UsbCameraListener::Stop() {
  [m_impl->listener stop];
}
