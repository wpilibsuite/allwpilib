/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/LogFile.h"

#include <cstdio>
#include <ctime>

#include <wpi/raw_ostream.h>

using namespace frc;

LogFile::LogFile(wpi::StringRef filePrefix, wpi::StringRef fileExtension)
    : m_filePrefix(filePrefix), m_fileExtension(fileExtension) {
  m_time = std::time(0);
  std::string filename = CreateFilename(m_time);

  m_file.open(filename);

  if (m_file.fail()) {
    wpi::outs() << "Could not open file `" << filename << "` for writing."
                << '\n';
    return;
  }
}

void LogFile::Log(const wpi::StringRef& text) { *this << text; }

void LogFile::Logln(const wpi::StringRef& text) { *this << text << '\n'; }

const std::string LogFile::GetFileName() const {
  return CreateFilename(m_time);
}

void LogFile::SetTimeIntervalBeforeRenaming(units::second_t duration) {
  m_timeIntervalBeforeRenaming = duration;
}

void LogFile::UpdateFilename() {
  std::time_t newTime = std::time(0);
  // If the difference between the two timestamps is too long
  if (units::second_t{std::difftime(newTime, m_time)} >
      m_timeIntervalBeforeRenaming) {
    std::string newName = CreateFilename(newTime);
    m_file.close();
    std::rename(CreateFilename(m_time).c_str(), newName.c_str());
    m_file.open(newName);
  }

  m_time = newTime;
}

const std::string LogFile::CreateFilename(std::time_t time) const {
  // Get current date/time, format is YYYY-MM-DD.HH_mm_ss
  struct tm localTime = *std::localtime(&time);
  char datetime[80];
  std::strftime(datetime, sizeof(datetime), "%Y-%m-%d-%H_%M_%S", &localTime);

  return m_filePrefix + "-" + datetime + "." + m_fileExtension;
}
