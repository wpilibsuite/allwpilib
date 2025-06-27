// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "DataLogManager.h"

#include <algorithm>
#include <ctime>
#include <random>
#include <string>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/DataLogBackgroundWriter.h>
#include <wpi/FileLogger.h>
#include <wpi/SafeThread.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/print.h>

#ifdef __FRC_ROBORIO__
#include <FRC_NetworkCommunication/FRCComm.h>
#include <FRC_NetworkCommunication/LoadOut.h>
#endif

using namespace wpi;

/** Shims to keep the code as similar as possible to wpilibc */
namespace {

namespace warn {
static constexpr int Warning = 16;
}  // namespace warn

namespace frc {
void ReportErrorV(int32_t status, const char* fileName, int lineNumber,
                  const char* funcName, fmt::string_view format,
                  fmt::format_args args) {
#ifdef __FRC_ROBORIO__
  if (status == 0) {
    return;
  }
  fmt::memory_buffer out;
  fmt::format_to(fmt::appender{out}, "Warning: ");
  fmt::vformat_to(fmt::appender{out}, format, args);
  out.push_back('\0');
  FRC_NetworkCommunication_sendError(status < 0, status, 0, out.data(),
                                     "DataLogManager", "");
#endif
}

template <typename... Args>
inline void ReportError(int32_t status, const char* fileName, int lineNumber,
                        const char* funcName, fmt::string_view format,
                        Args&&... args) {
  ReportErrorV(status, fileName, lineNumber, funcName, format,
               fmt::make_format_args(args...));
}
}  // namespace frc

#define FRC_ReportError(status, format, ...)                       \
  do {                                                             \
    if ((status) != 0) {                                           \
      ::frc::ReportError(status, __FILE__, __LINE__, __FUNCTION__, \
                         format __VA_OPT__(, ) __VA_ARGS__);       \
    }                                                              \
  } while (0)

namespace RobotController {
inline bool IsSystemTimeValid() {
#ifdef __FRC_ROBORIO__
  uint8_t timeWasSet = 0;
  FRC_NetworkCommunication_getTimeWasSet(&timeWasSet);
  return timeWasSet != 0;
#else
  return true;
#endif
}
}  // namespace RobotController

namespace filesystem {
inline std::string GetOperatingDirectory() {
#ifdef __FRC_ROBORIO__
  return "/home/lvuser";
#else
  return fs::current_path().string();
#endif
}
}  // namespace filesystem

namespace DriverStation {
#ifdef __FRC_ROBORIO__
using MatchType = MatchType_t;
constexpr int kNone = kMatchType_none;
constexpr int kPractice = kMatchType_practice;
constexpr int kQualification = kMatchType_qualification;
constexpr int kElimination = kMatchType_elimination;
char gEventName[128];
MatchType_t gMatchType;
uint16_t gMatchNumber;
uint8_t gReplayNumber;
uint8_t gGameSpecificMessage[16];
uint16_t gGameSpecificMessageSize;
#else
enum MatchType { kNone, kPractice, kQualification, kElimination };
#endif

inline void UpdateMatchInfo() {
#ifdef __FRC_ROBORIO__
  gGameSpecificMessageSize = sizeof(gGameSpecificMessage);
  FRC_NetworkCommunication_getMatchInfo(gEventName, &gMatchType, &gMatchNumber,
                                        &gReplayNumber, gGameSpecificMessage,
                                        &gGameSpecificMessageSize);
#endif
}

inline MatchType GetMatchType() {
#ifdef __FRC_ROBORIO__
  return gMatchType;
#else
  return kNone;
#endif
}

inline std::string_view GetEventName() {
#ifdef __FRC_ROBORIO__
  return gEventName;
#else
  return "";
#endif
}

inline uint16_t GetMatchNumber() {
#ifdef __FRC_ROBORIO__
  return gMatchNumber;
#else
  return 0;
#endif
}

inline bool IsDSAttached() {
#ifdef __FRC_ROBORIO__
  struct ControlWord_t cw;
  FRC_NetworkCommunication_getControlWord(&cw);
  return cw.dsAttached;
#else
  return true;
#endif
}

inline bool IsFMSAttached() {
#ifdef __FRC_ROBORIO__
  struct ControlWord_t cw;
  FRC_NetworkCommunication_getControlWord(&cw);
  return cw.fmsAttached;
#else
  return false;
#endif
}

WPI_EventHandle gNewDataEvent;

inline void ProvideRefreshedDataEventHandle(WPI_EventHandle event) {
  gNewDataEvent = event;
}

inline void RemoveRefreshedDataEventHandle(WPI_EventHandle event) {}

}  // namespace DriverStation

#ifdef __FRC_ROBORIO__
static constexpr int kRoboRIO = 0;
namespace RobotBase {
inline int GetRuntimeType() {
  nLoadOut::tTargetClass targetClass = nLoadOut::getTargetClass();
  if (targetClass == nLoadOut::kTargetClass_RoboRIO2) {
    return 1;
  } else {
    return 0;
  }
}
}  // namespace RobotBase
#endif

struct Thread final : public wpi::SafeThread {
  Thread(std::string_view dir, std::string_view filename, double period);
  ~Thread() override;

  void Main() final;

  void StartNTLog();
  void StopNTLog();
  void StartConsoleLog();
  void StopConsoleLog();

