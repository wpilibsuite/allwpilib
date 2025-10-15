// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_HTTPCAMERAIMPL_H_
#define CSCORE_HTTPCAMERAIMPL_H_

#include <atomic>
#include <functional>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <wpi/StringMap.h>
#include <wpi/condition_variable.h>
#include <wpi/raw_istream.h>
#include <wpinet/HttpUtil.h>

#include "SourceImpl.h"
#include "cscore_cpp.h"

namespace cs {

class HttpCameraImpl : public SourceImpl {
 public:
  HttpCameraImpl(std::string_view name, CS_HttpCameraKind kind,
                 wpi::Logger& logger, Notifier& notifier, Telemetry& telemetry);
  ~HttpCameraImpl() override;

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

  void NumSinksChanged() override;
  void NumSinksEnabledChanged() override;

  CS_HttpCameraKind GetKind() const;
  bool SetUrls(std::span<const std::string> urls, CS_Status* status);
  std::vector<std::string> GetUrls() const;

  // Property data
  class PropertyData : public PropertyImpl {
   public:
    PropertyData() = default;
    explicit PropertyData(std::string_view name_) : PropertyImpl{name_} {}
    PropertyData(std::string_view name_, std::string_view httpParam_,
                 bool viaSettings_, CS_PropertyKind kind_, int minimum_,
                 int maximum_, int step_, int defaultValue_, int value_)
        : PropertyImpl(name_, kind_, step_, defaultValue_, value_),
          viaSettings(viaSettings_),
          httpParam(httpParam_) {
      hasMinimum = true;
      minimum = minimum_;
      hasMaximum = true;
      maximum = maximum_;
    }
    ~PropertyData() override = default;

    bool viaSettings{false};
    std::string httpParam;
  };

 protected:
  std::unique_ptr<PropertyImpl> CreateEmptyProperty(
      std::string_view name) const override;

  bool CacheProperties(CS_Status* status) const override;

  void CreateProperty(std::string_view name, std::string_view httpParam,
                      bool viaSettings, CS_PropertyKind kind, int minimum,
                      int maximum, int step, int defaultValue, int value) const;

  template <typename T>
  void CreateEnumProperty(std::string_view name, std::string_view httpParam,
                          bool viaSettings, int defaultValue, int value,
                          std::initializer_list<T> choices) const;

 private:
  // The camera streaming thread
  void StreamThreadMain();

  // Functions used by StreamThreadMain()
  wpi::HttpConnection* DeviceStreamConnect(
      wpi::SmallVectorImpl<char>& boundary);
  void DeviceStream(wpi::raw_istream& is, std::string_view boundary);
  bool DeviceStreamFrame(wpi::raw_istream& is, std::string& imageBuf);

  // The camera settings thread
  void SettingsThreadMain();
  void DeviceSendSettings(wpi::HttpRequest& req);

  // The monitor thread
  void MonitorThreadMain();

  std::atomic_bool m_connected{false};
  std::atomic_bool m_active{true};  // set to false to terminate thread
  std::thread m_streamThread;
  std::thread m_settingsThread;
  std::thread m_monitorThread;

  //
  // Variables protected by m_mutex
  //

  // The camera connections
  std::unique_ptr<wpi::HttpConnection> m_streamConn;
  std::unique_ptr<wpi::HttpConnection> m_settingsConn;

  CS_HttpCameraKind m_kind;

  std::vector<wpi::HttpLocation> m_locations;
  size_t m_nextLocation{0};
  int m_prefLocation{-1};  // preferred location

  std::atomic_int m_frameCount{0};

  wpi::condition_variable m_sinkEnabledCond;

  wpi::StringMap<std::string> m_settings;
  wpi::condition_variable m_settingsCond;

  wpi::StringMap<std::string> m_streamSettings;
  std::atomic_bool m_streamSettingsUpdated{false};

  wpi::condition_variable m_monitorCond;
};

class AxisCameraImpl : public HttpCameraImpl {
 public:
  AxisCameraImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier,
                 Telemetry& telemetry)
      : HttpCameraImpl{name, CS_HTTP_AXIS, logger, notifier, telemetry} {}
#if 0
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, std::string_view value,
                         CS_Status* status) override;
#endif
 protected:
  bool CacheProperties(CS_Status* status) const override;
};

}  // namespace cs

#endif  // CSCORE_HTTPCAMERAIMPL_H_
