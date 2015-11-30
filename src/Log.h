/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_LOG_H_
#define NT_LOG_H_

#include <functional>
#include <sstream>
#include <string>

#include "atomic_static.h"
#include "ntcore_c.h"

namespace nt {

class Logger {
 public:
  static Logger& GetInstance() {
    ATOMIC_STATIC(Logger, instance);
    return instance;
  }
  ~Logger();

  typedef std::function<void(unsigned int level, const char* file,
                             unsigned int line, const char* msg)> LogFunc;

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
  Logger();

  LogFunc m_func;
  unsigned int m_min_level = 20;

  ATOMIC_STATIC_DECL(Logger)
};

#define LOG(level, x)                                           \
  do {                                                          \
    nt::Logger& logger = nt::Logger::GetInstance();             \
    if (logger.min_level() <= level && logger.HasLogger()) {    \
      std::ostringstream oss;                                   \
      oss << x;                                                 \
      logger.Log(level, __FILE__, __LINE__, oss.str().c_str()); \
    }                                                           \
  } while (0)

#undef ERROR
#define ERROR(x) LOG(NT_LOG_ERROR, x)
#define WARNING(x) LOG(NT_LOG_WARNING, x)
#define INFO(x) LOG(NT_LOG_INFO, x)

#ifdef NDEBUG
#define DEBUG(x) do {} while (0)
#define DEBUG1(x) do {} while (0)
#define DEBUG2(x) do {} while (0)
#define DEBUG3(x) do {} while (0)
#define DEBUG4(x) do {} while (0)
#else
#define DEBUG(x) LOG(NT_LOG_DEBUG, x)
#define DEBUG1(x) LOG(NT_LOG_DEBUG1, x)
#define DEBUG2(x) LOG(NT_LOG_DEBUG2, x)
#define DEBUG3(x) LOG(NT_LOG_DEBUG3, x)
#define DEBUG4(x) LOG(NT_LOG_DEBUG4, x)
#endif

} // namespace nt

#endif  // NT_LOG_H_
