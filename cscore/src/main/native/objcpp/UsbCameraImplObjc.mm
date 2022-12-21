#import "UsbCameraImplObjc.h"
#include "UsbCameraImpl.h"
#include "Notifier.h"

using namespace cs;

@implementation UsbCameraImplObjc

- (void)start {
  switch ([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]) {
    case AVAuthorizationStatusAuthorized:
      NSLog(@"Already Authorized");
      break;
    default:
      NSLog(@"Authorization Failed");
      // TODO log
      break;
    case AVAuthorizationStatusNotDetermined:
      dispatch_suspend(self.sessionQueue);
      [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo
                               completionHandler:^(BOOL granted) {
                                 (void)granted;
                                 dispatch_resume(self.sessionQueue);
                               }];
      break;
  }
  dispatch_async(self.sessionQueue, ^{
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(cameraConnected:)
               name:AVCaptureDeviceWasConnectedNotification
             object:nil];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(cameraDisconnected:)
               name:AVCaptureDeviceWasDisconnectedNotification
             object:nil];
    [self deviceConnect];
  });
}

// Property functions
- (void)setProperty:(int)property
          withValue:(int)value
             status:(CS_Status*)status {
}
- (void)setStringProperty:(int)property
                withValue:(std::string_view*)value
                   status:(CS_Status*)status {
}

// Standard common camera properties
- (void)setBrightness:(int)brightness status:(CS_Status*)status {
}
- (int)getBrightness:(CS_Status*)status {
  return 0;
}
- (void)setWhiteBalanceAuto:(CS_Status*)status {
}
- (void)setWhiteBalanceHoldCurrent:(CS_Status*)status {
}
- (void)setWhiteBalanceManual:(int)value status:(CS_Status*)status {
}
- (void)setExposureAuto:(CS_Status*)status {
}
- (void)setExposureHoldCurrent:(CS_Status*)status {
}
- (void)setExposureManual:(int)value status:(CS_Status*)status {
}

- (bool)setVideoMode:(const cs::VideoMode&)mode status:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
      *status = CS_READ_FAILED;
      return;
    }

    [self internalSetMode:mode status:status];
  });
  return true;
}
- (bool)setPixelFormat:(cs::VideoMode::PixelFormat)pixelFormat
                status:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
      *status = CS_READ_FAILED;
      return;
    }
    VideoMode newMode;
    newMode = sharedThis->objcGetVideoMode();
    newMode.pixelFormat = pixelFormat;

    [self internalSetMode:newMode status:status];
  });
  return true;
}
- (bool)setResolutionWidth:(int)width
                withHeight:(int)height
                    status:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
      *status = CS_READ_FAILED;
      return;
    }
    VideoMode newMode;
    newMode = sharedThis->objcGetVideoMode();
    newMode.width = width;
    newMode.height = height;

    [self internalSetMode:newMode status:status];
  });
  return true;
}

- (void)internalSetMode:(const cs::VideoMode&)newMode
                 status:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_READ_FAILED;
    return;
  }
  // If device is not connected, just apply and leave.
  if (!self.propertiesCached) {
    sharedThis->objcSetVideoMode(newMode);
    *status = CS_OK;
    return;
  }

  if (newMode != sharedThis->objcGetVideoMode()) {
    NSLog(@"Trying Mode %d %d %d %d", newMode.pixelFormat, newMode.width,
          newMode.height, newMode.fps);
    int localFPS = 0;
    AVCaptureDeviceFormat* newModeType = [self deviceCheckModeValid:&newMode
                                                            withFps:&localFPS];
    if (newModeType == nil) {
      NSLog(@"Invalid Set Mode");
      *status = CS_UNSUPPORTED_MODE;
      return;
    }

    self.currentFormat = newModeType;
    self.currentFPS = localFPS;
    sharedThis->objcSetVideoMode(newMode);
    [self deviceDisconnect];
    [self deviceConnect];
    sharedThis->objcGetNotifier().NotifySourceVideoMode(*sharedThis, newMode);
  }
  *status = CS_OK;
}

