// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/Logger.h>

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
