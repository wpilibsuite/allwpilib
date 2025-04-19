// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import "UsbCameraImplObjc.h"
#include "UsbCameraImpl.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "Notifier.h"
#include "Log.h"

template <typename S, typename... Args>
inline void NamedLog(UsbCameraImplObjc* objc, unsigned int level,
                     const char* file, unsigned int line, const S& format,
                     Args&&... args) {
  auto sharedThis = objc.cppImpl.lock();
  if (!sharedThis) {
    return;
  }

  wpi::Logger& logger = sharedThis->objcGetLogger();
  std::string_view name = sharedThis->GetName();

  if (logger.HasLogger() && level >= logger.min_level()) {
    cs::NamedLogV(logger, level, file, line, name, format,
                  fmt::make_format_args(args...));
  }
}

#define OBJCLOG(level, format, ...)         \
  NamedLog(self, level, __FILE__, __LINE__, \
           format __VA_OPT__(, ) __VA_ARGS__)

#define OBJCERROR(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_ERROR, format __VA_OPT__(, ) __VA_ARGS__)
#define OBJCWARNING(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_WARNING, format __VA_OPT__(, ) __VA_ARGS__)
#define OBJCINFO(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_INFO, format __VA_OPT__(, ) __VA_ARGS__)

#ifdef NDEBUG
#define OBJCDEBUG(format, ...) \
  do {                         \
  } while (0)
#define OBJCDEBUG1(format, ...) \
  do {                          \
  } while (0)
#define OBJCDEBUG2(format, ...) \
  do {                          \
  } while (0)
#define OBJCDEBUG3(format, ...) \
  do {                          \
  } while (0)
#define OBJCDEBUG4(format, ...) \
  do {                          \
  } while (0)
#else
#define OBJCDEBUG(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_DEBUG, format __VA_OPT__(, ) __VA_ARGS__)
#define OBJCDEBUG1(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_DEBUG1, format __VA_OPT__(, ) __VA_ARGS__)
#define OBJCDEBUG2(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_DEBUG2, format __VA_OPT__(, ) __VA_ARGS__)
#define OBJCDEBUG3(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_DEBUG3, format __VA_OPT__(, ) __VA_ARGS__)
#define OBJCDEBUG4(format, ...) \
  OBJCLOG(::wpi::WPI_LOG_DEBUG4, format __VA_OPT__(, ) __VA_ARGS__)
#endif

using namespace cs;

@implementation UsbCameraImplObjc

+ (int)clampToPercent:(int)value {
  if (value < 0) return 0;
  if (value > 100) return 100;
  return value;
}

- (void)start {
  switch ([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]) {
    case AVAuthorizationStatusAuthorized:
      self.isAuthorized = true;
      break;
    default:
      OBJCERROR(
          "Camera access explicitly blocked for application. No cameras are "
          "accessible");
      self.isAuthorized = false;
      // TODO log
      break;
    case AVAuthorizationStatusNotDetermined:
      dispatch_suspend(self.sessionQueue);
      [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo
                               completionHandler:^(BOOL granted) {
                                 self.isAuthorized = granted;
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
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.uvcControl == nil) {
      *status = CS_INVALID_PROPERTY;
      return;
    }

    BOOL isPercent = (property == CAPPROPID_BRIGHTNESS ||
                      property == CAPPROPID_EXPOSURE ||
                      property == CAPPROPID_WHITEBALANCE ||
                      property == CAPPROPID_CONTRAST ||
                      property == CAPPROPID_SATURATION ||
                      property == CAPPROPID_HUE ||
                      property == CAPPROPID_SHARPNESS ||
                      property == CAPPROPID_GAIN);

    int32_t min = 0, max = 0, defValue = 0;
    if (![self.uvcControl getPropertyLimits:property min:&min max:&max defValue:&defValue status:status]) {
      return;
    }

    int32_t realValue = value;
    if (isPercent) {
      int localValue = [UsbCameraImplObjc clampToPercent:value];
      realValue = min + (max - min) * localValue / 100;
    }

    if (![self.uvcControl setProperty:property withValue:realValue status:status]) {
      return;
    }
  });
}

