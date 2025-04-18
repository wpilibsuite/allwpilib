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

@end
