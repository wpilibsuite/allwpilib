/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CAMERASERVER_FRAME_H_
#define CAMERASERVER_FRAME_H_

#include <atomic>
#include <chrono>

#include "llvm/SmallVector.h"

#include "Image.h"

namespace cs {

class SourceImpl;

class Frame {
  friend class SourceImpl;

  struct Data {
    std::atomic_int refcount{0};
    std::chrono::system_clock::time_point timestamp;
    llvm::SmallVector<Image, 4> images;
  };

 public:
  Frame(SourceImpl& source, Data* data) : m_source(&source), m_data(data) {
    if (m_data) ++(m_data->refcount);
  }

  Frame(const Frame& frame) : m_source(frame.m_source), m_data(frame.m_data) {
    if (m_data) ++(m_data->refcount);
  }

  Frame(Frame&& frame) : m_source(frame.m_source), m_data(frame.m_data) {
    frame.m_data = nullptr;
  }

  ~Frame() { DecRef(); }

  Frame& operator=(const Frame& frame) {
    DecRef();
    m_source = frame.m_source;
    m_data = frame.m_data;
    if (m_data) ++(m_data->refcount);
    return *this;
  }

  explicit operator bool() const { return m_data; }

  std::size_t size(std::size_t channel) const {
    if (!m_data || channel >= m_data->images.size()) return 0;
    return m_data->images[channel].size();
  }

  const char* data(std::size_t channel) const {
    if (!m_data || channel >= m_data->images.size()) return nullptr;
    return m_data->images[channel].data();
  }

  std::chrono::system_clock::time_point time() const {
    if (!m_data) return std::chrono::system_clock::time_point{};
    return m_data->timestamp;
  }

 private:
  void DecRef() {
    if (m_data && --(m_data->refcount) == 0) ReleaseFrame();
  }
  void ReleaseFrame();

  SourceImpl* m_source;
  Data* m_data;
};

}  // namespace cs

#endif  // CAMERASERVER_FRAME_H_
