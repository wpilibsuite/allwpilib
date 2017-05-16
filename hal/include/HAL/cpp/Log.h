/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "llvm/SmallString.h"
#include "llvm/raw_ostream.h"

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
  llvm::raw_ostream& Get(TLogLevel level = logINFO);

 public:
  static TLogLevel& ReportingLevel();
  static std::string ToString(TLogLevel level);
  static TLogLevel FromString(const std::string& level);

 protected:
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream oss{buf};

 private:
  Log(const Log&);
  Log& operator=(const Log&);
};

inline Log::Log() {}

inline llvm::raw_ostream& Log::Get(TLogLevel level) {
  oss << "- " << NowTime();
  oss << " " << ToString(level) << ": ";
  oss << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
  return oss;
}

inline Log::~Log() {
  oss << "\n";
  std::cerr << oss.str();
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

typedef Log FILELog;

#define FILE_LOG(level)                  \
  if (level > FILELog::ReportingLevel()) \
    ;                                    \
  else                                   \
  Log().Get(level)

inline std::string NowTime() {
  llvm::SmallString<128> buf;
  llvm::raw_svector_ostream oss(buf);

  using namespace std::chrono;
  auto now = system_clock::now().time_since_epoch();

  // Hours
  auto count = duration_cast<hours>(now).count() % 24;
  if (count < 10) oss << "0";
  oss << count << ":";

  // Minutes
  count = duration_cast<minutes>(now).count() % 60;
  if (count < 10) oss << "0";
  oss << count << ":";

  // Seconds
  count = duration_cast<seconds>(now).count() % 60;
  if (count < 10) oss << "0";
  oss << count << ".";

  // Milliseconds
  oss << duration_cast<milliseconds>(now).count() % 1000;

  return oss.str();
}