- (bool)setFPS:(int)fps status:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
      *status = CS_READ_FAILED;
      return;
    }
    VideoMode newMode;
    newMode = sharedThis->objcGetVideoMode();
    newMode.fps = fps;

    [self internalSetMode:newMode status:status];
  });
  return true;
}

- (void)numSinksChanged {
  dispatch_async(self.sessionQueue, ^{
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
      return;
    }
    if (!sharedThis->IsEnabled()) {
      [self deviceStreamOff];
    } else if (!self.streaming && sharedThis->IsEnabled()) {
      [self deviceStreamOn];
    }
  });
}
- (void)numSinksEnabledChanged {
  [self numSinksChanged];
}

// All above is direct forwarders from C++, must always dispatch to loop

- (void)getCurrentCameraPath:(std::string*)path {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.videoDevice == nil) {
      return;
    }
    *path = [self.videoDevice.uniqueID UTF8String];
  });
}

- (void)getCameraName:(std::string*)name {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.videoDevice == nil) {
      return;
    }
    *name = [self.videoDevice.localizedName UTF8String];
  });
}

- (void)setNewCameraPath:(std::string_view*)path {
  dispatch_async_and_wait(self.sessionQueue, ^{
    NSString* nsPath = [[NSString alloc] initWithBytes:path->data()
                                                length:path->size()
                                              encoding:NSUTF8StringEncoding];
    if (self.path != nil && [self.path isEqualToString:nsPath]) {
      return;
    }
    self.path = nsPath;
    [self deviceDisconnect];
    [self deviceConnect];
  });
}

// All above are called from C++, must always dispatch to loop

- (void)deviceCacheProperties {
  if (self.session == nil) {
    return;
  }
}

static cs::VideoMode::PixelFormat FourCCToPixelFormat(FourCharCode fourcc) {
  switch (fourcc) {
    case kCVPixelFormatType_422YpCbCr8_yuvs:
    case kCVPixelFormatType_422YpCbCr8FullRange:
      return cs::VideoMode::PixelFormat::kYUYV;
    default:
      return cs::VideoMode::PixelFormat::kBGR;
  }
}

- (void)deviceCacheVideoModes {
  if (self.session == nil) {
    return;
  }

  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return;
  }
  std::vector<CameraModeStore>& platformModes =
      sharedThis->objcGetPlatformVideoModes();
  platformModes.clear();

  std::vector<VideoMode> modes;
  @autoreleasepool {
    NSArray<AVCaptureDeviceFormat*>* formats = self.videoDevice.formats;

    int count = 0;

    for (AVCaptureDeviceFormat* format in formats) {
      CMFormatDescriptionRef cmformat = format.formatDescription;
      CMVideoDimensions s1 = CMVideoFormatDescriptionGetDimensions(cmformat);

      FourCharCode fourcc = CMFormatDescriptionGetMediaSubType(cmformat);
      auto videoFormat = FourCCToPixelFormat(fourcc);

      NSArray<AVFrameRateRange*>* frameRates =
          format.videoSupportedFrameRateRanges;

      CameraModeStore store;
      store.mode.pixelFormat = videoFormat;
      store.mode.width = static_cast<int>(s1.width);
      store.mode.height = static_cast<int>(s1.height);
      store.format = format;
      int maxFps = 0;

      for (AVFrameRateRange* rate in frameRates) {
        CMTime highest = rate.minFrameDuration;
        CMTime lowest = rate.maxFrameDuration;

        int highestFps = highest.timescale / static_cast<double>(highest.value);
        int lowestFps = lowest.timescale / static_cast<double>(lowest.value);

        NSLog(@"%d %lld", lowest.timescale, lowest.value);

        store.fpsRanges.emplace_back(CameraFPSRange{lowestFps, highestFps});
        if (highestFps > maxFps) {
          maxFps = highestFps;
        }
      }
      store.mode.fps = maxFps;

      modes.emplace_back(store.mode);
      platformModes.emplace_back(store);
      count++;
    }
  }

  sharedThis->objcSwapVideoModes(modes);
  sharedThis->objcGetNotifier().NotifySource(*sharedThis,
                                             CS_SOURCE_VIDEOMODES_UPDATED);
}

