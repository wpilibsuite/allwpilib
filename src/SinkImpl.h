/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_SINKIMPL_H_
#define CAMERASERVER_SINKIMPL_H_

#include <memory>
#include <mutex>
#include <string>

#include "llvm/StringRef.h"

namespace cs {

class Frame;
class SourceImpl;

class SinkImpl {
 public:
  SinkImpl(llvm::StringRef name);
  virtual ~SinkImpl() = default;
  SinkImpl(const SinkImpl& queue) = delete;
  SinkImpl& operator=(const SinkImpl& queue) = delete;

  llvm::StringRef GetName() const { return m_name; }
  virtual void GetDescription(llvm::SmallVectorImpl<char>& desc) const = 0;

  void SetSource(std::shared_ptr<SourceImpl> source) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_source = source;
  }

  std::shared_ptr<SourceImpl> GetSource() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_source;
  }

 private:
  std::string m_name;
  mutable std::mutex m_mutex;
  std::shared_ptr<SourceImpl> m_source;
};

}  // namespace cs

#endif  // CAMERASERVER_SINKIMPL_H_
