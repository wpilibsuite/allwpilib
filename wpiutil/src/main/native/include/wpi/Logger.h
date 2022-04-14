// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_LOGGER_H_
#define WPIUTIL_WPI_LOGGER_H_

#include <functional>
#include <utility>

#include "fmt/format.h"

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
  explicit Logger(LogFunc func) : m_func(std::move(func)) {}
  Logger(LogFunc func, unsigned int min_level)
      : m_func(std::move(func)), m_min_level(min_level) {}

  void SetLogger(LogFunc func) { m_func = func; }

  void set_min_level(unsigned int level) { m_min_level = level; }
  unsigned int min_level() const { return m_min_level; }

  void DoLog(unsigned int level, const char* file, unsigned int line,
             const char* msg);

  void LogV(unsigned int level, const char* file, unsigned int line,
            fmt::string_view format, fmt::format_args args);

  template <typename S, typename... Args>
  void Log(unsigned int level, const char* file, unsigned int line,
           const S& format, Args&&... args) {
    if (m_func && level >= m_min_level) {
      LogV(level, file, line, format,
           fmt::make_args_checked<Args...>(format, args...));
    }
  }

  bool HasLogger() const { return m_func != nullptr; }

 private:
  LogFunc m_func;
  unsigned int m_min_level = 20;
};

#define WPI_LOG(logger_inst, level, format, ...) \
  logger_inst.Log(level, __FILE__, __LINE__, FMT_STRING(format), __VA_ARGS__)

#define WPI_ERROR(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_ERROR, format, __VA_ARGS__)
#define WPI_WARNING(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_WARNING, format, __VA_ARGS__)
#define WPI_INFO(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_INFO, format, __VA_ARGS__)

#ifdef NDEBUG
#define WPI_DEBUG(inst, format, ...) \
  do {                               \
  } while (0)
#define WPI_DEBUG1(inst, format, ...) \
  do {                                \
  } while (0)
#define WPI_DEBUG2(inst, format, ...) \
  do {                                \
  } while (0)
#define WPI_DEBUG3(inst, format, ...) \
  do {                                \
  } while (0)
#define WPI_DEBUG4(inst, format, ...) \
  do {                                \
  } while (0)
#else
#define WPI_DEBUG(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG, format, __VA_ARGS__)
#define WPI_DEBUG1(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG1, format, __VA_ARGS__)
#define WPI_DEBUG2(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG2, format, __VA_ARGS__)
#define WPI_DEBUG3(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG3, format, __VA_ARGS__)
#define WPI_DEBUG4(inst, format, ...) \
  WPI_LOG(inst, ::wpi::WPI_LOG_DEBUG4, format, __VA_ARGS__)
#endif

}  // namespace wpi

#endif  // WPIUTIL_WPI_LOGGER_H_
