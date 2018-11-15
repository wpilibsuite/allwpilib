/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_LOGGER_H_
#define WPIUTIL_WPI_LOGGER_H_

#include <functional>

#include "wpi/SmallString.h"
#include "wpi/raw_ostream.h"

namespace wpi {

enum LogLevel {
  WPI_LOG_CRITICAL = 50,
  WPI_LOG_ERROR = 40,
  WPI_LOG_WARNING = 30,
  WPI_LOG_INFO = 20,
  WPI_LOG_DEBUG = 10,
  WPI_LOG_DEBUG1 = 9,
  WPI_LOG_DEBUG2 = 8,
  WPI_LOG_DEBUG3 = 7,
  WPI_LOG_DEBUG4 = 6
};

class Logger {
 public:
  using LogFunc = std::function<void(unsigned int level, const char* file,
                                     unsigned int line, const char* msg)>;

  Logger() = default;
  explicit Logger(const LogFunc& func) : m_func(func) {}
  Logger(const LogFunc& func, unsigned int min_level)
      : m_func(func), m_min_level(min_level) {}

  void SetLogger(LogFunc func) { m_func = func; }

  void set_min_level(unsigned int level) { m_min_level = level; }
  unsigned int min_level() const { return m_min_level; }

  void Log(unsigned int level, const char* file, unsigned int line,
           const char* msg) {
    if (!m_func || level < m_min_level) return;
    m_func(level, file, line, msg);
  }

  bool HasLogger() const { return m_func != nullptr; }

 private:
  LogFunc m_func;
  unsigned int m_min_level = 20;
};

#define WPI_LOG(logger_inst, level, x)                                 \
  do {                                                                 \
    ::wpi::Logger& WPI_logger_ = logger_inst;                          \
    if (WPI_logger_.min_level() <= static_cast<unsigned int>(level) && \
        WPI_logger_.HasLogger()) {                                     \
      ::wpi::SmallString<128> log_buf_;                                \
      ::wpi::raw_svector_ostream log_os_{log_buf_};                    \
      log_os_ << x;                                                    \
      WPI_logger_.Log(level, __FILE__, __LINE__, log_buf_.c_str());    \
    }                                                                  \
  } while (0)

#define WPI_ERROR(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_ERROR, x)
#define WPI_WARNING(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_WARNING, x)
#define WPI_INFO(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_INFO, x)

#ifdef NDEBUG
#define WPI_DEBUG(inst, x) \
  do {                     \
  } while (0)
#define WPI_DEBUG1(inst, x) \
  do {                      \
  } while (0)
#define WPI_DEBUG2(inst, x) \
  do {                      \
  } while (0)
#define WPI_DEBUG3(inst, x) \
  do {                      \
  } while (0)
#define WPI_DEBUG4(inst, x) \
  do {                      \
  } while (0)
#else
#define WPI_DEBUG(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG, x)
#define WPI_DEBUG1(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG1, x)
#define WPI_DEBUG2(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG2, x)
#define WPI_DEBUG3(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG3, x)
#define WPI_DEBUG4(inst, x) WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG4, x)
#endif

}  // namespace wpi

#endif  // WPIUTIL_WPI_LOGGER_H_
