// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/SmallString.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#import "UsbCameraImplObjc.h"

#include "Notifier.h"
#include "Log.h"
#include "UsbCameraImpl.h"

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
    [self deviceCacheProperties];
  });
}

- (BOOL)getEnabledWithProperty:(int)property withValue:(int)value {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    return false;
  }

  // There is room for quirk handling improvement here, but I will leave it
  // for now.
  if (property == sharedThis->GetPropertyIndex(kPropertyAutoExposure)) {
    return value == kPropertyAutoExposureOn;
  }

  return value != 0;
}


- (int)clampToPercent:(int)value {
  if (value < 0) {
    return 0;
  }
  if (value > 100) {
    return 100;
  }
  return value;
}

- (int)percentageToRaw:(int)propID percentage:(int)percentage min:(int)min max:(int)max {
  if (min == max) {
    return min;
  }

  return min + (max - min) * percentage / 100;
}

- (BOOL)isPercentageProperty:(int)propID {
    return propID == CAPPROPID_BRIGHTNESS ||
           propID == CAPPROPID_CONTRAST ||
           propID == CAPPROPID_SATURATION ||
           propID == CAPPROPID_HUE ||
           propID == CAPPROPID_SHARPNESS ||
           propID == CAPPROPID_GAIN;
}

// Property functions
- (void)setProperty:(int)property
          withValue:(int)value
             status:(CS_Status*)status {
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
        *status = CS_INVALID_HANDLE;
        return;
    }
    
    // Make sure properties are cached
    if (!self.propertiesCached) {
        [self deviceCacheProperties];
    }
    
    // Get the property name from the property index
    wpi::SmallString<128> nameBuf;
    std::string_view propName = sharedThis->GetPropertyName(property, nameBuf, status);
    if (*status != 0) {
        OBJCERROR("Failed to get property name for index {}", property);
        return;
    }
    
    std::string nameStr(propName);
    
    // Check if it's an auto property
    auto& propertyAutoCache = sharedThis->GetPropertyAutoCache();
    auto autoIt = propertyAutoCache.find(nameStr);
    if (autoIt != propertyAutoCache.end()) {
        uint32_t propID = autoIt->second;
        bool enabled = [self getEnabledWithProperty:property withValue:value];
        dispatch_async_and_wait(self.sessionQueue, ^{
            if (self.uvcControl == nil) {
                *status = CS_INVALID_PROPERTY;
                return;
            }
            
            if (![self.uvcControl setAutoProperty:propID enabled:enabled status:status]) {
                OBJCERROR("Failed to set auto property {} to {}",
                        nameStr, enabled);
                return;
            }
            
            // Update property value
            sharedThis->UpdatePropertyValuePublic(property, false, value, {});
        });
        return;
    }
    
    // Handle regular property
    auto& propertyCache = sharedThis->GetPropertyCache();
    auto it = propertyCache.find(nameStr);
    if (it == propertyCache.end()) {
        OBJCERROR("Property not found in cache: {}", nameStr);
        *status = CS_INVALID_PROPERTY;
        return;
    }
    
    uint32_t propID = it->second;
    
    dispatch_async_and_wait(self.sessionQueue, ^{
        if (self.uvcControl == nil) {
            *status = CS_INVALID_PROPERTY;
            return;
        }
        
        // Get the property implementation to access its limits
        const PropertyImpl* prop = sharedThis->GetPropertyPublic(property);
        if (!prop) {
            *status = CS_INVALID_PROPERTY;
            return;
        }
        

        int32_t realValue = value;
        if ([self isPercentageProperty:propID]) {
            // Clamp to 0-100
            realValue = [self clampToPercent:realValue];

            // Scale to min/max
            realValue = [self percentageToRaw:propID percentage:realValue min:prop->minimum max:prop->maximum];
        }

        if (![self.uvcControl setProperty:propID withValue:realValue status:status]) {
            OBJCERROR("Failed to set property {} to value {}", nameStr, realValue);
            return;
        }
        
        // Update property value in the container
        sharedThis->UpdatePropertyValuePublic(property, false, value, {});
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
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  // Get the property index and set it
  int prop = sharedThis->GetPropertyIndex(kPropertyBrightness);
  sharedThis->SetProperty(prop, brightness, status);
}

- (int)getBrightness:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return 0;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  // Get the property index and its value
  int prop = sharedThis->GetPropertyIndex(kPropertyBrightness);
  return sharedThis->GetProperty(prop, status);
}

- (void)setWhiteBalanceAuto:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  int prop = sharedThis->GetPropertyIndex(kPropertyAutoWhiteBalance);
  sharedThis->SetProperty(prop, 1, status);
}

