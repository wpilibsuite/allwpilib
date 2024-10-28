// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DeploySession.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpinet/uv/Error.h>
#include <wpinet/uv/GetAddrInfo.h>
#include <wpinet/uv/Work.h>
#include <wpinet/uv/util.h>

#include "SshSession.h"

using namespace rtns;

#ifdef ERROR
#undef ERROR
#endif

// Macros to make logging easier.
#define INFO(fmt, ...) WPI_INFO(m_logger, fmt, __VA_ARGS__)
#define DEBUG(fmt, ...) WPI_DEBUG(m_logger, fmt, __VA_ARGS__)
#define ERROR(fmt, ...) WPI_DEBUG(m_logger, fmt, __VA_ARGS__)
#define SUCCESS(fmt, ...) WPI_LOG(m_logger, kLogSuccess, fmt, __VA_ARGS__)

// roboRIO SSH constants.
static constexpr int kPort = 22;
static constexpr std::string_view kUsername = "admin";
static constexpr std::string_view kPassword = "";

std::unordered_map<std::string, std::future<int>> s_outstanding;
std::unordered_map<std::string, std::future<DeviceStatus>> s_outstandingStatus;

DeploySession::DeploySession(wpi::Logger& logger) : m_logger{logger} {}

template <typename T>
struct SafeDeleter {
  explicit SafeDeleter(T d) : deleter(d) {}
  ~SafeDeleter() noexcept { deleter(); }
  T deleter;
};

std::future<int>* DeploySession::GetFuture(const std::string& macAddress) {
  auto itr = s_outstanding.find(macAddress);
  if (itr == s_outstanding.end()) {
    return nullptr;
  }
  return &itr->second;
}

void DeploySession::DestroyFuture(const std::string& macAddress) {
  s_outstanding.erase(macAddress);
}

std::future<DeviceStatus>* DeploySession::GetStatusFuture(
    const std::string& macAddress) {
  auto itr = s_outstandingStatus.find(macAddress);
  if (itr == s_outstandingStatus.end()) {
    return nullptr;
  }
  return &itr->second;
}

void DeploySession::DestroyStatusFuture(const std::string& macAddress) {
  s_outstandingStatus.erase(macAddress);
}

bool DeploySession::ChangeTeamNumber(const std::string& macAddress,
                                     int teamNumber, unsigned int ipAddress) {
  auto itr = s_outstanding.find(macAddress);
  if (itr != s_outstanding.end()) {
    return false;
  }

  std::future<int> future = std::async(
      std::launch::async, [this, ipAddress, teamNumber, mac = macAddress]() {
        //  Convert to IP address.
        wpi::SmallString<16> ip;
        in_addr addr;
        addr.s_addr = ipAddress;
        wpi::uv::AddrToName(addr, &ip);
        DEBUG("Trying to establish SSH connection to {}.", ip.str());
        try {
          SshSession session{ip.str(), kPort, kUsername, kPassword, m_logger};
          session.Open();
          DEBUG("SSH connection to {} was successful.", ip.str());

          SUCCESS("roboRIO Connected!");

          try {
            session.Execute(fmt::format(
                "/usr/local/natinst/bin/nirtcfg "
                "--file=/etc/natinst/share/ni-rt.ini --set "
                "section=systemsettings,token=host_name,value=roboRIO-{"
                "}-FRC ; sync",
                teamNumber));
          } catch (const SshSession::SshException& e) {
            ERROR("An exception occurred: {}", e.what());
            throw e;
          }
        } catch (const SshSession::SshException& e) {
          DEBUG("SSH connection to {} failed with {}.", ip.str(), e.what());
          throw e;
        }
        return 0;
      });

  s_outstanding[macAddress] = std::move(future);
  return true;
}

bool DeploySession::Reboot(const std::string& macAddress,
                           unsigned int ipAddress) {
  auto itr = s_outstanding.find(macAddress);
  if (itr != s_outstanding.end()) {
    return false;
  }

  std::future<int> future =
      std::async(std::launch::async, [this, ipAddress, mac = macAddress]() {
        //  Convert to IP address.
        wpi::SmallString<16> ip;
        in_addr addr;
        addr.s_addr = ipAddress;
        wpi::uv::AddrToName(addr, &ip);
        DEBUG("Trying to establish SSH connection to {}.", ip.str());
        try {
          SshSession session{ip.str(), kPort, kUsername, kPassword, m_logger};
          session.Open();
          DEBUG("SSH connection to {} was successful.", ip.str());

          SUCCESS("roboRIO Connected!");

          try {
            session.Execute(fmt::format("sync ; shutdown -r now"));
          } catch (const SshSession::SshException& e) {
            ERROR("An exception occurred: {}", e.what());
            throw e;
          }
        } catch (const SshSession::SshException& e) {
          DEBUG("SSH connection to {} failed with {}.", ip.str(), e.what());
          throw e;
        }
        return 0;
      });

  s_outstanding[macAddress] = std::move(future);
  return true;
}

bool DeploySession::Blink(const std::string& macAddress,
                          unsigned int ipAddress) {
  auto itr = s_outstanding.find(macAddress);
  if (itr != s_outstanding.end()) {
    return false;
  }

  std::future<int> future =
      std::async(std::launch::async, [this, ipAddress, mac = macAddress]() {
        //  Convert to IP address.
        wpi::SmallString<16> ip;
        in_addr addr;
        addr.s_addr = ipAddress;
        wpi::uv::AddrToName(addr, &ip);
        DEBUG("Trying to establish SSH connection to {}.", ip.str());
        try {
          SshSession session{ip.str(), kPort, kUsername, kPassword, m_logger};
          session.Open();
          DEBUG("SSH connection to {} was successful.", ip.str());

          SUCCESS("roboRIO Connected!");

          try {
            session.Execute(fmt::format(
                "for i in 1 2 3 4 5  ;  do ` echo 255 > "
                "/sys/class/leds/nilrt:wifi:primary/brightness; sleep 0.5; "
                "echo 0 > /sys/class/leds/nilrt:wifi:primary/brightness; sleep "
                "0.5 ` ; done"));
          } catch (const SshSession::SshException& e) {
            ERROR("An exception occurred: {}", e.what());
            throw e;
          }
        } catch (const SshSession::SshException& e) {
          DEBUG("SSH connection to {} failed with {}.", ip.str(), e.what());
          throw e;
        }
        return 0;
      });

  s_outstanding[macAddress] = std::move(future);
  return true;
}