- (AVCaptureDeviceFormat*)deviceCheckModeValid:(const cs::VideoMode*)toCheck
                                       withFps:(int*)fps {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return nil;
  }

  NSLog(@"Checking mode %d %d %d %d", toCheck->pixelFormat, toCheck->width,
        toCheck->height, toCheck->fps);
  std::vector<CameraModeStore>& platformModes =
      sharedThis->objcGetPlatformVideoModes();
  // Find the matching mode
  auto match = std::find_if(platformModes.begin(), platformModes.end(),
                            [&](CameraModeStore& input) {
                              return input.mode.CompareWithoutFps(*toCheck);
                            });

  if (match == platformModes.end()) {
    NSLog(@"No match even without fps");
    return nil;
  }

  // Check FPS
  for (CameraFPSRange& range : match->fpsRanges) {
    NSLog(@"Checking Range %d %d", range.min, range.max);
    if (range.IsWithinRange(toCheck->fps)) {
      *fps = toCheck->fps;
      return match->format;
    }
  }

  NSLog(@"No Matching FPS");
  return nil;
}

- (void)deviceCacheMode {
  if (!self.session) {
    return;
  }

  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return;
  }

  std::vector<CameraModeStore>& platformModes =
      sharedThis->objcGetPlatformVideoModes();

  if (platformModes.size() == 0) {
    return;
  }

  if (self.currentFormat == nil) {
    int localFps = 0;
    self.currentFormat =
        [self deviceCheckModeValid:&sharedThis->objcGetVideoMode()
                           withFps:&localFps];
    if (self.currentFormat == nil) {
      self.currentFormat = self.videoDevice.activeFormat;
      auto result = std::find_if(platformModes.begin(), platformModes.end(),
                                 [f = self.currentFormat](CameraModeStore& i) {
                                   return [f isEqual:i.format];
                                 });
      if (result == platformModes.end()) {
        auto& firstSupported = platformModes[0];
        self.currentFormat = firstSupported.format;
        self.currentFPS = firstSupported.mode.fps;
        sharedThis->objcSetVideoMode(firstSupported.mode);
      } else {
        self.currentFPS = result->mode.fps;
        sharedThis->objcSetVideoMode(result->mode);
      }
    } else {
      self.currentFPS = localFps;
    }
  }

  [self deviceSetMode];

  sharedThis->objcGetNotifier().NotifySourceVideoMode(
      *sharedThis, sharedThis->objcGetVideoMode());
}

- (void)deviceSetMode {
  self.deviceValid = true;
}

- (bool)deviceStreamOn {
  if (self.streaming) {
    return false;
  }
  if (!self.deviceValid) {
    return false;
  }
  self.streaming = true;

  [self.session startRunning];

  if ([self.videoDevice lockForConfiguration:nil]) {
    if (self.currentFormat != nil) {
      self.videoDevice.activeFormat = self.currentFormat;
    }
    if (self.currentFPS != 0) {
      self.videoDevice.activeVideoMinFrameDuration =
          CMTimeMake(1, self.currentFPS);
      self.videoDevice.activeVideoMaxFrameDuration =
          CMTimeMake(1, self.currentFPS);
    }
    [self.videoDevice unlockForConfiguration];
  } else {
    NSLog(@"Failed to lock for configuration");
  }

  return true;
}

- (bool)deviceStreamOff {
  if (self.streaming) {
    [self.session stopRunning];
  }
  self.streaming = false;
  return true;
}

- (id)init {
  self = [super init];
  // TODO pass in name, make this queue specific
  self.sessionQueue =
      dispatch_queue_create("session queue", DISPATCH_QUEUE_SERIAL);
  return self;
}

- (void)deviceDisconnect {
  [self deviceStreamOff];
  self.session = nil;
  self.videoOutput = nil;
  self.callback = nil;
  self.videoInput = nil;
  self.videoDevice = nil;
  self.streaming = false;
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return;
  }
  sharedThis->SetConnected(false);
}

