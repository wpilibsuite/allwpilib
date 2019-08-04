/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_HANDLE_H_
#define NTCORE_HANDLE_H_

#include "ntcore_c.h"

namespace nt {

// Handle data layout:
// Bits 30-28: Type
// Bits 27-20: Instance index
// Bits 19-0:  Handle index (0/unused for instance handles)

class Handle {
 public:
  enum Type {
    kConnectionListener = 1,
    kConnectionListenerPoller,
    kEntry,
    kEntryListener,
    kEntryListenerPoller,
    kInstance,
    kLogger,
    kLoggerPoller,
    kRpcCall,
    kRpcCallPoller
  };
  enum { kIndexMax = 0xfffff };

  explicit Handle(NT_Handle handle) : m_handle(handle) {}
  operator NT_Handle() const { return m_handle; }

  NT_Handle handle() const { return m_handle; }

  Handle(int inst, int index, Type type) {
    if (inst < 0 || index < 0) {
      m_handle = 0;
      return;
    }
    m_handle = ((static_cast<int>(type) & 0xf) << 27) | ((inst & 0x7f) << 20) |
               (index & 0xfffff);
  }

  int GetIndex() const { return static_cast<int>(m_handle) & 0xfffff; }
  Type GetType() const {
    return static_cast<Type>((static_cast<int>(m_handle) >> 27) & 0xf);
  }
  int GetInst() const { return (static_cast<int>(m_handle) >> 20) & 0x7f; }
  bool IsType(Type type) const { return type == GetType(); }
  int GetTypedIndex(Type type) const { return IsType(type) ? GetIndex() : -1; }
  int GetTypedInst(Type type) const { return IsType(type) ? GetInst() : -1; }

 private:
  NT_Handle m_handle;
};

}  // namespace nt

#endif  // NTCORE_HANDLE_H_
