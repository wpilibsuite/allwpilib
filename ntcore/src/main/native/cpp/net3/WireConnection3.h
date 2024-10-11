// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string_view>

namespace wpi {
class raw_ostream;
}  // namespace wpi

namespace nt::net3 {

class Writer;

class WireConnection3 {
  friend class Writer;

 public:
  virtual ~WireConnection3() = default;

  virtual bool Ready() const = 0;

  virtual Writer Send() = 0;

  virtual void Flush() = 0;

  virtual uint64_t GetLastFlushTime() const = 0;  // in microseconds

  virtual void StopRead() = 0;
  virtual void StartRead() = 0;

  virtual void Disconnect(std::string_view reason) = 0;

 protected:
  virtual void FinishSend() = 0;
};

class Writer {
 public:
  Writer(wpi::raw_ostream& os, WireConnection3& wire)
      : m_os{&os}, m_wire{&wire} {}
  Writer(const Writer&) = delete;
  Writer(Writer&& rhs) : m_os{rhs.m_os}, m_wire{rhs.m_wire} {
    rhs.m_os = nullptr;
    rhs.m_wire = nullptr;
  }
  ~Writer() {
    if (m_wire) {
      m_wire->FinishSend();
    }
  }
  Writer& operator=(const Writer&) = delete;
  Writer& operator=(Writer&& rhs) {
    m_os = rhs.m_os;
    m_wire = rhs.m_wire;
    rhs.m_os = nullptr;
    rhs.m_wire = nullptr;
    return *this;
  }

  wpi::raw_ostream& stream() { return *m_os; }

 private:
  wpi::raw_ostream* m_os;
  WireConnection3* m_wire;
};

}  // namespace nt::net3
