/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogFile.h"

#include <cstdio>
#include <ctime>
#include <iostream>

using namespace frc;

LogFile::LogFile(std::string filePrefix, std::string fileExtension)
    : m_filePrefix(filePrefix),
      m_fileExtension(fileExtension),
      m_active(false) {}

LogFile::~LogFile() {
  if (m_active) m_file.close();
}

void LogFile::Start() {
  if (m_active) {
    std::cout << "This table has already been initialized" << std::endl;
    return;
  }

  m_time = std::time(0);
  std::string filename = CreateFilename(m_time);

  m_file.open(filename.c_str());

  if (m_file.fail()) {
    std::cout << "Could not open file `" << filename << "` for writing."
              << std::endl;
    return;
  }

  m_active = true;
}

void LogFile::Stop() {
  if (m_active) {
    m_file.close();
    m_active = false;
  }
}

void LogFile::Log(std::string text) {
  m_file << text;
  UpdateFilename();
}

bool LogFile::IsActive() { return m_active; }

void LogFile::UpdateFilename() {
  std::time_t newTime = std::time(0);
  // If the difference between the two timestamps is more than 24 hours
  if (std::difftime(newTime, m_time) > 86400) {
    std::rename(CreateFilename(m_time).c_str(),
                CreateFilename(newTime).c_str());
  }

  m_time = newTime;
}

std::string LogFile::CreateFilename(std::time_t time) {
  // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
  struct tm localTime = *std::localtime(&time);
  char datetime[80];
  std::strftime(datetime, sizeof(datetime), "%Y-%m-%d.%X", &localTime);

  return m_filePrefix + "-" + datetime + "." + m_fileExtension;
}
