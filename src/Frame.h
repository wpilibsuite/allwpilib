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
#include <memory>

#include "llvm/StringRef.h"

#include "cameraserver_cpp.h"

namespace cs {

class SourceImpl;

class Frame {
  friend class SourceImpl;

 public:
  typedef std::chrono::system_clock::time_point Time;

 private:
  struct Data {
    explicit Data(std::size_t capacity_)
        : data(new char[capacity_]), size(0), capacity(capacity_) {}
    ~Data() { delete[] data; }

    std::atomic_int refcount{0};
    Time time;
    char* data;
    std::size_t size;
    std::size_t capacity;
    VideoMode::PixelFormat pixelFormat;
  };

 public:
  Frame() noexcept : m_source{nullptr}, m_data{nullptr} {}

  Frame(SourceImpl& source, std::unique_ptr<Data> data) noexcept
      : m_source{&source},
        m_data{data.release()} {
    if (m_data) ++(m_data->refcount);
  }

  Frame(const Frame& frame) noexcept : m_source{frame.m_source},
                                       m_data{frame.m_data} {
    if (m_data) ++(m_data->refcount);
  }

  Frame(Frame&& other) noexcept : Frame() { swap(*this, other); }

  ~Frame() { DecRef(); }

  Frame& operator=(Frame other) noexcept {
    swap(*this, other);
    return *this;
  }

  explicit operator bool() const { return m_data; }

  operator llvm::StringRef() const {
    if (!m_data) return llvm::StringRef{};
    return llvm::StringRef(m_data->data, m_data->size);
  }

  std::size_t size() const {
    if (!m_data) return 0;
    return m_data->size;
  }

  const char* data() const {
    if (!m_data) return nullptr;
    return m_data->data;
  }

  VideoMode::PixelFormat GetPixelFormat() const {
    if (!m_data) return VideoMode::kUnknown;
    return m_data->pixelFormat;
  }

  Time time() const {
    if (!m_data) return Time{};
    return m_data->time;
  }

  friend void swap(Frame& first, Frame& second) noexcept {
    using std::swap;
    swap(first.m_source, second.m_source);
    swap(first.m_data, second.m_data);
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
