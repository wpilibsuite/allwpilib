// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#import <AVFoundation/AVFoundation.h>
#import "UsbCameraDelegate.hpp"
#import "UsbCameraImplObjc.hpp"

#include <memory>
#include <string>
#include <optional>

#include "wpi/util/StringMap.hpp"

#include "SourceImpl.hpp"

namespace cs {
struct CameraFPSRange {
  int min;
  int max;

  bool IsWithinRange(int fps) { return fps >= min && fps <= max; }
};

struct CameraModeStore {
  VideoMode mode;
  AVCaptureDeviceFormat* format;
  std::vector<CameraFPSRange> fpsRanges;
};

class UsbCameraImpl : public SourceImpl {
 public:
  UsbCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier,
                Telemetry& telemetry, std::string_view path);
  UsbCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier,
                Telemetry& telemetry, int deviceId);
  ~UsbCameraImpl() override;

  void Start() override;

  // Property functions
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, std::string_view value,
                         CS_Status* status) override;

  // Standard common camera properties
  void SetBrightness(int brightness, CS_Status* status) override;
  int GetBrightness(CS_Status* status) const override;
  void SetWhiteBalanceAuto(CS_Status* status) override;
  void SetWhiteBalanceHoldCurrent(CS_Status* status) override;
  void SetWhiteBalanceManual(int value, CS_Status* status) override;
  void SetExposureAuto(CS_Status* status) override;
  void SetExposureHoldCurrent(CS_Status* status) override;
  void SetExposureManual(int value, CS_Status* status) override;

  bool SetVideoMode(const VideoMode& mode, CS_Status* status) override;
  bool SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                      CS_Status* status) override;
  bool SetResolution(int width, int height, CS_Status* status) override;
  bool SetFPS(int fps, CS_Status* status) override;

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  cs::Notifier& objcGetNotifier() { return m_notifier; }

  void objcSwapVideoModes(std::vector<VideoMode>& modes) {
    std::scoped_lock lock(m_mutex);
    m_videoModes.swap(modes);
  }

  void objcSetVideoMode(const VideoMode& mode) {
    std::scoped_lock lock(m_mutex);
    m_mode = mode;
  }

  void objcPutFrame(std::unique_ptr<Image> image, Frame::Time time) {
    PutFrame(std::move(image), time);
  }

  const VideoMode& objcGetVideoMode() const { return m_mode; }

  std::vector<CameraModeStore>& objcGetPlatformVideoModes() {
    return m_platformModes;
  }

  wpi::Logger& objcGetLogger() { return m_logger; }

  UsbCameraImplObjc* cppGetObjc() { return m_objc; }

  int CreatePropertyPublic(std::string_view name, std::function<std::unique_ptr<PropertyImpl>()> newFunc) {
    return CreateProperty(name, newFunc);
  }

  PropertyImpl* GetPropertyPublic(int property) {
    return GetProperty(property);
  }

  void NotifyPropertyCreatedPublic(int propIndex, PropertyImpl& prop) {
    NotifyPropertyCreated(propIndex, prop);
  }

  void UpdatePropertyValuePublic(int property, bool setString, int value, std::string_view valueStr) {
    UpdatePropertyValue(property, setString, value, valueStr);
  }

  wpi::mutex& GetMutex() { return m_mutex; }

  // Property cache accessors
  wpi::StringMap<uint32_t>& GetPropertyCache() { return m_propertyCache; }
  wpi::StringMap<uint32_t>& GetPropertyAutoCache() { return m_propertyAutoCache; }

 private:
  UsbCameraImplObjc* m_objc;
  std::vector<CameraModeStore> m_platformModes;
  
  // Property caches
  wpi::StringMap<uint32_t> m_propertyCache;
  wpi::StringMap<uint32_t> m_propertyAutoCache;
};
}  // namespace cs
