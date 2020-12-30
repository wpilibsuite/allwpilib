// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <vector>

#include <wpi/Twine.h>

#include "glass/View.h"

namespace glass {

class LogData {
  friend void DisplayLog(LogData*, bool);

 public:
  explicit LogData(size_t maxLines = 10000) : m_maxLines{maxLines} {}

  void Clear();
  void Append(const wpi::Twine& msg);

 private:
  size_t m_maxLines;
  std::vector<char> m_buf;
  std::vector<size_t> m_lineOffsets{0};
};

void DisplayLog(LogData* data, bool autoScroll);

class LogView : public View {
 public:
  explicit LogView(LogData* data) : m_data{data} {}

  void Display() override;

 private:
  LogData* m_data;
  bool m_autoScroll{true};
};

}  // namespace glass
