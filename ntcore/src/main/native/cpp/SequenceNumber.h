// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_SEQUENCENUMBER_H_
#define NTCORE_SEQUENCENUMBER_H_

namespace nt {

/* A sequence number per RFC 1982 */
class SequenceNumber {
 public:
  SequenceNumber() : m_value(0) {}
  explicit SequenceNumber(unsigned int value) : m_value(value) {}
  unsigned int value() const { return m_value; }

  SequenceNumber& operator++() {
    ++m_value;
    if (m_value > 0xffff) m_value = 0;
    return *this;
  }
  SequenceNumber operator++(int) {
    SequenceNumber tmp(*this);
    operator++();
    return tmp;
  }

  friend bool operator<(const SequenceNumber& lhs, const SequenceNumber& rhs);
  friend bool operator>(const SequenceNumber& lhs, const SequenceNumber& rhs);
  friend bool operator<=(const SequenceNumber& lhs, const SequenceNumber& rhs);
  friend bool operator>=(const SequenceNumber& lhs, const SequenceNumber& rhs);
  friend bool operator==(const SequenceNumber& lhs, const SequenceNumber& rhs);
  friend bool operator!=(const SequenceNumber& lhs, const SequenceNumber& rhs);

 private:
  unsigned int m_value;
};

bool operator<(const SequenceNumber& lhs, const SequenceNumber& rhs);
bool operator>(const SequenceNumber& lhs, const SequenceNumber& rhs);

inline bool operator<=(const SequenceNumber& lhs, const SequenceNumber& rhs) {
  return lhs == rhs || lhs < rhs;
}

inline bool operator>=(const SequenceNumber& lhs, const SequenceNumber& rhs) {
  return lhs == rhs || lhs > rhs;
}

inline bool operator==(const SequenceNumber& lhs, const SequenceNumber& rhs) {
  return lhs.m_value == rhs.m_value;
}

inline bool operator!=(const SequenceNumber& lhs, const SequenceNumber& rhs) {
  return lhs.m_value != rhs.m_value;
}

}  // namespace nt

#endif  // NTCORE_SEQUENCENUMBER_H_
