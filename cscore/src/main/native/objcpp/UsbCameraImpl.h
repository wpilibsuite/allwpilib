#pragma once

#import <AVFoundation/AVFoundation.h>

#include <memory>
#include <string>
#include <optional>

#include "SourceImpl.h"

namespace cs {
class UsbCameraImpl : public SourceImpl {
 public:
  UsbCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier, Telemetry& telemetry, std::string_view path);
  UsbCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier, Telemetry& telemetry, int deviceId);
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

  void SetNewPath(std::string_view path, CS_Status* status);
  std::string GetCurrentPath();
  std::string GetCameraName();
 private:

  id m_objc;
};
}
