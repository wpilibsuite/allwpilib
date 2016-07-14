/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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
  std::ostringstream& Get(TLogLevel level = logINFO);

 public:
  static TLogLevel& ReportingLevel();
  static std::string ToString(TLogLevel level);
  static TLogLevel FromString(const std::string& level);

 protected:
  std::ostringstream os;

 private:
  Log(const Log&);
  Log& operator=(const Log&);
};

inline Log::Log() {}

inline std::ostringstream& Log::Get(TLogLevel level) {
  os << "- " << NowTime();
  os << " " << ToString(level) << ": ";
  os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
  return os;
}

inline Log::~Log() {
  os << std::endl;
  std::cerr << os.str();
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
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2);

  using namespace std::chrono;
  auto now = system_clock::now().time_since_epoch();

  ss << duration_cast<hours>(now).count() % 24 << ":"
     << duration_cast<minutes>(now).count() % 60 << ":"
     << duration_cast<seconds>(now).count() % 60 << "."
     << duration_cast<milliseconds>(now).count() % 1000;

  return ss.str();
}
