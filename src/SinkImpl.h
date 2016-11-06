/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CS_SINKIMPL_H_
#define CS_SINKIMPL_H_

#include <memory>
#include <mutex>
#include <string>

#include "llvm/StringRef.h"

#include "SourceImpl.h"

namespace cs {

class Frame;

class SinkImpl {
 public:
  SinkImpl(llvm::StringRef name);
  virtual ~SinkImpl();
  SinkImpl(const SinkImpl& queue) = delete;
  SinkImpl& operator=(const SinkImpl& queue) = delete;

  llvm::StringRef GetName() const { return m_name; }

  void SetDescription(llvm::StringRef description);
  llvm::StringRef GetDescription(llvm::SmallVectorImpl<char>& buf) const;

  void Enable() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_enabledCount == 0 && m_source) m_source->EnableSink();
    ++m_enabledCount;
  }

  void Disable() {
    std::lock_guard<std::mutex> lock(m_mutex);
    --m_enabledCount;
    if (m_enabledCount == 0 && m_source) m_source->DisableSink();
  }

  void SetEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (enabled && m_enabledCount == 0) {
      if (m_source) m_source->EnableSink();
      m_enabledCount = 1;
    } else if (!enabled && m_enabledCount > 0) {
      if (m_source) m_source->DisableSink();
      m_enabledCount = 0;
    }
  }

  void SetSource(std::shared_ptr<SourceImpl> source);

  std::shared_ptr<SourceImpl> GetSource() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_source;
  }

  std::string GetError() const;
  llvm::StringRef GetError(llvm::SmallVectorImpl<char>& buf) const;

 protected:
  mutable std::mutex m_mutex;

 private:
  std::string m_name;
  std::string m_description;
  std::shared_ptr<SourceImpl> m_source;
  int m_enabledCount{0};
};

}  // namespace cs

#endif  // CS_SINKIMPL_H_
