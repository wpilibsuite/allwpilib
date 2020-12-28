// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_FRAME_H_
#define CSCORE_FRAME_H_

#include <atomic>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "Image.h"
#include "cscore_cpp.h"

namespace cs {

class SourceImpl;

class Frame {
  friend class SourceImpl;

 public:
  using Time = uint64_t;

 private:
  struct Impl {
    explicit Impl(SourceImpl& source_) : source(source_) {}

    wpi::recursive_mutex mutex;
    std::atomic_int refcount{0};
    Time time{0};
    SourceImpl& source;
    std::string error;
    wpi::SmallVector<Image*, 4> images;
    std::vector<int> compressionParams;
  };

 public:
  Frame() noexcept = default;

  Frame(SourceImpl& source, const wpi::Twine& error, Time time);

  Frame(SourceImpl& source, std::unique_ptr<Image> image, Time time);

  Frame(const Frame& frame) noexcept : m_impl{frame.m_impl} {
    if (m_impl) {
      ++m_impl->refcount;
    }
  }

  Frame(Frame&& other) noexcept : Frame() { swap(*this, other); }

  ~Frame() { DecRef(); }

  Frame& operator=(Frame other) noexcept {
    swap(*this, other);
    return *this;
  }

  explicit operator bool() const { return m_impl && m_impl->error.empty(); }

  friend void swap(Frame& first, Frame& second) noexcept {
    using std::swap;
    swap(first.m_impl, second.m_impl);
  }

  Time GetTime() const { return m_impl ? m_impl->time : 0; }

  wpi::StringRef GetError() const {
    if (!m_impl) {
      return {};
    }
    return m_impl->error;
  }

  int GetOriginalWidth() const {
    if (!m_impl) {
      return 0;
    }
    std::scoped_lock lock(m_impl->mutex);
    if (m_impl->images.empty()) {
      return 0;
    }
    return m_impl->images[0]->width;
  }

  int GetOriginalHeight() const {
    if (!m_impl) {
      return 0;
    }
    std::scoped_lock lock(m_impl->mutex);
    if (m_impl->images.empty()) {
      return 0;
    }
    return m_impl->images[0]->height;
  }

  int GetOriginalPixelFormat() const {
    if (!m_impl) {
      return 0;
    }
    std::scoped_lock lock(m_impl->mutex);
    if (m_impl->images.empty()) {
      return 0;
    }
    return m_impl->images[0]->pixelFormat;
  }

  int GetOriginalJpegQuality() const {
    if (!m_impl) {
      return 0;
    }
    std::scoped_lock lock(m_impl->mutex);
    if (m_impl->images.empty()) {
      return 0;
    }
    return m_impl->images[0]->jpegQuality;
  }

  Image* GetExistingImage(size_t i = 0) const {
    if (!m_impl) {
      return nullptr;
    }
    std::scoped_lock lock(m_impl->mutex);
    if (i >= m_impl->images.size()) {
      return nullptr;
    }
    return m_impl->images[i];
  }

  Image* GetExistingImage(int width, int height) const {
    if (!m_impl) {
      return nullptr;
    }
    std::scoped_lock lock(m_impl->mutex);
    for (auto i : m_impl->images) {
      if (i->Is(width, height)) {
        return i;
      }
    }
    return nullptr;
  }

  Image* GetExistingImage(int width, int height,
                          VideoMode::PixelFormat pixelFormat) const {
    if (!m_impl) {
      return nullptr;
    }
    std::scoped_lock lock(m_impl->mutex);
    for (auto i : m_impl->images) {
      if (i->Is(width, height, pixelFormat)) {
        return i;
      }
    }
    return nullptr;
  }

  Image* GetExistingImage(int width, int height,
                          VideoMode::PixelFormat pixelFormat,
                          int jpegQuality) const {
    if (!m_impl) {
      return nullptr;
    }
    std::scoped_lock lock(m_impl->mutex);
    for (auto i : m_impl->images) {
      if (i->Is(width, height, pixelFormat, jpegQuality)) {
        return i;
      }
    }
    return nullptr;
  }

  Image* GetNearestImage(int width, int height) const;
  Image* GetNearestImage(int width, int height,
                         VideoMode::PixelFormat pixelFormat,
                         int jpegQuality = -1) const;

  Image* Convert(Image* image, VideoMode::PixelFormat pixelFormat) {
    if (pixelFormat == VideoMode::kMJPEG) {
      return nullptr;
    }
    return ConvertImpl(image, pixelFormat, -1, 80);
  }
  Image* ConvertToMJPEG(Image* image, int requiredQuality,
                        int defaultQuality = 80) {
    return ConvertImpl(image, VideoMode::kMJPEG, requiredQuality,
                       defaultQuality);
  }
  Image* ConvertMJPEGToBGR(Image* image);
  Image* ConvertMJPEGToGray(Image* image);
  Image* ConvertYUYVToBGR(Image* image);
  Image* ConvertBGRToRGB565(Image* image);
  Image* ConvertRGB565ToBGR(Image* image);
  Image* ConvertBGRToGray(Image* image);
  Image* ConvertGrayToBGR(Image* image);
  Image* ConvertBGRToMJPEG(Image* image, int quality);
  Image* ConvertGrayToMJPEG(Image* image, int quality);

  Image* GetImage(int width, int height, VideoMode::PixelFormat pixelFormat) {
    if (pixelFormat == VideoMode::kMJPEG) {
      return nullptr;
    }
    return GetImageImpl(width, height, pixelFormat, -1, 80);
  }
  Image* GetImageMJPEG(int width, int height, int requiredQuality,
                       int defaultQuality = 80) {
    return GetImageImpl(width, height, VideoMode::kMJPEG, requiredQuality,
                        defaultQuality);
  }

  bool GetCv(cv::Mat& image) {
    return GetCv(image, GetOriginalWidth(), GetOriginalHeight());
  }
  bool GetCv(cv::Mat& image, int width, int height);

 private:
  Image* ConvertImpl(Image* image, VideoMode::PixelFormat pixelFormat,
                     int requiredJpegQuality, int defaultJpegQuality);
  Image* GetImageImpl(int width, int height, VideoMode::PixelFormat pixelFormat,
                      int requiredJpegQuality, int defaultJpegQuality);
  void DecRef() {
    if (m_impl && --(m_impl->refcount) == 0) {
      ReleaseFrame();
    }
  }
  void ReleaseFrame();

  Impl* m_impl{nullptr};
};

}  // namespace cs

#endif  // CSCORE_FRAME_H_
