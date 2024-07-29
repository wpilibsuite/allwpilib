// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===- SmallVectorMemoryBuffer.h --------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a wrapper class to hold the memory into which an
// object will be generated.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "wpi/MemoryBuffer.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_ostream.h"

namespace wpi {

/// SmallVector-backed MemoryBuffer instance.
///
/// This class enables efficient construction of MemoryBuffers from SmallVector
/// instances.
class SmallVectorMemoryBuffer : public MemoryBuffer {
 public:
  /// Construct an SmallVectorMemoryBuffer from the given SmallVector
  /// r-value.
  SmallVectorMemoryBuffer(SmallVectorImpl<uint8_t>&& sv)  // NOLINT
      : m_sv(std::move(sv)), m_bufferName("<in-memory object>") {
    Init(this->m_sv.begin(), this->m_sv.end());
  }

  /// Construct a named SmallVectorMemoryBuffer from the given
  /// SmallVector r-value and StringRef.
  SmallVectorMemoryBuffer(SmallVectorImpl<uint8_t>&& sv, std::string_view name)
      : m_sv(std::move(sv)), m_bufferName(name) {
    Init(this->m_sv.begin(), this->m_sv.end());
  }

  // Key function.
  ~SmallVectorMemoryBuffer() override;

  std::string_view GetBufferIdentifier() const override { return m_bufferName; }

  BufferKind GetBufferKind() const override { return MemoryBuffer_Malloc; }

 private:
  SmallVector<uint8_t, 0> m_sv;
  std::string m_bufferName;
};

}  // namespace wpi
