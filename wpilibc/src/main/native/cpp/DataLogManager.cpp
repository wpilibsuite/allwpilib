// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DataLogManager.h"

#include <algorithm>
#include <ctime>
#include <random>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/DataLog.h>
#include <wpi/SafeThread.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/timestamp.h>

#include "frc/DriverStation.h"
#include "frc/Filesystem.h"

using namespace frc;

namespace {

struct Thread final : public wpi::SafeThread {
  Thread(std::string_view dir, std::string_view filename, double period);

  void Main() final;

  void StartNTLog();
  void StopNTLog();

  std::string m_logDir;
  bool m_filenameOverride;
  wpi::log::DataLog m_log;
  bool m_ntLoggerEnabled = false;
  NT_DataLogger m_ntEntryLogger = 0;
  NT_ConnectionDataLogger m_ntConnLogger = 0;
  wpi::log::StringLogEntry m_messageLog;
};

struct Instance {
  Instance(std::string_view dir, std::string_view filename, double period);
  wpi::SafeThreadOwner<Thread> owner;
};

}  // namespace

// if less than this much free space, delete log files until there is this much
// free space OR there are this many files remaining.
static constexpr uintmax_t kFreeSpaceThreshold = 50000000;
static constexpr int kFileCountThreshold = 10;

static std::string MakeLogDir(std::string_view dir) {
  if (!dir.empty()) {
    return std::string{dir};
  }
#ifdef __FRC_ROBORIO__
  // prefer a mounted USB drive if one is accessible
  constexpr std::string_view usbDir{"/u"};
  std::error_code ec;
  auto s = fs::status(usbDir, ec);
  if (!ec && fs::is_directory(s) &&
      (s.permissions() & fs::perms::others_write) != fs::perms::none) {
    return std::string{usbDir};
  }
#endif
  return frc::filesystem::GetOperatingDirectory();
}

static std::string MakeLogFilename(std::string_view filenameOverride) {
  if (!filenameOverride.empty()) {
    return std::string{filenameOverride};
  }
  static std::random_device dev;
  static std::mt19937 rng(dev());
  std::uniform_int_distribution<int> dist(0, 15);
  const char* v = "0123456789abcdef";
  std::string filename = "FRC_TBD_";
  for (int i = 0; i < 16; i++) {
    filename += v[dist(rng)];
  }
  filename += ".wpilog";
  return filename;
}

Thread::Thread(std::string_view dir, std::string_view filename, double period)
    : m_logDir{dir},
      m_filenameOverride{!filename.empty()},
      m_log{dir, MakeLogFilename(filename), period},
      m_messageLog{m_log, "messages"} {
  StartNTLog();
}