- (void)setStringProperty:(int)property
                withValue:(std::string_view*)value
                   status:(CS_Status*)status {
  *status = CS_INVALID_PROPERTY;
  return;
}

// Standard common camera properties
- (void)setBrightness:(int)brightness status:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.uvcControl == nil) {
      *status = CS_INVALID_PROPERTY;
      return;
    }
    
    int localBrightness = [UsbCameraImplObjc clampToPercent:brightness];

    int32_t min = 0, max = 0, defValue = 0;
    if (![self.uvcControl getPropertyLimits:CAPPROPID_BRIGHTNESS min:&min max:&max defValue:&defValue status:status]) {
      return;
    }

    int32_t realValue = min + (max - min) * localBrightness / 100;
    if (![self.uvcControl setProperty:CAPPROPID_BRIGHTNESS withValue:realValue status:status]) {
      return;
    }
  });
}

- (int)getBrightness:(CS_Status*)status {
  __block int percent = 0;
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.uvcControl == nil) {
      *status = CS_INVALID_PROPERTY;
      percent = 0;
      return;
    }
    
    int32_t value = 0;
    if (![self.uvcControl getProperty:CAPPROPID_BRIGHTNESS withValue:&value status:status]) {
      *status = CS_INVALID_PROPERTY;
      percent = 0;
      return;
    }
    
    int32_t min = 0, max = 0, defValue = 0;
    if (![self.uvcControl getPropertyLimits:CAPPROPID_BRIGHTNESS min:&min max:&max defValue:&defValue status:status]) {
      *status = CS_INVALID_PROPERTY;
      percent = 0;
      return;
    }

    if (max == min) {
      percent = 0;
      return;
    }
    
    percent = (value - min) * 100 / (max - min);
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
  });
  return percent;
}

- (void)setWhiteBalanceAuto:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.uvcControl == nil) {
      // try AVCaptureDevice when uvc is not available
      if ([self.videoDevice lockForConfiguration:nil]) {
        if ([self.videoDevice isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeAutoWhiteBalance]) {
          self.videoDevice.whiteBalanceMode = AVCaptureWhiteBalanceModeAutoWhiteBalance;
          *status = CS_OK;
        } else {
        *status = CS_INVALID_PROPERTY;
      }
      [self.videoDevice unlockForConfiguration];
    } else {
      *status = CS_READ_FAILED;
    }
      return;
    }
    
    if (![self.uvcControl setAutoProperty:CAPPROPID_WHITEBALANCE enabled:YES status:status]) {
      return;
    }
  });
}

- (void)setWhiteBalanceHoldCurrent:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.videoDevice == nil) {
      *status = CS_READ_FAILED;
      return;
    }
    
    if ([self.videoDevice lockForConfiguration:nil]) {
      if ([self.videoDevice isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeLocked]) {
        self.videoDevice.whiteBalanceMode = AVCaptureWhiteBalanceModeLocked;
        *status = CS_OK;
      } else {
        *status = CS_INVALID_PROPERTY;
      }
      [self.videoDevice unlockForConfiguration];
    } else {
      *status = CS_READ_FAILED;
    }
  });
}

- (void)setWhiteBalanceManual:(int)value status:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.uvcControl == nil) {
      *status = CS_INVALID_PROPERTY;
      return;
    }
    
    if (![self.uvcControl setAutoProperty:CAPPROPID_WHITEBALANCE enabled:NO status:status]) {
      return;
    }

    int localValue = [UsbCameraImplObjc clampToPercent:value];

    int32_t min = 0, max = 0, defValue = 0;
    if (![self.uvcControl getPropertyLimits:CAPPROPID_WHITEBALANCE min:&min max:&max defValue:&defValue status:status]) {
      return;
    }

    int32_t realValue = min + (max - min) * localValue / 100;
    if (![self.uvcControl setProperty:CAPPROPID_WHITEBALANCE withValue:realValue status:status]) {
      return;
    }
  });
}

