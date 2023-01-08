// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string_view>

#include <wpi/raw_ostream.h>

namespace nt::net {

class BinaryWriter;
class TextWriter;

class WireConnection {
  friend class TextWriter;
  friend class BinaryWriter;

 public:
  virtual ~WireConnection() = default;

  virtual bool Ready() const = 0;

  virtual TextWriter SendText() = 0;

  virtual BinaryWriter SendBinary() = 0;

  virtual void Flush() = 0;

  virtual void Disconnect(std::string_view reason) = 0;

 protected:
  virtual void StartSendText() = 0;
  virtual void FinishSendText() = 0;
  virtual void StartSendBinary() = 0;
  virtual void FinishSendBinary() = 0;
};

class TextWriter {
 public:
  TextWriter(wpi::raw_ostream& os, WireConnection& wire)
      : m_os{&os}, m_wire{&wire} {}
  TextWriter(const TextWriter&) = delete;
  TextWriter(TextWriter&& rhs) : m_os{rhs.m_os}, m_wire{rhs.m_wire} {
    rhs.m_os = nullptr;
    rhs.m_wire = nullptr;
  }
  TextWriter& operator=(const TextWriter&) = delete;
  TextWriter& operator=(TextWriter&& rhs) {
    m_os = rhs.m_os;
    m_wire = rhs.m_wire;
    rhs.m_os = nullptr;
    rhs.m_wire = nullptr;
    return *this;
  }
  ~TextWriter() {
    if (m_os) {
      m_wire->FinishSendText();
    }
  }

  wpi::raw_ostream& Add() {
    m_wire->StartSendText();
    return *m_os;
  }
  WireConnection& wire() { return *m_wire; }

 private:
  wpi::raw_ostream* m_os;
  WireConnection* m_wire;
};

class BinaryWriter {
 public:
  BinaryWriter(wpi::raw_ostream& os, WireConnection& wire)
      : m_os{&os}, m_wire{&wire} {}
  BinaryWriter(const BinaryWriter&) = delete;
  BinaryWriter(BinaryWriter&& rhs) : m_os{rhs.m_os}, m_wire{rhs.m_wire} {
    rhs.m_os = nullptr;
    rhs.m_wire = nullptr;
  }
  BinaryWriter& operator=(const BinaryWriter&) = delete;
  BinaryWriter& operator=(BinaryWriter&& rhs) {
    m_os = rhs.m_os;
    m_wire = rhs.m_wire;
    rhs.m_os = nullptr;
    rhs.m_wire = nullptr;
    return *this;
  }
  ~BinaryWriter() {
    if (m_wire) {
      m_wire->FinishSendBinary();
    }
  }

  wpi::raw_ostream& Add() {
    m_wire->StartSendBinary();
    return *m_os;
  }
  WireConnection& wire() { return *m_wire; }

 private:
  wpi::raw_ostream* m_os;
  WireConnection* m_wire;
};

}  // namespace nt::net
