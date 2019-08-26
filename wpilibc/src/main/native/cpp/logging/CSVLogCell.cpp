/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/logging/CSVLogCell.h"

#include <algorithm>

#include <wpi/raw_ostream.h>

using namespace frc;

CSVLogCell::CSVLogCell(wpi::StringRef name) {
  if (IsStringValid(name)) {
    m_name = name;
  }
}

const std::string& CSVLogCell::GetName() const { return m_name; }

const std::string& CSVLogCell::GetContent() const { return m_content; }

void CSVLogCell::ClearCell() { m_content.clear(); }

bool CSVLogCell::IsStringValid(wpi::StringRef string) {
  if (string.find('\"') != std::string::npos) {
    wpi::outs() << "CSVLogCell's name/content cannot contain double quotes"
                << '\n';
    return false;
  } else if (string.find('\n') != std::string::npos) {
    wpi::outs() << "CSVLogCell's name/content cannot contain newlines" << '\n';
    return false;
  }
  return true;
}