- (void)setExposureAuto:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.uvcControl == nil) {
      // try AVCaptureDevice when uvc is not available
      if ([self.videoDevice lockForConfiguration:nil]) {
        if ([self.videoDevice isExposureModeSupported:AVCaptureExposureModeAutoExpose]) {
          self.videoDevice.exposureMode = AVCaptureExposureModeAutoExpose;
          *status = CS_OK;
        } else {
          *status = CS_INVALID_PROPERTY;
        }
        [self.videoDevice unlockForConfiguration];
      } else {
        *status = CS_READ_FAILED;
      }
      return;
    }
    
    if (![self.uvcControl setAutoProperty:CAPPROPID_EXPOSURE enabled:YES status:status]) {
      return;
    }
  });
}

- (void)setExposureHoldCurrent:(CS_Status*)status {
  dispatch_async_and_wait(self.sessionQueue, ^{
    if (self.videoDevice == nil) {
      *status = CS_READ_FAILED;
      return;
    }
    
    if ([self.videoDevice lockForConfiguration:nil]) {
      if ([self.videoDevice isExposureModeSupported:AVCaptureExposureModeLocked]) {
        self.videoDevice.exposureMode = AVCaptureExposureModeLocked;
        *status = CS_OK;
      } else {
        *status = CS_INVALID_PROPERTY;
      }
      [self.videoDevice unlockForConfiguration];
    } else {
      *status = CS_READ_FAILED;
    }
  });
}