- (bool)deviceConnect {
  OSType pixelFormat = kCVPixelFormatType_32BGRA;

  NSDictionary* pixelBufferOptions =
      @{(id)kCVPixelBufferPixelFormatTypeKey : @(pixelFormat)};

  if (self.session != nil) {
    return true;
  }

  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return false;
  }

  if (self.path == nil) {
    NSLog(@"Starting for device id %d", self.deviceId);
    // Enumerate Devices
    CS_Status status = 0;
    auto cameras = cs::EnumerateUsbCameras(&status);
    if (static_cast<int>(cameras.size()) <= self.deviceId) {
      NSLog(@"Path for index %d not found. Only %d exist", self.deviceId,
            static_cast<int>(cameras.size()));
      return false;
    }
    std::string& path = cameras[self.deviceId].path;
    self.path = [[NSString alloc] initWithBytes:path.data()
                                         length:path.size()
                                       encoding:NSUTF8StringEncoding];
  }

  NSLog(@"Starting for path %@", self.path);

  self.videoDevice = [AVCaptureDevice deviceWithUniqueID:self.path];
  if (self.videoDevice == nil) {
    NSLog(@"Device Not found");
    goto err;
  }

  self.videoInput = [AVCaptureDeviceInput deviceInputWithDevice:self.videoDevice
                                                          error:nil];
  if (self.videoInput == nil) {
    NSLog(@"Video Input Failed");
    goto err;
  }

  self.callback = [[UsbCameraDelegate alloc] init];
  if (self.callback == nil) {
    NSLog(@"Callback failed");
    goto err;
  }
  self.callback.cppImpl = self.cppImpl;

  self.videoOutput = [[AVCaptureVideoDataOutput alloc] init];
  if (self.videoOutput == nil) {
    NSLog(@"Video Output Failed");
    goto err;
  }

  [self.videoOutput setSampleBufferDelegate:self.callback
                                      queue:self.sessionQueue];

  self.videoOutput.videoSettings = pixelBufferOptions;
  self.videoOutput.alwaysDiscardsLateVideoFrames = YES;

  self.session = [[AVCaptureSession alloc] init];
  if (self.session == nil) {
    NSLog(@"Session failed");
    goto err;
  }

  [[NSNotificationCenter defaultCenter]
      addObserver:self
         selector:@selector(sessionRuntimeError:)
             name:AVCaptureSessionRuntimeErrorNotification
           object:self.session];

  [self.session addInput:self.videoInput];
  [self.session addOutput:self.videoOutput];
  //

  sharedThis->SetDescription([self.videoDevice.localizedName UTF8String]);

  if (!self.propertiesCached) {
    NSLog(@"Caching properties");
    [self deviceCacheProperties];
    [self deviceCacheVideoModes];
    [self deviceCacheMode];
    self.propertiesCached = true;
  } else {
    [self deviceSetMode];
  }

  sharedThis->SetConnected(true);

  if (sharedThis->IsEnabled()) {
    [self deviceStreamOn];
  }

  return true;

err:
  self.session = nil;
  self.videoOutput = nil;
  self.callback = nil;
  self.videoInput = nil;
  self.videoDevice = nil;
  return false;
}

// Helpers
- (void)sessionRuntimeError:(NSNotification*)notification {
  NSError* error = notification.userInfo[AVCaptureSessionErrorKey];
  NSLog(@"Capture session runtime error: %@", error);
}

- (void)cameraDisconnected:(NSNotification*)notification {
  AVCaptureDevice* device = notification.object;
  NSLog(@"Device Disconnected: %@", device.uniqueID);
  dispatch_async(self.sessionQueue, ^{
    if (self.path != nil && [device.uniqueID isEqualToString:self.path]) {
      [self deviceDisconnect];
    }
  });
}

- (void)cameraConnected:(NSNotification*)notification {
  AVCaptureDevice* device = notification.object;
  NSLog(@"Device Connected: %@", device.uniqueID);
  dispatch_async(self.sessionQueue, ^{
    if (self.path == nil || [device.uniqueID isEqualToString:self.path]) {
      [self deviceConnect];
    }
  });
}

@end
