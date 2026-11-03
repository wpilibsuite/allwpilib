// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#import <AVFoundation/AVFoundation.h>

#include <memory>
#include <string_view>

#import "UsbCameraDelegate.hpp"
#import "UvcControlImpl.hpp"

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/util/PixelFormat.hpp"

// Quirk: exposure auto is 3 for on, 1 for off
#define PROPERTY_AUTO_EXPOSURE_ON 3
#define PROPERTY_AUTO_EXPOSURE_OFF 1

// Property names
#define PROPERTY_BRIGHTNESS "brightness"
#define PROPERTY_WHITE_BALANCE "white_balance_temperature"
#define PROPERTY_EXPOSURE "raw_exposure_time_absolute"
#define PROPERTY_CONTRAST "raw_contrast"
#define PROPERTY_SATURATION "raw_saturation"
#define PROPERTY_SHARPNESS "raw_sharpness"
#define PROPERTY_GAIN "gain"
#define PROPERTY_GAMMA "gamma"
#define PROPERTY_HUE "raw_hue"
#define PROPERTY_FOCUS "focus_absolute"
#define PROPERTY_ZOOM "zoom"
#define PROPERTY_BACK_LIGHT_COMPENSATION "backlight_compensation"
#define PROPERTY_POWER_LINE_FREQUENCY "power_line_frequency"

// Auto property names
#define PROPERTY_AUTO_EXPOSURE "exposure_auto"
#define PROPERTY_AUTO_WHITE_BALANCE "white_balance_automatic"
#define PROPERTY_AUTO_FOCUS "focus_auto"

namespace wpi::cs {
class UsbCameraImpl;
}

@interface UsbCameraImplObjc : NSObject

@property(nonatomic) AVCaptureDeviceFormat* currentFormat;
@property(nonatomic) int currentFPS;
@property(nonatomic) std::weak_ptr<wpi::cs::UsbCameraImpl> cppImpl;
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

- (bool)setVideoMode:(const wpi::cs::VideoMode&)mode status:(CS_Status*)status;
- (bool)setPixelFormat:(wpi::util::PixelFormat)pixelFormat
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
