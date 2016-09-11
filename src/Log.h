/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_LOG_H_
#define CAMERASERVER_LOG_H_

#include "support/atomic_static.h"
#include "support/Logger.h"

namespace cs {

class Logger : public wpi::Logger {
 public:
  static Logger& GetInstance() {
    ATOMIC_STATIC(Logger, instance);
    return instance;
  }
  ~Logger();

 private:
  Logger();

  ATOMIC_STATIC_DECL(Logger)
};

#define LOG(level, x) WPI_LOG(cs::Logger::GetInstance(), level, x)

#undef ERROR
#define ERROR(x) WPI_ERROR(cs::Logger::GetInstance(), x)
#define WARNING(x) WPI_WARNING(cs::Logger::GetInstance(), x)
#define INFO(x) WPI_INFO(cs::Logger::GetInstance(), x)

#define DEBUG(x) WPI_DEBUG(cs::Logger::GetInstance(), x)
#define DEBUG1(x) WPI_DEBUG1(cs::Logger::GetInstance(), x)
#define DEBUG2(x) WPI_DEBUG2(cs::Logger::GetInstance(), x)
#define DEBUG3(x) WPI_DEBUG3(cs::Logger::GetInstance(), x)
#define DEBUG4(x) WPI_DEBUG4(cs::Logger::GetInstance(), x)

} // namespace cs

#endif  // CAMERASERVER_LOG_H_
