/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_LOG_H_
#define NTCORE_LOG_H_

#include <wpi/Logger.h>

#define LOG(level, x) WPI_LOG(m_logger, level, x)

#undef ERROR
#define ERROR(x) WPI_ERROR(m_logger, x)
#define WARNING(x) WPI_WARNING(m_logger, x)
#define INFO(x) WPI_INFO(m_logger, x)

#define DEBUG(x) WPI_DEBUG(m_logger, x)
#define DEBUG1(x) WPI_DEBUG1(m_logger, x)
#define DEBUG2(x) WPI_DEBUG2(m_logger, x)
#define DEBUG3(x) WPI_DEBUG3(m_logger, x)
#define DEBUG4(x) WPI_DEBUG4(m_logger, x)

#endif  // NTCORE_LOG_H_
