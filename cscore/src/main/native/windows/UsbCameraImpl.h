// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_USBCAMERAIMPL_H_
#define CSCORE_USBCAMERAIMPL_H_

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#include <ks.h>  // NOLINT(build/include_order)

#include <ksmedia.h>  // NOLINT(build/include_order)

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <Dbt.h>
#include <wpi/STLExtras.h>
#include <wpi/SmallVector.h>
#include <wpi/Twine.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>

#include "ComCreators.h"
#include "ComPtr.h"
#include "SourceImpl.h"
#include "UsbCameraProperty.h"
#include "WindowsMessagePump.h"

namespace cs {

class UsbCameraImpl : public SourceImpl,
                      public std::enable_shared_from_this<UsbCameraImpl> {
 public:
  UsbCameraImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
                Telemetry& telemetry, const wpi::Twine& path);
  UsbCameraImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
                Telemetry& telemetry, int deviceId);
  ~UsbCameraImpl() override;

  void Start();

  // Property functions
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, const wpi::Twine& value,
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

  void ProcessFrame(IMFSample* sample, const VideoMode& mode);
  void PostRequestNewFrame();

  void SetPath(const wpi::Twine& path, CS_Status* status);
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
        : kind(kind_), data{0}, from(std::this_thread::get_id()) {}

    Kind kind;
    int data[4];
    std::string dataStr;
    std::thread::id from;
  };

 protected:
  std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      const wpi::Twine& name) const override;

  // Cache properties.  Immediately successful if properties are already cached.
  // If they are not, tries to connect to the camera to do so; returns false and
  // sets status to CS_SOURCE_IS_DISCONNECTED if that too fails.
  bool CacheProperties(CS_Status* status) const override;

 private:
  LRESULT PumpMain(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

  bool CheckDeviceChange(WPARAM wParam, DEV_BROADCAST_HDR* pHdr,
                         bool* connected);

  // Functions used by CameraThreadMain()
  void DeviceDisconnect();
  bool DeviceConnect();
  bool DeviceStreamOn();
  bool DeviceStreamOff();
  CS_StatusValue DeviceSetMode();
  void DeviceCacheMode();
  void DeviceCacheProperty(std::unique_ptr<UsbCameraProperty> rawProp,
                           IMFSourceReader* sourceReader);
  void DeviceCacheProperties();
  void DeviceCacheVideoModes();
  template <typename TagProperty, typename IAM>
  void DeviceAddProperty(const wpi::Twine& name_, TagProperty tag,
                         IAM* pProcAmp);

  ComPtr<IMFMediaType> DeviceCheckModeValid(const VideoMode& toCheck);

  // Command helper functions
  CS_StatusValue DeviceProcessCommand(std::unique_lock<wpi::mutex>& lock,
                                      Message::Kind msgKind,
                                      const Message* msg);
  CS_StatusValue DeviceCmdSetMode(std::unique_lock<wpi::mutex>& lock,
                                  const Message& msg);
  CS_StatusValue DeviceCmdSetProperty(std::unique_lock<wpi::mutex>& lock,
                                      const Message& msg);

  // Property helper functions
  int RawToPercentage(const UsbCameraProperty& rawProp, int rawValue);
  int PercentageToRaw(const UsbCameraProperty& rawProp, int percentValue);

  void StartMessagePump();

  //
  // Variables only used within camera thread
  //
  bool m_streaming{false};
  bool m_wasStreaming{false};
  bool m_modeSet{false};
  int m_connectVerbose{1};
  bool m_deviceValid{false};

  ComPtr<IMFMediaSource> m_mediaSource;
  ComPtr<IMFSourceReader> m_sourceReader;
  ComPtr<SourceReaderCB> m_imageCallback;
  std::unique_ptr<cs::WindowsMessagePump> m_messagePump;
  ComPtr<IMFMediaType> m_currentMode;

  std::string m_path;

  std::wstring m_widePath;
  int m_deviceId;

  std::vector<std::pair<VideoMode, ComPtr<IMFMediaType>>> m_windowsVideoModes;
};

}  // namespace cs

#endif  // CSCORE_USBCAMERAIMPL_H_
