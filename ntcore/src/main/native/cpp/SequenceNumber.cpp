// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SequenceNumber.h"

namespace nt {

bool operator<(const SequenceNumber& lhs, const SequenceNumber& rhs) {
  if (lhs.m_value < rhs.m_value) {
    return (rhs.m_value - lhs.m_value) < (1u << 15);
  } else if (lhs.m_value > rhs.m_value) {
    return (lhs.m_value - rhs.m_value) > (1u << 15);
  } else {
    return false;
  }
}

bool operator>(const SequenceNumber& lhs, const SequenceNumber& rhs) {
  if (lhs.m_value < rhs.m_value) {
    return (rhs.m_value - lhs.m_value) > (1u << 15);
  } else if (lhs.m_value > rhs.m_value) {
    return (lhs.m_value - rhs.m_value) < (1u << 15);
  } else {
    return false;
  }
}

}  // namespace nt
