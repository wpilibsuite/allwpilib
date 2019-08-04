/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <string>

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

inline std::string NowTime();

enum TLogLevel {
  logNONE,
  logERROR,
  logWARNING,
  logINFO,
  logDEBUG,
  logDEBUG1,
  logDEBUG2,
  logDEBUG3,
  logDEBUG4
};

class Log {
 public:
  Log();
  virtual ~Log();
  wpi::raw_ostream& Get(TLogLevel level = logINFO);

 public:
  static TLogLevel& ReportingLevel();
  static std::string ToString(TLogLevel level);
  static TLogLevel FromString(const std::string& level);

 protected:
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream oss{buf};

 private:
  Log(const Log&);
  Log& operator=(const Log&);
};

inline Log::Log() {}

inline wpi::raw_ostream& Log::Get(TLogLevel level) {
  oss << "- " << NowTime();
  oss << " " << ToString(level) << ": ";
  if (level > logDEBUG) {
    oss << std::string(level - logDEBUG, '\t');
  }
  return oss;
}

inline Log::~Log() {
  oss << "\n";
  wpi::errs() << oss.str();
}

inline TLogLevel& Log::ReportingLevel() {
  static TLogLevel reportingLevel = logDEBUG4;
  return reportingLevel;
}

inline std::string Log::ToString(TLogLevel level) {
  static const char* const buffer[] = {"NONE",   "ERROR",  "WARNING",
                                       "INFO",   "DEBUG",  "DEBUG1",
                                       "DEBUG2", "DEBUG3", "DEBUG4"};
  return buffer[level];
}

inline TLogLevel Log::FromString(const std::string& level) {
  if (level == "DEBUG4") return logDEBUG4;
  if (level == "DEBUG3") return logDEBUG3;
  if (level == "DEBUG2") return logDEBUG2;
  if (level == "DEBUG1") return logDEBUG1;
  if (level == "DEBUG") return logDEBUG;
  if (level == "INFO") return logINFO;
  if (level == "WARNING") return logWARNING;
  if (level == "ERROR") return logERROR;
  if (level == "NONE") return logNONE;
  Log().Get(logWARNING) << "Unknown logging level '" << level
                        << "'. Using INFO level as default.";
  return logINFO;
}

using FILELog = Log;  // NOLINT

#define FILE_LOG(level)                  \
  if (level > FILELog::ReportingLevel()) \
    ;                                    \
  else                                   \
    Log().Get(level)

inline std::string NowTime() {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream oss(buf);

  using std::chrono::duration_cast;

  auto now = std::chrono::system_clock::now().time_since_epoch();

  // Hours
  auto count = duration_cast<std::chrono::hours>(now).count() % 24;
  if (count < 10) oss << "0";
  oss << count << ":";

  // Minutes
  count = duration_cast<std::chrono::minutes>(now).count() % 60;
  if (count < 10) oss << "0";
  oss << count << ":";

  // Seconds
  count = duration_cast<std::chrono::seconds>(now).count() % 60;
  if (count < 10) oss << "0";
  oss << count << ".";

  // Milliseconds
  oss << duration_cast<std::chrono::milliseconds>(now).count() % 1000;

  return oss.str();
}