bool DeploySession::DisableWebServer(const std::string& macAddress,
                                     unsigned int ipAddress) {
  auto itr = s_outstanding.find(macAddress);
  if (itr != s_outstanding.end()) {
    return false;
  }

  std::future<int> future =
      std::async(std::launch::async, [this, ipAddress, mac = macAddress]() {
        //  Convert to IP address.
        wpi::SmallString<16> ip;
        in_addr addr;
        addr.s_addr = ipAddress;
        wpi::uv::AddrToName(addr, &ip);
        DEBUG("Trying to establish SSH connection to {}.", ip.str());
        try {
          SshSession session{ip.str(), kPort, kUsername, kPassword, m_logger};
          session.Open();
          DEBUG("SSH connection to {} was successful.", ip.str());

          SUCCESS("roboRIO Connected!");

          try {
            session.Execute(
                "/bin/bash -c \"/etc/init.d/systemWebServer stop\"");
            session.Execute(
                "/bin/bash -c \"update-rc.d -f systemWebServer remove\"");
            session.Execute("/bin/bash -c \"sync\"");
          } catch (const SshSession::SshException& e) {
            ERROR("An exception occurred: {}", e.what());
            throw e;
          }
        } catch (const SshSession::SshException& e) {
          DEBUG("SSH connection to {} failed with {}.", ip.str(), e.what());
          throw e;
        }
        return 0;
      });

  s_outstanding[macAddress] = std::move(future);
  return true;
}

bool DeploySession::EnableWebServer(const std::string& macAddress,
                                    unsigned int ipAddress) {
  auto itr = s_outstanding.find(macAddress);
  if (itr != s_outstanding.end()) {
    return false;
  }

  std::future<int> future =
      std::async(std::launch::async, [this, ipAddress, mac = macAddress]() {
        //  Convert to IP address.
        wpi::SmallString<16> ip;
        in_addr addr;
        addr.s_addr = ipAddress;
        wpi::uv::AddrToName(addr, &ip);
        DEBUG("Trying to establish SSH connection to {}.", ip.str());
        try {
          SshSession session{ip.str(), kPort, kUsername, kPassword, m_logger};
          session.Open();
          DEBUG("SSH connection to {} was successful.", ip.str());

          SUCCESS("roboRIO Connected!");

          try {
            session.Execute(
                "/bin/bash -c \"update-rc.d -f systemWebServer defaults\"");
            session.Execute(
                "/bin/bash -c \"/etc/init.d/systemWebServer start\"");
            session.Execute("/bin/bash -c \"sync\"");
          } catch (const SshSession::SshException& e) {
            ERROR("An exception occurred: {}", e.what());
            throw e;
          }
        } catch (const SshSession::SshException& e) {
          DEBUG("SSH connection to {} failed with {}.", ip.str(), e.what());
          throw e;
        }
        return 0;
      });

  s_outstanding[macAddress] = std::move(future);
  return true;
}

bool DeploySession::GetStatus(const std::string& macAddress,
                              unsigned int ipAddress) {
  auto itr = s_outstandingStatus.find(macAddress);
  if (itr != s_outstandingStatus.end()) {
    return false;
  }

  std::future<DeviceStatus> future =
      std::async(std::launch::async, [this, ipAddress, mac = macAddress]() {
        //  Convert to IP address.
        wpi::SmallString<16> ip;
        in_addr addr;
        addr.s_addr = ipAddress;
        wpi::uv::AddrToName(addr, &ip);
        DEBUG("Trying to establish SSH connection to {}.", ip.str());
        DeviceStatus status;
        try {
          SshSession session{ip.str(), kPort, kUsername, kPassword, m_logger};
          session.Open();
          DEBUG("SSH connection to {} was successful.", ip.str());

          SUCCESS("roboRIO Connected!");

          try {
            int exitStatus = 0;
            session.ExecuteResult(
                "start-stop-daemon --status -x "
                "/usr/local/natinst/share/NIWebServer/SystemWebServer",
                &exitStatus);
            status.webServerEnabled = exitStatus == 0;
            auto serialNumber = session.ExecuteResult(
                "/sbin/fw_printenv -n serial#", &exitStatus);
            if (exitStatus == 0) {
              status.serialNumber = wpi::trim(serialNumber);
            }
            auto image = session.ExecuteResult(
                "/usr/local/natinst/bin/nirtcfg --file "
                "/etc/natinst/share/scs_imagemetadata.ini --get "
                "section=ImageMetadata,token=IMAGEVERSION,value=UNKNOWN",
                &exitStatus);
            if (exitStatus == 0) {
              status.image = wpi::trim(image);
            }
          } catch (const SshSession::SshException& e) {
            ERROR("An exception occurred: {}", e.what());
            throw e;
          }
        } catch (const SshSession::SshException& e) {
          DEBUG("SSH connection to {} failed with {}.", ip.str(), e.what());
          throw e;
        }
        return status;
      });

  s_outstandingStatus[macAddress] = std::move(future);
  return true;
}
