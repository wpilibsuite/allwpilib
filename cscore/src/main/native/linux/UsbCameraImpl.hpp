// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_USBCAMERAIMPL_H_
#define CSCORE_USBCAMERAIMPL_H_

#include <linux/videodev2.h>

#include <atomic>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include "SourceImpl.h"
#include "UsbCameraBuffer.h"
#include "UsbCameraProperty.h"

namespace cs {

class Notifier;
class Telemetry;

class UsbCameraImpl : public SourceImpl {
 public:
  UsbCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier,
                Telemetry& telemetry, std::string_view path);
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

  void SetPath(std::string_view path, CS_Status* status);
  std::string GetPath() const;

  // Messages passed to/from camera thread
  struct Message {
    enum Kind {
      kNone = 0,
      kCmdSetPath,
      kCmdSetMode,
      kCmdSetPixelFormat,
      kCmdSetResolution,
      kCmdSetFPS,
      kCmdSetProperty,
      kCmdSetPropertyStr,
      kNumSinksChanged,         // no response
      kNumSinksEnabledChanged,  // no response
      // Responses
      kOk,
      kError
    };

    explicit Message(Kind kind_)
        : kind(kind_), from(std::this_thread::get_id()) {}

    Kind kind;
    int data[4];
    std::string dataStr;
    std::thread::id from;
  };

 protected:
  std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      std::string_view name) const override;

  // Cache properties.  Immediately successful if properties are already cached.
  // If they are not, tries to connect to the camera to do so; returns false and
  // sets status to CS_SOURCE_IS_DISCONNECTED if that too fails.
  bool CacheProperties(CS_Status* status) const override;

 private:
  // Send a message to the camera thread and wait for a response (generic)
  CS_StatusValue SendAndWait(Message&& msg) const;
  // Send a message to the camera thread with no response
  void Send(Message&& msg) const;

  // The camera processing thread
  void CameraThreadMain();

  // Functions used by CameraThreadMain()
  void DeviceDisconnect();
  void DeviceConnect();
  bool DeviceStreamOn();
  bool DeviceStreamOff();
  void DeviceProcessCommands();
  void DeviceSetMode();
  void DeviceSetFPS();
  void DeviceCacheMode();
  void DeviceCacheProperty(std::unique_ptr<UsbCameraProperty> rawProp);
  void DeviceCacheProperties();
  void DeviceCacheVideoModes();

  // Command helper functions
  CS_StatusValue DeviceProcessCommand(std::unique_lock<wpi::mutex>& lock,
                                      const Message& msg);
  CS_StatusValue DeviceCmdSetMode(std::unique_lock<wpi::mutex>& lock,
                                  const Message& msg);
  CS_StatusValue DeviceCmdSetProperty(std::unique_lock<wpi::mutex>& lock,
                                      const Message& msg);
  CS_StatusValue DeviceCmdSetPath(std::unique_lock<wpi::mutex>& lock,
                                  const Message& msg);

  // Property helper functions
  int RawToPercentage(const UsbCameraProperty& rawProp, int rawValue);
  int PercentageToRaw(const UsbCameraProperty& rawProp, int percentValue);

  void SetQuirks();

  //
  // Variables only used within camera thread
  //
  bool m_streaming;
  bool m_modeSetPixelFormat{false};
  bool m_modeSetResolution{false};
  bool m_modeSetFPS{false};
  int m_connectVerbose{1};
  unsigned m_capabilities = 0;
  // Number of buffers to ask OS for
  static constexpr int kNumBuffers = 4;
  std::array<UsbCameraBuffer, kNumBuffers> m_buffers;

  std::atomic_int m_fd;
  std::atomic_int m_command_fd;  // for command eventfd

  std::atomic_bool m_active;  // set to false to terminate thread
  std::thread m_cameraThread;

  // Quirks
  bool m_lifecam_exposure{false};    // Microsoft LifeCam exposure
  bool m_ps3eyecam_exposure{false};  // PS3 Eyecam exposure
  bool m_ov9281_exposure{false};     // Arducam OV9281 exposure
  bool m_picamera{false};            // Raspberry Pi camera

  //
  // Variables protected by m_mutex
  //

  // Message queues
  mutable std::vector<Message> m_commands;
  mutable std::vector<std::pair<std::thread::id, CS_StatusValue>> m_responses;
  mutable wpi::condition_variable m_responseCv;

  // Path
  std::string m_path;
};

}  // namespace cs

#endif  // CSCORE_USBCAMERAIMPL_H_
