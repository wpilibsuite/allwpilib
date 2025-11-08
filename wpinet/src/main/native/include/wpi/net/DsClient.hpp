// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <wpi/Signal.h>

namespace wpi {

class Logger;

namespace uv {
class Loop;
class Tcp;
class Timer;
}  // namespace uv

class DsClient : public std::enable_shared_from_this<DsClient> {
  struct private_init {};

 public:
  static std::shared_ptr<DsClient> Create(wpi::uv::Loop& loop,
                                          wpi::Logger& logger) {
    return std::make_shared<DsClient>(loop, logger, private_init{});
  }

  DsClient(wpi::uv::Loop& loop, wpi::Logger& logger, const private_init&);
  ~DsClient();
  DsClient(const DsClient&) = delete;
  DsClient& operator=(const DsClient&) = delete;

  void Close();

  sig::Signal<std::string_view> setIp;
  sig::Signal<> clearIp;

 private:
  void Connect();
  void HandleIncoming(std::string_view in);
  void ParseJson();

  wpi::Logger& m_logger;

  std::shared_ptr<wpi::uv::Tcp> m_tcp;
  std::shared_ptr<wpi::uv::Timer> m_timer;

  std::string m_json;
};

}  // namespace wpi
