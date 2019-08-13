/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogCell.h"

using namespace frc;

LogCell::LogCell(std::string name) : m_name(name), m_mutex() {
  pthread_mutex_init(&m_mutex, NULL);
}

LogCell::~LogCell() { pthread_mutex_destroy(&m_mutex); }

std::string LogCell::GetName() { return m_name; }

std::string LogCell::GetContent() { return m_content; }

void LogCell::ClearCell() { m_content.clear(); }

void LogCell::AcquireLock() { pthread_mutex_lock(&m_mutex); }

void LogCell::ReleaseLock() { pthread_mutex_unlock(&m_mutex); }
