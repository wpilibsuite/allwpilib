// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_HANDLE_H_
#define CSCORE_HANDLE_H_

#include <wpi/Synchronization.h>

#include "cscore_c.h"

namespace cs {

// Handle data layout:
// Bits 0-15:  Handle index
// Bits 16-23: Parent index (property only)
// Bits 24-30: Type

class Handle {
 public:
  enum Type {
    kUndefined = 0,
    kProperty = wpi::kHandleTypeCSBase,
    kSource,
    kSink,
    kListener,
    kSinkProperty,
    kListenerPoller
  };
  enum { kIndexMax = 0xffff };

  Handle(CS_Handle handle) : m_handle(handle) {}  // NOLINT
  operator CS_Handle() const { return m_handle; }

  Handle(int index, Type type) {
    if (index < 0) {
      m_handle = 0;
      return;
    }
    m_handle = ((static_cast<int>(type) & 0x7f) << 24) | (index & 0xffff);
  }
  Handle(int index, int property, Type type) {
    if (index < 0 || property < 0) {
      m_handle = 0;
      return;
    }
    m_handle = ((static_cast<int>(type) & 0x7f) << 24) |
               ((index & 0xff) << 16) | (property & 0xffff);
  }

  int GetIndex() const { return static_cast<int>(m_handle) & 0xffff; }
  Type GetType() const {
    return static_cast<Type>((static_cast<int>(m_handle) >> 24) & 0xff);
  }
  bool IsType(Type type) const { return type == GetType(); }
  int GetTypedIndex(Type type) const { return IsType(type) ? GetIndex() : -1; }
  int GetParentIndex() const {
    return (IsType(Handle::kProperty) || IsType(Handle::kSinkProperty))
               ? (static_cast<int>(m_handle) >> 16) & 0xff
               : -1;
  }

 private:
  CS_Handle m_handle;
};

}  // namespace cs

#endif  // CSCORE_HANDLE_H_
