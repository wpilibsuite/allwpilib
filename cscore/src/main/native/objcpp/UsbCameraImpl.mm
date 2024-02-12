// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#include <iostream>

#include <vector>
#include <string>
#include <wpi/timestamp.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "Handle.h"
#include "Log.h"
#include "Notifier.h"
#include "Instance.h"
#include "c_util.h"
#include "cscore_cpp.h"
#include "UsbCameraImpl.h"

namespace cs {

UsbCameraImpl::UsbCameraImpl(std::string_view name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             std::string_view path)
    : SourceImpl{name, logger, notifier, telemetry} {
  UsbCameraImplObjc* objc = [[UsbCameraImplObjc alloc] init];
  objc.path = [[NSString alloc] initWithBytes:path.data()
                                       length:path.size()
                                     encoding:NSUTF8StringEncoding];
  m_objc = objc;
}
UsbCameraImpl::UsbCameraImpl(std::string_view name, wpi::Logger& logger,
                             Notifier& notifier, Telemetry& telemetry,
                             int deviceId)
    : SourceImpl{name, logger, notifier, telemetry} {
  UsbCameraImplObjc* objc = [[UsbCameraImplObjc alloc] init];
  objc.path = nil;
  objc.deviceId = deviceId;
  m_objc = objc;
}

UsbCameraImpl::~UsbCameraImpl() {
  m_objc = nil;
}

void UsbCameraImpl::Start() {
  [m_objc start];
}

// Property functions
void UsbCameraImpl::SetProperty(int property, int value, CS_Status* status) {
  [m_objc setProperty:property withValue:value status:status];
}
void UsbCameraImpl::SetStringProperty(int property, std::string_view value,
                                      CS_Status* status) {
  [m_objc setStringProperty:property withValue:&value status:status];
}

// Standard common camera properties
void UsbCameraImpl::SetBrightness(int brightness, CS_Status* status) {
  [m_objc setBrightness:brightness status:status];
}
int UsbCameraImpl::GetBrightness(CS_Status* status) const {
  return [m_objc getBrightness:status];
}
void UsbCameraImpl::SetWhiteBalanceAuto(CS_Status* status) {
  [m_objc setWhiteBalanceAuto:status];
}
void UsbCameraImpl::SetWhiteBalanceHoldCurrent(CS_Status* status) {
  [m_objc setWhiteBalanceHoldCurrent:status];
}
void UsbCameraImpl::SetWhiteBalanceManual(int value, CS_Status* status) {
  [m_objc setWhiteBalanceManual:value status:status];
}
void UsbCameraImpl::SetExposureAuto(CS_Status* status) {
  [m_objc setExposureAuto:status];
}
void UsbCameraImpl::SetExposureHoldCurrent(CS_Status* status) {
  [m_objc setExposureHoldCurrent:status];
}
void UsbCameraImpl::SetExposureManual(int value, CS_Status* status) {
  [m_objc setExposureManual:value status:status];
}

bool UsbCameraImpl::SetVideoMode(const VideoMode& mode, CS_Status* status) {
  return [m_objc setVideoMode:mode status:status];
}
bool UsbCameraImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                   CS_Status* status) {
  return [m_objc setPixelFormat:pixelFormat status:status];
}
bool UsbCameraImpl::SetResolution(int width, int height, CS_Status* status) {
  return [m_objc setResolutionWidth:width withHeight:height status:status];
}
bool UsbCameraImpl::SetFPS(int fps, CS_Status* status) {
  return [m_objc setFPS:fps status:status];
}

void UsbCameraImpl::NumSinksChanged() {
  [m_objc numSinksChanged];
}
void UsbCameraImpl::NumSinksEnabledChanged() {
  [m_objc numSinksEnabledChanged];
}

CS_Source CreateUsbCameraDev(std::string_view name, int dev,
                             CS_Status* status) {
  std::vector<UsbCameraInfo> devices = cs::EnumerateUsbCameras(status);
  if (static_cast<int>(devices.size()) > dev) {
    return CreateUsbCameraPath(name, devices[dev].path, status);
  }
  auto& inst = Instance::GetInstance();
  return inst.CreateSource(CS_SOURCE_USB, std::make_shared<UsbCameraImpl>(
                                              name, inst.logger, inst.notifier,
                                              inst.telemetry, dev));
}

CS_Source CreateUsbCameraPath(std::string_view name, std::string_view path,
                              CS_Status* status) {
  (void)status;
  auto& inst = Instance::GetInstance();
  auto val = std::make_shared<UsbCameraImpl>(name, inst.logger, inst.notifier,
                                             inst.telemetry, path);
  val->cppGetObjc().cppImpl = val;
  return inst.CreateSource(CS_SOURCE_USB, val);
}

std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status) {
  @autoreleasepool {
    (void)status;
    std::vector<UsbCameraInfo> retval;
    NSArray<AVCaptureDeviceType>* deviceTypes = @[
      AVCaptureDeviceTypeBuiltInWideAngleCamera,
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 140000
      AVCaptureDeviceTypeExternal
#else
      AVCaptureDeviceTypeExternalUnknown
#endif
    ];
    AVCaptureDeviceDiscoverySession* session = [AVCaptureDeviceDiscoverySession
        discoverySessionWithDeviceTypes:deviceTypes
                              mediaType:AVMediaTypeVideo
                               position:AVCaptureDevicePositionUnspecified];

    NSArray* captureDevices = [session devices];

    int count = 0;
    for (id device in captureDevices) {
      NSString* name = [device localizedName];
      NSString* uniqueIdentifier = [(AVCaptureDevice*)device uniqueID];
      retval.push_back(
          {count, [uniqueIdentifier UTF8String], [name UTF8String], {}});

      count++;
    }

    return retval;
  }
}

void SetUsbCameraPath(CS_Source source, std::string_view path,
                      CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return;
  }
  [static_cast<UsbCameraImpl&>(*data->source).cppGetObjc()
      setNewCameraPath:&path];
}

std::string GetUsbCameraPath(CS_Source source, CS_Status* status) {
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return std::string{};
  }
  std::string ret;
  [static_cast<UsbCameraImpl&>(*data->source).cppGetObjc()
      getCurrentCameraPath:&ret];
  return ret;
}

UsbCameraInfo GetUsbCameraInfo(CS_Source source, CS_Status* status) {
  UsbCameraInfo info;
  auto data = Instance::GetInstance().GetSource(source);
  if (!data || data->kind != CS_SOURCE_USB) {
    *status = CS_INVALID_HANDLE;
    return info;
  }

  [static_cast<UsbCameraImpl&>(*data->source).cppGetObjc()
      getCurrentCameraPath:&info.path];
  [static_cast<UsbCameraImpl&>(*data->source).cppGetObjc()
      getCameraName:&info.name];
  info.productId = 0;
  info.vendorId = 0;
  // ParseVidAndPid(info.path, &info.productId, &info.vendorId);
  info.dev = -1;  // We have lost dev information by this point in time.
  return info;
}

}  // namespace cs
