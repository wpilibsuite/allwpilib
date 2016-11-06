/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_HANDLE_H_
#define CS_HANDLE_H_

#include <atomic>

#include "llvm/StringRef.h"

#include "cscore_c.h"
#include "UnlimitedHandleResource.h"

namespace cs {

class SinkImpl;
class SourceImpl;

// Handle data layout:
// Bits 0-15:  Handle index
// Bits 16-23: Parent index (property only)
// Bits 24-30: Type

class Handle {
 public:
  enum Type {
    kUndefined = 0,
    kProperty = 0x40,
    kSource,
    kSink,
    kListener
  };
  enum { kIndexMax = 0xffff };

  Handle(CS_Handle handle) : m_handle(handle) {}
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
    return IsType(Handle::kProperty) ? (static_cast<int>(m_handle) >> 16) & 0xff
                                     : -1;
  }

 private:
  CS_Handle m_handle;
};

struct SourceData {
  enum Type {
    kUnknown = 0,
    kUSB,
    kHTTP,
    kCv
  };
  SourceData(Type type_, std::shared_ptr<SourceImpl> source_)
      : type{type_}, refCount{0}, source{source_} {}

  Type type;
  std::atomic_int refCount;
  std::shared_ptr<SourceImpl> source;
};

typedef StaticUnlimitedHandleResource<Handle, SourceData, Handle::kSource>
    Sources;

struct SinkData {
  enum Type {
    kUnknown = 0,
    kHTTP,
    kCv
  };
  explicit SinkData(Type type_, std::shared_ptr<SinkImpl> sink_)
      : type{type_}, refCount{0}, sourceHandle{0}, sink{sink_} {}

  Type type;
  std::atomic_int refCount;
  std::atomic<CS_Source> sourceHandle;
  std::shared_ptr<SinkImpl> sink;
};

typedef StaticUnlimitedHandleResource<Handle, SinkData, Handle::kSink> Sinks;

}  // namespace cs

#endif  // CS_HANDLE_H_
