// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#import <AVFoundation/AVFoundation.h>
#import "UsbCameraDelegate.h"
#import "UvcControlImpl.h"

#include <memory>
#include <string_view>
#include "cscore_cpp.h"

// Quirk: exposure auto is 3 for on, 1 for off
#define kPropertyAutoExposureOn 3
#define kPropertyAutoExposureOff 1

// Property names
#define kPropertyBrightness "brightness"
#define kPropertyWhiteBalance "white_balance_temperature"
#define kPropertyExposure "raw_exposure_time_absolute"
#define kPropertyContrast "raw_contrast"
#define kPropertySaturation "raw_saturation"
#define kPropertySharpness "raw_sharpness"
#define kPropertyGain "gain"
#define kPropertyGamma "gamma"
#define kPropertyHue "raw_hue"
#define kPropertyFocus "focus_absolute"
#define kPropertyZoom "zoom"
#define kPropertyBackLightCompensation "backlight_compensation"
#define kPropertyPowerLineFrequency "power_line_frequency"

// Auto property names
#define kPropertyAutoExposure "exposure_auto"
#define kPropertyAutoWhiteBalance "white_balance_automatic"
#define kPropertyAutoFocus "focus_auto"

namespace cs {
class UsbCameraImpl;
}

@interface UsbCameraImplObjc : NSObject

@property(nonatomic) AVCaptureDeviceFormat* currentFormat;
@property(nonatomic) int currentFPS;
@property(nonatomic) std::weak_ptr<cs::UsbCameraImpl> cppImpl;
@property(nonatomic) dispatch_queue_t sessionQueue;
@property(nonatomic) NSString* path;
@property(nonatomic) int deviceId;
@property(nonatomic) bool propertiesCached;
@property(nonatomic) bool streaming;
@property(nonatomic) bool deviceValid;
@property(nonatomic) bool isAuthorized;

@property(nonatomic) AVCaptureDevice* videoDevice;
@property(nonatomic) AVCaptureDeviceInput* videoInput;
@property(nonatomic) UsbCameraDelegate* callback;
@property(nonatomic) UvcControlImpl* uvcControl;
@property(nonatomic) AVCaptureVideoDataOutput* videoOutput;
@property(nonatomic) AVCaptureSession* session;

- (void)start;

// Property functions
- (void)setProperty:(int)property
          withValue:(int)value
             status:(CS_Status*)status;
- (void)setStringProperty:(int)property
                withValue:(std::string_view*)value
                   status:(CS_Status*)status;

// Standard common camera properties
- (void)setBrightness:(int)brightness status:(CS_Status*)status;
- (int)getBrightness:(CS_Status*)status;
- (void)setWhiteBalanceAuto:(CS_Status*)status;
- (void)setWhiteBalanceHoldCurrent:(CS_Status*)status;
- (void)setWhiteBalanceManual:(int)value status:(CS_Status*)status;
- (void)setExposureAuto:(CS_Status*)status;
- (void)setExposureHoldCurrent:(CS_Status*)status;
- (void)setExposureManual:(int)value status:(CS_Status*)status;

- (bool)setVideoMode:(const cs::VideoMode&)mode status:(CS_Status*)status;
- (bool)setPixelFormat:(cs::VideoMode::PixelFormat)pixelFormat
                status:(CS_Status*)status;
- (bool)setResolutionWidth:(int)width
                withHeight:(int)height
                    status:(CS_Status*)status;
- (bool)setFPS:(int)fps status:(CS_Status*)status;

- (void)numSinksChanged;
- (void)numSinksEnabledChanged;

- (void)getCurrentCameraPath:(std::string*)path;
- (void)getCameraName:(std::string*)name;
- (void)setNewCameraPath:(std::string_view*)path;

- (void)deviceCacheProperties;
- (void)cacheProperty:(uint32_t)propID withName:(NSString *)name;
- (void)cacheAutoProperty:(uint32_t)propID withName:(NSString *)baseName;

@end
