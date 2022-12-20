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
  return false;
}
- (bool)setPixelFormat:(cs::VideoMode::PixelFormat)pixelFormat
                status:(CS_Status*)status {
  return false;
}
- (bool)setResolutionWidth:(int)width
                withHeight:(int)height
                    status:(CS_Status*)status {
  return false;
}
- (bool)setFPS:(int)fps status:(CS_Status*)status {
  return false;
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
  // TODO Make this a lot better
  (void)fourcc;
  return cs::VideoMode::PixelFormat::kBGR;
}

- (void)deviceCacheVideoModes {
  if (self.session == nil) {
    return;
  }

  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return;
  }
  std::vector<std::pair<VideoMode, AVCaptureDeviceFormat*>>& platformModes =
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

      for (AVFrameRateRange* rate in frameRates) {
        NSLog(@"%@ %@", format, rate);
        CMTime frameDuration = rate.minFrameDuration;



        NSLog(@"%d %lld", frameDuration.timescale, frameDuration.value);

        int fps = 30;
        if (frameDuration.value != 0) {
          fps = frameDuration.timescale /
                          static_cast<double>(frameDuration.value);
        }

        VideoMode newMode = {videoFormat, static_cast<int>(s1.width),
                             static_cast<int>(s1.height), fps};

        modes.emplace_back(newMode);
        platformModes.emplace_back(newMode, format);
        count++;
      }
    }
  }

  sharedThis->objcSwapVideoModes(modes);
  sharedThis->objcGetNotifier().NotifySource(*sharedThis,
                                             CS_SOURCE_VIDEOMODES_UPDATED);
}

- (AVCaptureDeviceFormat*)deviceCheckModeValid:(const cs::VideoMode*)toCheck {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return nil;
  }
  std::vector<std::pair<VideoMode, AVCaptureDeviceFormat*>>& platformModes =
      sharedThis->objcGetPlatformVideoModes();
  // Find the matching mode
  auto match =
      std::find_if(platformModes.begin(), platformModes.end(),
                   [&](std::pair<VideoMode, AVCaptureDeviceFormat*>& input) {
                     return input.first == *toCheck;
                   });

  if (match == platformModes.end()) {
    return nil;
  }
  return match->second;
}

- (void)deviceCacheMode {
  if (!self.session) {
    return;
  }

  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return;
  }

  std::vector<std::pair<VideoMode, AVCaptureDeviceFormat*>>& platformModes =
      sharedThis->objcGetPlatformVideoModes();

  if (platformModes.size() == 0) {
    return;
  }

  if (self.currentFormat == nil) {
    self.currentFormat =
        [self deviceCheckModeValid:&sharedThis->objcGetVideoMode()];
    if (self.currentFormat == nil) {
      self.currentFormat = self.videoDevice.activeFormat;
      auto result =
          std::find_if(platformModes.begin(), platformModes.end(),
                       [f = self.currentFormat](
                           std::pair<VideoMode, AVCaptureDeviceFormat*>& i) {
                         return [f isEqual:i.second];
                       });
      if (result == platformModes.end()) {
        auto&& firstSupported = platformModes[0];
        self.currentFormat = firstSupported.second;
        sharedThis->objcSetVideoMode(firstSupported.first);
      } else {
        sharedThis->objcSetVideoMode(result->first);
      }
    }
  }

  [self deviceSetMode];

  sharedThis->objcGetNotifier().NotifySourceVideoMode(
      *sharedThis, sharedThis->objcGetVideoMode());
}

- (void)deviceSetMode {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return;
  }

  // TODO Set mode on callback

  self.deviceValid = true;

  NSLog(@"Setting mode to %@", self.currentFormat);
  [self.videoDevice lockForConfiguration:nil];
  self.videoDevice.activeFormat = self.currentFormat;
  // TODO FPS
  // self.videoDevice.activeVideoMaxFrameDuration = self.currentFormat.;
  [self.videoDevice unlockForConfiguration];
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

  NSDictionary* pixelBufferOptions = @{
    (id)kCVPixelBufferPixelFormatTypeKey: @(pixelFormat)
  };

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
