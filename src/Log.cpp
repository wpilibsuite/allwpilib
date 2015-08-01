/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Log.h"

using namespace nt;

ATOMIC_STATIC_INIT(Logger)

Logger::Logger() : m_min_level(100) {}

Logger::~Logger() {}
