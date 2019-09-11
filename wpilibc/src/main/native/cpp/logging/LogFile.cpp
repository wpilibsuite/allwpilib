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

void LogFile::Log(wpi::StringRef text) {
  m_file << text;
  UpdateFilename();
}

void LogFile::LogTwine(const wpi::Twine& text) { Log(text.str()); }

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
  // Get current date/time, format is YYYY-MM-DD.HH_mm_ss
  struct tm localTime = *std::localtime(&time);
  char datetime[80];
  std::strftime(datetime, sizeof(datetime), "%Y-%m-%d-%H_%M_%S", &localTime);

  return m_filePrefix + "-" + datetime + "." + m_fileExtension;
}