- (void)setWhiteBalanceHoldCurrent:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  int prop = sharedThis->GetPropertyIndex(kPropertyAutoWhiteBalance);
  sharedThis->SetProperty(prop, 0, status);
}

- (void)setWhiteBalanceManual:(int)value status:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  // First disable auto white balance
  int autoProp = sharedThis->GetPropertyIndex(kPropertyAutoWhiteBalance);
  sharedThis->SetProperty(autoProp, 0, status);
  if (*status != 0) {
    return;
  }
  
  // Then set the white balance value
  int prop = sharedThis->GetPropertyIndex(kPropertyWhiteBalance);
  sharedThis->SetProperty(prop, value, status);
}

- (void)setExposureAuto:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  // Set the auto exposure property to enabled (1)
  int prop = sharedThis->GetPropertyIndex(kPropertyAutoExposure);
  sharedThis->SetProperty(prop, kPropertyAutoExposureOn, status);
}

- (void)setExposureHoldCurrent:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  // Set the auto exposure property to disabled (0)
  int prop = sharedThis->GetPropertyIndex(kPropertyAutoExposure);
  sharedThis->SetProperty(prop, kPropertyAutoExposureOff, status);
}

- (void)setExposureManual:(int)value status:(CS_Status*)status {
  auto sharedThis = self.cppImpl.lock();
  if (!sharedThis) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  
  // Make sure properties are cached
  if (!self.propertiesCached) {
    [self deviceCacheProperties];
  }
  
  // First disable auto exposure
  int autoProp = sharedThis->GetPropertyIndex(kPropertyAutoExposure);
  sharedThis->SetProperty(autoProp, kPropertyAutoExposureOff, status);
  if (*status != 0) {
    return;
  }
  
  // Then set the exposure value
  int prop = sharedThis->GetPropertyIndex(kPropertyExposure);
  sharedThis->SetProperty(prop, value, status);
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

// Property caching methods
- (void)deviceCacheProperties {
    if (self.uvcControl == nil) {
        return;
    }

    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
        OBJCERROR("Cannot cache properties: UsbCameraImpl not available");
        return;
    }

    // Cache basic properties
    [self cacheProperty:CAPPROPID_BRIGHTNESS withName:@kPropertyBrightness];
    [self cacheProperty:CAPPROPID_WHITEBALANCE withName:@kPropertyWhiteBalance];
    [self cacheProperty:CAPPROPID_EXPOSURE withName:@kPropertyExposure];
    [self cacheProperty:CAPPROPID_CONTRAST withName:@kPropertyContrast];
    [self cacheProperty:CAPPROPID_SATURATION withName:@kPropertySaturation];
    [self cacheProperty:CAPPROPID_SHARPNESS withName:@kPropertySharpness];
    [self cacheProperty:CAPPROPID_GAIN withName:@kPropertyGain];
    [self cacheProperty:CAPPROPID_GAMMA withName:@kPropertyGamma];
    [self cacheProperty:CAPPROPID_HUE withName:@kPropertyHue];
    [self cacheProperty:CAPPROPID_FOCUS withName:@kPropertyFocus];
    [self cacheProperty:CAPPROPID_ZOOM withName:@kPropertyZoom];
    [self cacheProperty:CAPPROPID_BACKLIGHTCOMP withName:@kPropertyBackLightCompensation];
    [self cacheProperty:CAPPROPID_POWERLINEFREQ withName:@kPropertyPowerLineFrequency];
    
    // Cache auto properties
    [self cacheAutoProperty:CAPPROPID_EXPOSURE withName:@kPropertyAutoExposure];
    [self cacheAutoProperty:CAPPROPID_WHITEBALANCE withName:@kPropertyAutoWhiteBalance];
    [self cacheAutoProperty:CAPPROPID_FOCUS withName:@kPropertyAutoFocus];
    
    self.propertiesCached = true;
}

