// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#import <AVFoundation/AVFoundation.h>
#import <IOKit/IOCFPlugIn.h>
#import <IOKit/usb/IOUSBLib.h>

#include <memory>
#include <string_view>

#import "UsbCameraDelegate.hpp"

#include "wpi/cs/cscore_cpp.hpp"

// Status code definition
#define CS_UVC_STATUS_ERROR -3001
#define CS_UVC_STATUS_DEVICE_DISCONNECTED -3002

// UVC control selector definitions
#define UVC_INPUT_TERMINAL_ID 0x01

// Camera terminal control selectors
#define CT_AE_MODE_CONTROL 0x02
#define CT_AE_PRIORITY_CONTROL 0x03
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL 0x04
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL 0x05
#define CT_FOCUS_ABSOLUTE_CONTROL 0x06
#define CT_FOCUS_RELATIVE_CONTROL 0x07
#define CT_FOCUS_AUTO_CONTROL 0x08
#define CT_ZOOM_ABSOLUTE_CONTROL 0x0B
#define CT_ZOOM_RELATIVE_CONTROL 0x0C

// Processing unit control selectors
#define PU_BACKLIGHT_COMPENSATION_CONTROL 0x01
#define PU_BRIGHTNESS_CONTROL 0x02
#define PU_CONTRAST_CONTROL 0x03
#define PU_GAIN_CONTROL 0x04
#define PU_POWER_LINE_FREQUENCY_CONTROL 0x05
#define PU_HUE_CONTROL 0x06
#define PU_SATURATION_CONTROL 0x07
#define PU_SHARPNESS_CONTROL 0x08
#define PU_GAMMA_CONTROL 0x09
#define PU_WHITE_BALANCE_TEMPERATURE_CONTROL 0x0A
#define PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL 0x0B
#define PU_WHITE_BALANCE_COMPONENT_CONTROL 0x0C
#define PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL 0x0D
#define PU_HUE_AUTO_CONTROL 0x10
#define PU_CONTRAST_AUTO_CONTROL 0x13

// Camera request error code
#define VC_REQUEST_ERROR_CODE_CONTROL 0x02

// UVC control interface definitions
#define UVC_CONTROL_INTERFACE_CLASS 14
#define UVC_CONTROL_INTERFACE_SUBCLASS 1

// UVC control request types
#define UVC_SET_CUR 0x01
#define UVC_GET_CUR 0x81
#define UVC_GET_MIN 0x82
#define UVC_GET_MAX 0x83
#define UVC_GET_RES 0x84
#define UVC_GET_INFO 0x86
#define UVC_GET_DEF 0x87

// Camera property ID definitions
#define CAPPROPID_EXPOSURE 1
#define CAPPROPID_FOCUS 2
#define CAPPROPID_ZOOM 3
#define CAPPROPID_WHITEBALANCE 4
#define CAPPROPID_GAIN 5
#define CAPPROPID_BRIGHTNESS 6
#define CAPPROPID_CONTRAST 7
#define CAPPROPID_SATURATION 8
#define CAPPROPID_GAMMA 9
#define CAPPROPID_HUE 10
#define CAPPROPID_SHARPNESS 11
#define CAPPROPID_BACKLIGHTCOMP 12
#define CAPPROPID_POWERLINEFREQ 13
#define CAPPROPID_LAST 14

namespace wpi::cs {
class UsbCameraImpl;
}

@interface UvcControlImpl : NSObject

@property(nonatomic) IOUSBInterfaceInterface190** controlInterface;
@property(nonatomic) uint32_t processingUnitID;
@property(nonatomic) std::weak_ptr<wpi::cs::UsbCameraImpl> cppImpl;

// Create from AVCaptureDevice
+ (instancetype)createFromAVCaptureDevice:(AVCaptureDevice*)device status:(CS_Status*)status;

// Initialize with USB vendor/product/location
- (instancetype)initWithVendorId:(uint16_t)vid 
                      productId:(uint16_t)pid 
                       location:(uint32_t)location
                         status:(CS_Status*)status;

- (void)dealloc;

// Basic property control
- (bool)setProperty:(uint32_t)propID 
          withValue:(int32_t)value
             status:(CS_Status*)status;
- (bool)getProperty:(uint32_t)propID 
          withValue:(int32_t*)value
             status:(CS_Status*)status;

// Auto mode control
- (bool)setAutoProperty:(uint32_t)propID 
               enabled:(bool)enabled
                status:(CS_Status*)status;
- (bool)getAutoProperty:(uint32_t)propID 
               enabled:(bool*)enabled
                status:(CS_Status*)status;

// Property range query
- (bool)getPropertyLimits:(uint32_t)propID
                     min:(int32_t*)min
                     max:(int32_t*)max
                defValue:(int32_t*)defValue
                  status:(CS_Status*)status;

@end 