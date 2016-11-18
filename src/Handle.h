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
  SourceData(CS_SourceKind kind_, std::shared_ptr<SourceImpl> source_)
      : kind{kind_}, refCount{0}, source{source_} {}

  CS_SourceKind kind;
  std::atomic_int refCount;
  std::shared_ptr<SourceImpl> source;
};

class Sources
    : public UnlimitedHandleResource<Handle, SourceData, Handle::kSource> {
 public:
  static Sources& GetInstance() {
    ATOMIC_STATIC(Sources, instance);
    return instance;
  }

  std::pair<CS_Source, std::shared_ptr<SourceData>> Find(
      const SourceImpl& source) {
    return FindIf(
        [&](const SourceData& data) { return data.source.get() == &source; });
  }

 private:
  Sources() = default;

  ATOMIC_STATIC_DECL(Sources)
};

struct SinkData {
  explicit SinkData(CS_SinkKind kind_, std::shared_ptr<SinkImpl> sink_)
      : kind{kind_}, refCount{0}, sourceHandle{0}, sink{sink_} {}

  CS_SinkKind kind;
  std::atomic_int refCount;
  std::atomic<CS_Source> sourceHandle;
  std::shared_ptr<SinkImpl> sink;
};

class Sinks : public UnlimitedHandleResource<Handle, SinkData, Handle::kSink> {
 public:
  static Sinks& GetInstance() {
    ATOMIC_STATIC(Sinks, instance);
    return instance;
  }

  std::pair<CS_Sink, std::shared_ptr<SinkData>> Find(const SinkImpl& sink) {
    return FindIf(
        [&](const SinkData& data) { return data.sink.get() == &sink; });
  }

 private:
  Sinks() = default;

  ATOMIC_STATIC_DECL(Sinks)
};

}  // namespace cs

#endif  // CS_HANDLE_H_