- (void)cacheProperty:(uint32_t)propID withName:(NSString *)name {
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
        OBJCERROR("Cannot cache property: UsbCameraImpl not available");
        return;
    }
    
    if (self.uvcControl == nil) {
        OBJCWARNING("Cannot cache property {}: UVC control not initialized", [name UTF8String]);
        return;
    }
    
    // Get property limits
    int32_t minimum = 0, maximum = 0, defaultValue = 0;
    int32_t value = defaultValue;
    CS_Status status;
    
    std::string nameStr = std::string([name UTF8String]);
    
    // Get the property limits
    if (![self.uvcControl getPropertyLimits:propID 
                                      min:&minimum 
                                      max:&maximum 
                                 defValue:&defaultValue 
                                   status:&status]) {
        OBJCWARNING("Failed to get property limits for {}", nameStr);
        return;
    }
    
    // Get current value
    if (![self.uvcControl getProperty:propID withValue:&value status:&status]) {
        value = defaultValue;
        OBJCWARNING("Failed to get current value for {}: {}", 
                  nameStr, value);
        return;
    }
    
    // Create property
    auto& propertyCache = sharedThis->GetPropertyCache();
    propertyCache[nameStr] = propID;
    
    // Create the property implementation
    std::unique_ptr<PropertyImpl> prop;
    prop = std::make_unique<PropertyImpl>(nameStr);
    prop->propKind = CS_PROP_INTEGER;
    prop->value = value;
    prop->minimum = minimum;
    prop->maximum = maximum;
    prop->step = 1;  // Most camera properties use a step of 1
    prop->defaultValue = defaultValue;
    
    // Add the property to the container
    std::scoped_lock lock(sharedThis->GetMutex());
    int ndx = sharedThis->CreatePropertyPublic(nameStr, [&] { return std::move(prop); });
    
    // Notify that property has been created
    sharedThis->NotifyPropertyCreatedPublic(ndx, *sharedThis->GetPropertyPublic(ndx));
}

- (void)cacheAutoProperty:(uint32_t)propID withName:(NSString *)baseName {
    auto sharedThis = self.cppImpl.lock();
    if (!sharedThis) {
        OBJCERROR("Cannot cache auto property: UsbCameraImpl not available");
        return;
    }
    
    if (self.uvcControl == nil) {
        OBJCWARNING("Cannot cache auto property {}: UVC control not initialized", [baseName UTF8String]);
        return;
    }
    
    // Build auto mode property name
    std::string nameStr = std::string([baseName UTF8String]);
    
    // Get current auto mode status
    bool enabled = false;
    CS_Status status = 0;
    
    if(![self.uvcControl getAutoProperty:propID enabled:&enabled status:&status]) {
        OBJCWARNING("Failed to get auto property {}", nameStr);
        return;
    }
    
    // Create property
    std::unique_ptr<PropertyImpl> prop;
    prop = std::make_unique<PropertyImpl>(nameStr);
    prop->propKind = CS_PROP_BOOLEAN;
    prop->value = enabled ? 1 : 0;
    prop->minimum = 0;
    prop->maximum = 1;
    prop->step = 1;
    prop->defaultValue = 0;  // Default is manual mode
    
    // Add property to container
    std::scoped_lock lock(sharedThis->GetMutex());
    int ndx = sharedThis->CreatePropertyPublic(nameStr, [&] { return std::move(prop); });
    
    // Notify property created
    sharedThis->NotifyPropertyCreatedPublic(ndx, *sharedThis->GetPropertyPublic(ndx));
    
    // Map property name to ID
    auto& propertyAutoCache = sharedThis->GetPropertyAutoCache();
    propertyAutoCache[nameStr] = propID;
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

- (CMTime)findNearestFrameDuration:(int)fps {
  if (self.currentFormat == nil) {
    return CMTimeMake(1, fps);
  }

  NSArray<AVFrameRateRange*>* frameRates = self.currentFormat.videoSupportedFrameRateRanges;
  if (frameRates.count == 0) {
    return CMTimeMake(1, fps);
  }

  // Find the nearest frame duration
  CMTime nearestDuration = CMTimeMake(1, fps);
  double minDiff = DBL_MAX;

  for (AVFrameRateRange* range in frameRates) {
    CMTime minDuration = range.minFrameDuration;
    CMTime maxDuration = range.maxFrameDuration;
    
    // Calculate frame duration for current fps
    CMTime targetDuration = CMTimeMake(1, fps);
    
    // Check if within range
    if (CMTimeCompare(targetDuration, minDuration) >= 0 && 
        CMTimeCompare(targetDuration, maxDuration) <= 0) {
      return targetDuration;
    }
    
    // Calculate difference with min value
    double minDiffValue = fabs(CMTimeGetSeconds(targetDuration) - CMTimeGetSeconds(minDuration));
    if (minDiffValue < minDiff) {
      minDiff = minDiffValue;
      nearestDuration = minDuration;
    }
    
    // Calculate difference with max value
    double maxDiffValue = fabs(CMTimeGetSeconds(targetDuration) - CMTimeGetSeconds(maxDuration));
    if (maxDiffValue < minDiff) {
      minDiff = maxDiffValue;
      nearestDuration = maxDuration;
    }
  }

  OBJCDEBUG("Nearest fps: {}", nearestDuration.timescale / static_cast<double>(nearestDuration.value));
  
  return nearestDuration;
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
          [self findNearestFrameDuration:self.currentFPS];
      self.videoDevice.activeVideoMaxFrameDuration =
          [self findNearestFrameDuration:self.currentFPS];
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
  
  self.uvcControl.cppImpl = self.cppImpl;

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
