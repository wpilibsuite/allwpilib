/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_HTTPCAMERAIMPL_H_
#define CSCORE_HTTPCAMERAIMPL_H_

#include <atomic>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <llvm/SmallString.h>
#include <llvm/StringMap.h>
#include <support/HttpUtil.h>
#include <support/condition_variable.h>
#include <support/raw_istream.h>

#include "SourceImpl.h"
#include "cscore_cpp.h"

namespace cs {

class HttpCameraImpl : public SourceImpl {
 public:
  HttpCameraImpl(llvm::StringRef name, CS_HttpCameraKind kind);
  ~HttpCameraImpl() override;

  void Start();

  // Property functions
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, llvm::StringRef value,
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
  bool SetUrls(llvm::ArrayRef<std::string> urls, CS_Status* status);
  std::vector<std::string> GetUrls() const;

  // Property data
  class PropertyData : public PropertyImpl {
   public:
    PropertyData() = default;
    explicit PropertyData(llvm::StringRef name_) : PropertyImpl{name_} {}
    PropertyData(llvm::StringRef name_, llvm::StringRef httpParam_,
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
      llvm::StringRef name) const override;

  bool CacheProperties(CS_Status* status) const override;

  void CreateProperty(llvm::StringRef name, llvm::StringRef httpParam,
                      bool viaSettings, CS_PropertyKind kind, int minimum,
                      int maximum, int step, int defaultValue, int value) const;

  template <typename T>
  void CreateEnumProperty(llvm::StringRef name, llvm::StringRef httpParam,
                          bool viaSettings, int defaultValue, int value,
                          std::initializer_list<T> choices) const;

 private:
  // The camera streaming thread
  void StreamThreadMain();

  // Functions used by StreamThreadMain()
  wpi::HttpConnection* DeviceStreamConnect(
      llvm::SmallVectorImpl<char>& boundary);
  void DeviceStream(wpi::raw_istream& is, llvm::StringRef boundary);
  bool DeviceStreamFrame(wpi::raw_istream& is, std::string& imageBuf);

  // The camera settings thread
  void SettingsThreadMain();
  void DeviceSendSettings(wpi::HttpRequest& req);

  std::atomic_bool m_connected{false};
  std::atomic_bool m_active{true};  // set to false to terminate thread
  std::thread m_streamThread;
  std::thread m_settingsThread;

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

  wpi::condition_variable m_sinkEnabledCond;

  llvm::StringMap<llvm::SmallString<16>> m_settings;
  wpi::condition_variable m_settingsCond;

  llvm::StringMap<llvm::SmallString<16>> m_streamSettings;
  std::atomic_bool m_streamSettingsUpdated{false};
};

class AxisCameraImpl : public HttpCameraImpl {
 public:
  explicit AxisCameraImpl(llvm::StringRef name)
      : HttpCameraImpl{name, CS_HTTP_AXIS} {}
#if 0
  void SetProperty(int property, int value, CS_Status* status) override;
  void SetStringProperty(int property, llvm::StringRef value,
                         CS_Status* status) override;
#endif
 protected:
  bool CacheProperties(CS_Status* status) const override;
};

}  // namespace cs

#endif  // CSCORE_HTTPCAMERAIMPL_H_
