// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <compare>

namespace nt::net3 {

/* A sequence number per RFC 1982 */
class SequenceNumber {
 public:
  SequenceNumber() = default;
  explicit SequenceNumber(unsigned int value) : m_value(value) {}
  unsigned int value() const { return m_value; }

  SequenceNumber& operator++() {
    ++m_value;
    if (m_value > 0xffff) {
      m_value = 0;
    }
    return *this;
  }
  SequenceNumber operator++(int) {
    SequenceNumber tmp(*this);
    operator++();
    return tmp;
  }

  friend auto operator<=>(const SequenceNumber& lhs,
                          const SequenceNumber& rhs) = default;

 private:
  unsigned int m_value{0};
};

}  // namespace nt::net3
