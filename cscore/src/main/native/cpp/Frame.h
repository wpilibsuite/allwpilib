/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

#include "CompressionContext.h"
#include "Image.h"
#include "cscore_cpp.h"

namespace cs {

class FramePool;

class Frame {
  friend class FramePool;

 public:
  using Time = uint64_t;

 private:
  struct Impl {
    explicit Impl(FramePool& framePool_) : framePool(framePool_) {}

    wpi::recursive_mutex mutex;
    std::atomic_int refcount{0};
    Time time{0};
    FramePool& framePool;
    std::string error;
    wpi::SmallVector<Image*, 4> images;
    std::vector<int> mjpegParams;
  };

 public:
  Frame() noexcept : m_impl{nullptr} {}

  Frame(FramePool& framePool, const wpi::Twine& error, Time time);

  Frame(FramePool& framePool, std::unique_ptr<Image> image, Time time);

  Frame(const Frame& frame) noexcept : m_impl{frame.m_impl} {
    if (m_impl) ++m_impl->refcount;
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
    if (!m_impl) return wpi::StringRef{};
    return m_impl->error;
  }

  int GetOriginalWidth() const {
    if (!m_impl) return 0;
    std::scoped_lock lock(m_impl->mutex);
    if (m_impl->images.empty()) return 0;
    return m_impl->images[0]->width;
  }

  int GetOriginalHeight() const {
    if (!m_impl) return 0;
    std::scoped_lock lock(m_impl->mutex);
    if (m_impl->images.empty()) return 0;
    return m_impl->images[0]->height;
  }

  int GetOriginalPixelFormat() const {
    if (!m_impl) return 0;
    std::scoped_lock lock(m_impl->mutex);
    if (m_impl->images.empty()) return 0;
    return m_impl->images[0]->pixelFormat;
  }

  int GetOriginalJpegQuality() const {
    Image* image = GetExistingImage();
    if (!image)
      return 0;
    else
      return image->jpegQuality;
  }

  int GetOriginalH264Bitrate() const {
    Image* image = GetExistingImage();
    if (!image)
      return 0;
    else
      return image->h264Bitrate;
  }

  Image* GetExistingImage(size_t i = 0) const {
    if (!m_impl) return nullptr;
    std::scoped_lock lock(m_impl->mutex);
    if (i >= m_impl->images.size()) return nullptr;
    return m_impl->images[i];
  }

  Image* GetExistingImage(int width, int height) const {
    if (!m_impl) return nullptr;
    std::scoped_lock lock(m_impl->mutex);
    for (auto i : m_impl->images) {
      if (i->Is(width, height)) return i;
    }
    return nullptr;
  }

  Image* GetExistingImage(int width, int height,
                          VideoMode::PixelFormat pixelFormat) const {
    if (!m_impl) return nullptr;
    std::scoped_lock lock(m_impl->mutex);
    for (auto i : m_impl->images) {
      if (i->Is(width, height, pixelFormat)) return i;
    }
    return nullptr;
  }

  Image* GetExistingImage(
      int width, int height, VideoMode::PixelFormat pixelFormat,
      const CompressionContext::CompressionSettings& settings) const {
    if (!m_impl) return nullptr;
    std::scoped_lock lock(m_impl->mutex);
    for (auto i : m_impl->images) {
      if (i->Is(width, height, pixelFormat, settings)) return i;
    }
    return nullptr;
  }

  Image* GetNearestImage(int width, int height) const;
  Image* GetNearestImage(
      int width, int height, VideoMode::PixelFormat pixelFormat,
      const CompressionContext::CompressionSettings& compressionSettings) const;

  Image* Convert(Image* image, VideoMode::PixelFormat pixelFormat,
                 const CompressionContext& compressionContext);

  Image* GetImage(int width, int height, VideoMode::PixelFormat pixelFormat,
                  const CompressionContext& compressionContext);

  bool GetCv(cv::Mat& image, const CompressionContext& compressionContext) {
    return GetCv(image, GetOriginalWidth(), GetOriginalHeight(),
                 compressionContext);
  }
  bool GetCv(cv::Mat& image, int width, int height,
             const CompressionContext& compressionContext);

 private:
  Image* ConvertH264ToBGR(Image* image,
                          const CompressionContext& compressionContext);
  Image* ConvertMJPEGToBGR(Image* image);
  Image* ConvertYUYVToBGR(Image* image);
  Image* ConvertBGRToRGB565(Image* image);
  Image* ConvertRGB565ToBGR(Image* image);
  Image* ConvertBGRToGray(Image* image);
  Image* ConvertGrayToBGR(Image* image);
  Image* ConvertBGRToMJPEG(Image* image, int quality);
  Image* ConvertGrayToMJPEG(Image* image, int quality);
  Image* ConvertBGRToH264(Image* image,
                          const CompressionContext& compressionContext);

  void DecRef() {
    if (m_impl && --(m_impl->refcount) == 0) ReleaseFrame();
  }
  void ReleaseFrame();

  Impl* m_impl;
};

}  // namespace cs

#endif  // CSCORE_FRAME_H_