  std::string m_logDir;
  bool m_filenameOverride;
  wpi::log::DataLogBackgroundWriter m_log;
  bool m_ntLoggerEnabled = false;
  NT_DataLogger m_ntEntryLogger = 0;
  NT_ConnectionDataLogger m_ntConnLogger = 0;
  bool m_consoleLoggerEnabled = false;
  wpi::FileLogger m_consoleLogger;
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
  std::error_code ec;
  auto s = fs::status("/u", ec);
  if (!ec && fs::is_directory(s) &&
      (s.permissions() & fs::perms::others_write) != fs::perms::none) {
    fs::create_directory("/u/logs", ec);
    return "/u/logs";
  }
  if (RobotBase::GetRuntimeType() == kRoboRIO) {
    FRC_ReportError(warn::Warning,
                    "DataLogManager: Logging to RoboRIO 1 internal storage is "
                    "not recommended! Plug in a FAT32 formatted flash drive!");
  }
  fs::create_directory("/home/lvuser/logs", ec);
  return "/home/lvuser/logs";
#else
  std::string logDir = filesystem::GetOperatingDirectory() + "/logs";
  std::error_code ec;
  fs::create_directory(logDir, ec);
  return logDir;
#endif
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

Thread::~Thread() {
  StopNTLog();
}

void Thread::Main() {
  // based on free disk space, scan for "old" FRC_*.wpilog files and remove
  {
    std::error_code ec;
    uintmax_t freeSpace;
    auto freeSpaceInfo = fs::space(m_logDir, ec);
    if (!ec) {
      freeSpace = freeSpaceInfo.available;
    } else {
      freeSpace = UINTMAX_MAX;
    }
    if (freeSpace < kFreeSpaceThreshold) {
      // Delete oldest FRC_*.wpilog files (ignore FRC_TBD_*.wpilog as we just
      // created one)
      std::vector<fs::directory_entry> entries;
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
          FRC_ReportError(warn::Warning, "DataLogManager: Deleted {}",
                          entry.path().string());
          freeSpace += size;
          if (freeSpace >= kFreeSpaceThreshold) {
            break;
          }
        } else {
          wpi::print(stderr, "DataLogManager: could not delete {}\n",
                     entry.path().string());
        }
      }
    } else if (freeSpace < 2 * kFreeSpaceThreshold) {
      FRC_ReportError(
          warn::Warning,
          "DataLogManager: Log storage device has {} MB of free space "
          "remaining! Logs will get deleted below {} MB of free space. "
          "Consider deleting logs off the storage device.",
          freeSpace / 1000000, kFreeSpaceThreshold / 1000000);
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
        if (RobotController::IsSystemTimeValid()) {
          std::time_t now = std::time(nullptr);
          auto tm = std::gmtime(&now);
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
      if (fmsAttachCount > 250) {  // 5 seconds
        // match info comes through TCP, so we need to double-check we've
        // actually received it
        DriverStation::UpdateMatchInfo();
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
      if (RobotController::IsSystemTimeValid()) {
        sysTimeEntry.Append(wpi::GetSystemTime(), wpi::Now());
      }
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

void Thread::StartConsoleLog() {
  if (!m_consoleLoggerEnabled) {
    m_consoleLoggerEnabled = true;
    m_consoleLogger = {"/home/lvuser/FRC_UserProgram.log", m_log, "output"};
  }
}

void Thread::StopConsoleLog() {
  if (m_consoleLoggerEnabled) {
    m_consoleLoggerEnabled = false;
    m_consoleLogger = {};
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
        wpi::print(stderr, "DataLogManager: could not delete {}\n",
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
  if (!instance.owner) {
    instance.owner.Start(MakeLogDir(dir), filename, period);
  }
  return instance;
}

void DataLogManager::Start(std::string_view dir, std::string_view filename,
                           double period) {
  GetInstance(dir, filename, period);
}

void DataLogManager::Stop() {
  auto& inst = GetInstance();
  inst.owner.GetThread()->m_log.Stop();
  inst.owner.Join();
}

void DataLogManager::Log(std::string_view message) {
  GetInstance().owner.GetThread()->m_messageLog.Append(message);
  wpi::print("{}\n", message);
}

wpi::log::DataLog& DataLogManager::GetLog() {
  return GetInstance().owner.GetThread()->m_log;
}

std::string_view DataLogManager::GetLogDir() {
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

void DataLogManager::LogConsoleOutput(bool enabled) {
  if (auto thr = GetInstance().owner.GetThread()) {
    if (enabled) {
      thr->StartConsoleLog();
    } else if (!enabled) {
      thr->StopConsoleLog();
    }
  }
}

void DataLogManager::SignalNewDSDataOccur() {
  wpi::SetSignalObject(DriverStation::gNewDataEvent);
}

extern "C" {

void DLM_Start(const struct WPI_String* dir, const struct WPI_String* filename,
               double period) {
  DataLogManager::Start(wpi::to_string_view(dir), wpi::to_string_view(filename),
                        period);
}

void DLM_Stop(void) {
  DataLogManager::Stop();
}

void DLM_Log(const struct WPI_String* message) {
  DataLogManager::Log(wpi::to_string_view(message));
}

WPI_DataLog* DLM_GetLog(void) {
  return reinterpret_cast<WPI_DataLog*>(&DataLogManager::GetLog());
}

void DLM_GetLogDir(struct WPI_String* value) {
  auto logDir = DataLogManager::GetLogDir();
  char* write = WPI_AllocateString(value, logDir.size());
  std::memcpy(write, logDir.data(), logDir.size());
}

void DLM_LogNetworkTables(int enabled) {
  DataLogManager::LogNetworkTables(enabled);
}

void DLM_LogConsoleOutput(int enabled) {
  DataLogManager::LogConsoleOutput(enabled);
}

void DLM_SignalNewDSDataOccur(void) {
  DataLogManager::SignalNewDSDataOccur();
}

}  // extern "C"