void Thread::Main() {
  // based on free disk space, scan for "old" FRC_*.wpilog files and remove
  {
    uintmax_t freeSpace = fs::space(m_logDir).free;
    if (freeSpace < kFreeSpaceThreshold) {
      // Delete oldest FRC_*.wpilog files (ignore FRC_TBD_*.wpilog as we just
      // created one)
      std::vector<fs::directory_entry> entries;
      std::error_code ec;
      for (auto&& entry : fs::directory_iterator{m_logDir, ec}) {
        auto stem = entry.path().stem().string();
        if (wpi::starts_with(stem, "FRC_") &&
            entry.path().extension() == ".wpilog" &&
            !wpi::starts_with(stem, "FRC_TBD_")) {
          entries.emplace_back(entry);
        }
      }
      std::sort(entries.begin(), entries.end(),
                [](const auto& a, const auto& b) {
                  return a.last_write_time() < b.last_write_time();
                });

      int count = entries.size();
      for (auto&& entry : entries) {
        --count;
        if (count < kFileCountThreshold) {
          break;
        }
        auto size = entry.file_size();
        if (fs::remove(entry.path(), ec)) {
          freeSpace += size;
          if (freeSpace >= kFreeSpaceThreshold) {
            break;
          }
        } else {
          fmt::print(stderr, "DataLogManager: could not delete {}\n",
                     entry.path().string());
        }
      }
    }
  }

  int timeoutCount = 0;
  bool paused = false;
  int dsAttachCount = 0;
  int fmsAttachCount = 0;
  bool dsRenamed = m_filenameOverride;
  bool fmsRenamed = m_filenameOverride;
  int sysTimeCount = 0;
  wpi::log::IntegerLogEntry sysTimeEntry{
      m_log, "systemTime",
      "{\"source\":\"DataLogManager\",\"format\":\"time_t_us\"}"};

  wpi::Event newDataEvent;
  DriverStation::ProvideRefreshedDataEventHandle(newDataEvent.GetHandle());

  for (;;) {
    bool timedOut = false;
    bool newData =
        wpi::WaitForObject(newDataEvent.GetHandle(), 0.25, &timedOut);
    if (!m_active) {
      break;
    }
    if (!newData) {
      ++timeoutCount;
      // pause logging after being disconnected for 10 seconds
      if (timeoutCount > 40 && !paused) {
        timeoutCount = 0;
        paused = true;
        m_log.Pause();
      }
      continue;
    }
    // when we connect to the DS, resume logging
    timeoutCount = 0;
    if (paused) {
      paused = false;
      m_log.Resume();
    }

    if (!dsRenamed) {
      // track DS attach
      if (DriverStation::IsDSAttached()) {
        ++dsAttachCount;
      } else {
        dsAttachCount = 0;
      }
      if (dsAttachCount > 50) {  // 1 second
        std::time_t now = std::time(nullptr);
        auto tm = std::gmtime(&now);
        if (tm->tm_year > 100) {
          // assume local clock is now synchronized to DS, so rename based on
          // local time
          m_log.SetFilename(fmt::format("FRC_{:%Y%m%d_%H%M%S}.wpilog", *tm));
          dsRenamed = true;
        } else {
          dsAttachCount = 0;  // wait a bit and try again
        }
      }
    }

    if (!fmsRenamed) {
      // track FMS attach
      if (DriverStation::IsFMSAttached()) {
        ++fmsAttachCount;
      } else {
        fmsAttachCount = 0;
      }
      if (fmsAttachCount > 100) {  // 2 seconds
        // match info comes through TCP, so we need to double-check we've
        // actually received it
        auto matchType = DriverStation::GetMatchType();
        if (matchType != DriverStation::kNone) {
          // rename per match info
          char matchTypeChar;
          switch (matchType) {
            case DriverStation::kPractice:
              matchTypeChar = 'P';
              break;
            case DriverStation::kQualification:
              matchTypeChar = 'Q';
              break;
            case DriverStation::kElimination:
              matchTypeChar = 'E';
              break;
            default:
              matchTypeChar = '_';
              break;
          }
          std::time_t now = std::time(nullptr);
          m_log.SetFilename(
              fmt::format("FRC_{:%Y%m%d_%H%M%S}_{}_{}{}.wpilog",
                          *std::gmtime(&now), DriverStation::GetEventName(),
                          matchTypeChar, DriverStation::GetMatchNumber()));
          fmsRenamed = true;
          dsRenamed = true;  // don't override FMS rename
        }
      }
    }

    // Write system time every ~5 seconds
    ++sysTimeCount;
    if (sysTimeCount >= 250) {
      sysTimeCount = 0;
      sysTimeEntry.Append(wpi::GetSystemTime(), wpi::Now());
    }
  }
  DriverStation::RemoveRefreshedDataEventHandle(newDataEvent.GetHandle());
}

void Thread::StartNTLog() {
  if (!m_ntLoggerEnabled) {
    m_ntLoggerEnabled = true;
    auto inst = nt::NetworkTableInstance::GetDefault();
    m_ntEntryLogger = inst.StartEntryDataLog(m_log, "", "NT:");
    m_ntConnLogger = inst.StartConnectionDataLog(m_log, "NTConnection");
  }
}

void Thread::StopNTLog() {
  if (m_ntLoggerEnabled) {
    m_ntLoggerEnabled = false;
    nt::NetworkTableInstance::StopEntryDataLog(m_ntEntryLogger);
    nt::NetworkTableInstance::StopConnectionDataLog(m_ntConnLogger);
  }
}

Instance::Instance(std::string_view dir, std::string_view filename,
                   double period) {
  // Delete all previously existing FRC_TBD_*.wpilog files. These only exist
  // when the robot never connects to the DS, so they are very unlikely to
  // have useful data and just clutter the filesystem.
  auto logDir = MakeLogDir(dir);
  std::error_code ec;
  for (auto&& entry : fs::directory_iterator{logDir, ec}) {
    if (wpi::starts_with(entry.path().stem().string(), "FRC_TBD_") &&
        entry.path().extension() == ".wpilog") {
      if (!fs::remove(entry, ec)) {
        fmt::print(stderr, "DataLogManager: could not delete {}\n",
                   entry.path().string());
      }
    }
  }

  owner.Start(logDir, filename, period);
}

static Instance& GetInstance(std::string_view dir = "",
                             std::string_view filename = "",
                             double period = 0.25) {
  static Instance instance(dir, filename, period);
  return instance;
}

void DataLogManager::Start(std::string_view dir, std::string_view filename,
                           double period) {
  GetInstance(dir, filename, period);
}

void DataLogManager::Log(std::string_view message) {
  GetInstance().owner.GetThread()->m_messageLog.Append(message);
  fmt::print("{}\n", message);
}

wpi::log::DataLog& DataLogManager::GetLog() {
  return GetInstance().owner.GetThread()->m_log;
}

std::string DataLogManager::GetLogDir() {
  return GetInstance().owner.GetThread()->m_logDir;
}

void DataLogManager::LogNetworkTables(bool enabled) {
  if (auto thr = GetInstance().owner.GetThread()) {
    if (enabled) {
      thr->StartNTLog();
    } else if (!enabled) {
      thr->StopNTLog();
    }
  }
}