- (void)setExposureManual:(int)value status:(CS_Status*)status {
  if (self.uvcControl == nil) {
    *status = CS_UNSUPPORTED_MODE;
    return;
  }
  
  if (![self.uvcControl setAutoProperty:CAPPROPID_EXPOSURE enabled:NO status:status]) {
    return;
  }

  int localValue = [UsbCameraImplObjc clampToPercent:value];

  int32_t min = 0, max = 0, defValue = 0;
  if (![self.uvcControl getPropertyLimits:CAPPROPID_EXPOSURE min:&min max:&max defValue:&defValue status:status]) {
    return;
  }

  int32_t realValue = min + (max - min) * localValue / 100;
  if (![self.uvcControl setProperty:CAPPROPID_EXPOSURE withValue:realValue status:status]) {
    return;
  }
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
    OBJCDEBUG3("Trying Mode {} {} {} {}", newMode.pixelFormat, newMode.width,
               newMode.height, newMode.fps);
    int localFPS = 0;
    AVCaptureDeviceFormat* newModeType = [self deviceCheckModeValid:&newMode
                                                            withFps:&localFPS];
    if (newModeType == nil) {
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

        store.fpsRanges.emplace_back(CameraFPSRange{lowestFps, highestFps});
        if (highestFps > maxFps) {
          maxFps = highestFps;
        }
      }
      store.mode.fps = maxFps;

      modes.emplace_back(store.mode);
      platformModes.emplace_back(store);
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

  OBJCDEBUG3("Checking mode {} {} {} {}", toCheck->pixelFormat, toCheck->width,
             toCheck->height, toCheck->fps);
  std::vector<CameraModeStore>& platformModes =
      sharedThis->objcGetPlatformVideoModes();
  
  // Find all matching modes
  std::vector<CameraModeStore*> matchingModes;
  for (auto& mode : platformModes) {
    if (mode.mode.CompareWithoutFps(*toCheck)) {
      matchingModes.push_back(&mode);
    }
  }

  if (matchingModes.empty()) {
    return nil;
  }

  // Check FPS
  for (auto mode : matchingModes) {
    for (CameraFPSRange& range : mode->fpsRanges) {
      OBJCDEBUG3("Checking Range {} {}", range.min, range.max);
      if (range.IsWithinRange(toCheck->fps)) {
        *fps = toCheck->fps;
        return mode->format;
      }
    }
  }

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
      // self.videoDevice.activeVideoMinFrameDuration =
      //     CMTimeMake(1, self.currentFPS);
      // self.videoDevice.activeVideoMaxFrameDuration =
      //     CMTimeMake(1, self.currentFPS);
    }
    [self.videoDevice unlockForConfiguration];
  } else {
    OBJCERROR("Failed to lock for configuration");
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
  std::string pathStr = [self.path UTF8String];
  OBJCINFO("Disconnected from {}", pathStr);

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
  if (!self.isAuthorized) {
    OBJCERROR(
        "Camera access not authorized for application. No cameras are "
        "accessible");
    return false;
  }

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
    OBJCINFO("Starting for device id {}", self.deviceId);
    // Enumerate Devices
    CS_Status status = 0;
    auto cameras = cs::EnumerateUsbCameras(&status);
    if (static_cast<int>(cameras.size()) <= self.deviceId) {
      return false;
    }
    std::string& path = cameras[self.deviceId].path;
    self.path = [[NSString alloc] initWithBytes:path.data()
                                         length:path.size()
                                       encoding:NSUTF8StringEncoding];
  }

  std::string pathStr = [self.path UTF8String];
  OBJCINFO("Attempting to connect to USB camera on {}", pathStr);

  self.videoDevice = [AVCaptureDevice deviceWithUniqueID:self.path];
  if (self.videoDevice == nil) {
    OBJCWARNING("Device Not found");
    goto err;
  }

  self.videoInput = [AVCaptureDeviceInput deviceInputWithDevice:self.videoDevice
                                                          error:nil];
  if (self.videoInput == nil) {
    OBJCWARNING("Creating AVCaptureDeviceInput failed");
    goto err;
  }

  CS_Status status;
  self.uvcControl = [UvcControlImpl createFromAVCaptureDevice:self.videoDevice status:&status];
  if (self.uvcControl == nil) {
    OBJCWARNING("Failed to initialize UVC control for camera: {}", status);
  } else {
    OBJCINFO("UVC control initialized successfully");
  }

  self.callback = [[UsbCameraDelegate alloc] init];
  if (self.callback == nil) {
    OBJCWARNING("Creating Camera Callback failed");
    goto err;
  }
  self.callback.cppImpl = self.cppImpl;

  self.videoOutput = [[AVCaptureVideoDataOutput alloc] init];
  if (self.videoOutput == nil) {
    OBJCWARNING("Creating AVCaptureVideoDataOutput failed");
    goto err;
  }

  [self.videoOutput setSampleBufferDelegate:self.callback
                                      queue:self.sessionQueue];

  self.videoOutput.videoSettings = pixelBufferOptions;
  self.videoOutput.alwaysDiscardsLateVideoFrames = YES;

  self.session = [[AVCaptureSession alloc] init];
  if (self.session == nil) {
    OBJCWARNING("Creating AVCaptureSession failed");
    goto err;
  }

  OBJCINFO("Connected to USB camera on {}", pathStr);

  [[NSNotificationCenter defaultCenter]
      addObserver:self
         selector:@selector(sessionRuntimeError:)
             name:AVCaptureSessionRuntimeErrorNotification
           object:self.session];

  [self.session addInput:self.videoInput];
  [self.session addOutput:self.videoOutput];

  sharedThis->SetDescription([self.videoDevice.localizedName UTF8String]);

  if (!self.propertiesCached) {
    OBJCDEBUG3("Caching properties");
    [self deviceCacheProperties];
    [self deviceCacheVideoModes];
    [self deviceCacheMode];
    self.propertiesCached = true;
  } else {
    OBJCDEBUG3("Restoring Video Mode");
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
  @autoreleasepool {
    NSError* error = notification.userInfo[AVCaptureSessionErrorKey];
    const char* str = [error.description UTF8String];
    if (str) {
      std::string errorStr = str;
      OBJCERROR("Capture session runtime error: {}", errorStr);
    }
  }
}

- (void)cameraDisconnected:(NSNotification*)notification {
  AVCaptureDevice* device = notification.object;
  dispatch_async(self.sessionQueue, ^{
    if (self.path != nil && [device.uniqueID isEqualToString:self.path]) {
      [self deviceDisconnect];
    }
  });
}

- (void)cameraConnected:(NSNotification*)notification {
  AVCaptureDevice* device = notification.object;
  dispatch_async(self.sessionQueue, ^{
    if (self.path == nil || [device.uniqueID isEqualToString:self.path]) {
      [self deviceConnect];
    }
  });
}

@end
