// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/Synchronization.h>

#include "ntcore_c.h"

namespace nt {

// Handle data layout:
// Bits 30-24: Type
// Bits 23-20: Instance index
// Bits 19-0:  Handle index (0/unused for instance handles)

class Handle {
 public:
  enum Type {
    kListener = wpi::kHandleTypeNTBase,
    kListenerPoller,
    kEntry,
    kInstance,
    kDataLogger,
    kConnectionDataLogger,
    kMultiSubscriber,
    kTopic,
    kSubscriber,
    kPublisher,
    kTypeMax
  };
  static_assert(kTypeMax <= wpi::kHandleTypeHALBase);
  enum { kIndexMax = 0xfffff };

  constexpr explicit Handle(NT_Handle handle) : m_handle(handle) {}
  constexpr operator NT_Handle() const { return m_handle; }

  constexpr NT_Handle handle() const { return m_handle; }

  constexpr Handle(int inst, int index, Type type) {
    if (inst < 0 || index < 0) {
      m_handle = 0;
      return;
    }
    m_handle = ((static_cast<int>(type) & 0x7f) << 24) | ((inst & 0xf) << 20) |
               (index & 0xfffff);
  }

  constexpr unsigned int GetIndex() const {
    return static_cast<unsigned int>(m_handle) & 0xfffff;
  }
  constexpr Type GetType() const {
    return static_cast<Type>((static_cast<int>(m_handle) >> 24) & 0x7f);
  }
  constexpr int GetInst() const {
    return (static_cast<int>(m_handle) >> 20) & 0xf;
  }
  constexpr bool IsType(Type type) const { return type == GetType(); }
  constexpr int GetTypedIndex(Type type) const {
    return IsType(type) ? GetIndex() : -1;
  }
  constexpr int GetTypedInst(Type type) const {
    return IsType(type) ? GetInst() : -1;
  }

 private:
  NT_Handle m_handle;
};

}  // namespace nt
