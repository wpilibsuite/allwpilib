// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_LOG_H_
#define CSCORE_LOG_H_

#include <string_view>

#include <wpi/Logger.h>

namespace cs {

void NamedLogV(wpi::Logger& logger, unsigned int level, const char* file,
               unsigned int line, std::string_view name,
               fmt::string_view format, fmt::format_args args);

template <typename S, typename... Args>
inline void NamedLog(wpi::Logger& logger, unsigned int level, const char* file,
                     unsigned int line, std::string_view name, const S& format,
                     Args&&... args) {
  if (logger.HasLogger() && level >= logger.min_level()) {
    NamedLogV(logger, level, file, line, name, format,
              fmt::make_format_args(args...));
  }
}

}  // namespace cs

#define LOG(level, format, ...) \
  WPI_LOG(m_logger, level, format __VA_OPT__(, ) __VA_ARGS__)

#undef ERROR
#define ERROR(format, ...) \
  WPI_ERROR(m_logger, format __VA_OPT__(, ) __VA_ARGS__)
#define WARNING(format, ...) \
  WPI_WARNING(m_logger, format __VA_OPT__(, ) __VA_ARGS__)
#define INFO(format, ...) WPI_INFO(m_logger, format __VA_OPT__(, ) __VA_ARGS__)

#define DEBUG0(format, ...) \
  WPI_DEBUG(m_logger, format __VA_OPT__(, ) __VA_ARGS__)
#define DEBUG1(format, ...) \
  WPI_DEBUG1(m_logger, format __VA_OPT__(, ) __VA_ARGS__)
#define DEBUG2(format, ...) \
  WPI_DEBUG2(m_logger, format __VA_OPT__(, ) __VA_ARGS__)
#define DEBUG3(format, ...) \
  WPI_DEBUG3(m_logger, format __VA_OPT__(, ) __VA_ARGS__)
#define DEBUG4(format, ...) \
  WPI_DEBUG4(m_logger, format __VA_OPT__(, ) __VA_ARGS__)

#define SLOG(level, format, ...)                           \
  NamedLog(m_logger, level, __FILE__, __LINE__, GetName(), \
           format __VA_OPT__(, ) __VA_ARGS__)

#define SERROR(format, ...) \
  SLOG(::wpi::WPI_LOG_ERROR, format __VA_OPT__(, ) __VA_ARGS__)
#define SWARNING(format, ...) \
  SLOG(::wpi::WPI_LOG_WARNING, format __VA_OPT__(, ) __VA_ARGS__)
#define SINFO(format, ...) \
  SLOG(::wpi::WPI_LOG_INFO, format __VA_OPT__(, ) __VA_ARGS__)

#ifdef NDEBUG
#define SDEBUG(format, ...) \
  do {                      \
  } while (0)
#define SDEBUG1(format, ...) \
  do {                       \
  } while (0)
#define SDEBUG2(format, ...) \
  do {                       \
  } while (0)
#define SDEBUG3(format, ...) \
  do {                       \
  } while (0)
#define SDEBUG4(format, ...) \
  do {                       \
  } while (0)
#else
#define SDEBUG(format, ...) \
  SLOG(::wpi::WPI_LOG_DEBUG, format __VA_OPT__(, ) __VA_ARGS__)
#define SDEBUG1(format, ...) \
  SLOG(::wpi::WPI_LOG_DEBUG1, format __VA_OPT__(, ) __VA_ARGS__)
#define SDEBUG2(format, ...) \
  SLOG(::wpi::WPI_LOG_DEBUG2, format __VA_OPT__(, ) __VA_ARGS__)
#define SDEBUG3(format, ...) \
  SLOG(::wpi::WPI_LOG_DEBUG3, format __VA_OPT__(, ) __VA_ARGS__)
#define SDEBUG4(format, ...) \
  SLOG(::wpi::WPI_LOG_DEBUG4, format __VA_OPT__(, ) __VA_ARGS__)
#endif

#endif  // CSCORE_LOG_H_
