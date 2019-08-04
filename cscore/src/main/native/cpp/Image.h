/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_IMAGE_H_
#define CSCORE_IMAGE_H_

#include <vector>

#include <opencv2/core/core.hpp>
#include <wpi/StringRef.h>

#include "cscore_cpp.h"
#include "default_init_allocator.h"

namespace cs {

class Frame;

class Image {
  friend class Frame;

 public:
#ifndef __linux__
  explicit Image(size_t capacity) { m_data.reserve(capacity); }
#else
  explicit Image(size_t capacity)
      : m_data{capacity, default_init_allocator<uchar>{}} {
    m_data.resize(0);
  }
#endif

  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;

  // Getters
  operator wpi::StringRef() const { return str(); }
  wpi::StringRef str() const { return wpi::StringRef(data(), size()); }
  size_t capacity() const { return m_data.capacity(); }
  const char* data() const {
    return reinterpret_cast<const char*>(m_data.data());
  }
  char* data() { return reinterpret_cast<char*>(m_data.data()); }
  size_t size() const { return m_data.size(); }

  const std::vector<uchar>& vec() const { return m_data; }
  std::vector<uchar>& vec() { return m_data; }

  void resize(size_t size) { m_data.resize(size); }
  void SetSize(size_t size) { m_data.resize(size); }

  cv::Mat AsMat() {
    int type;
    switch (pixelFormat) {
      case VideoMode::kYUYV:
      case VideoMode::kRGB565:
        type = CV_8UC2;
        break;
      case VideoMode::kBGR:
        type = CV_8UC3;
        break;
      case VideoMode::kGray:
      case VideoMode::kMJPEG:
      default:
        type = CV_8UC1;
        break;
    }
    return cv::Mat{height, width, type, m_data.data()};
  }

  cv::_InputArray AsInputArray() { return cv::_InputArray{m_data}; }

  bool Is(int width_, int height_) {
    return width == width_ && height == height_;
  }
  bool Is(int width_, int height_, VideoMode::PixelFormat pixelFormat_) {
    return width == width_ && height == height_ && pixelFormat == pixelFormat_;
  }
  bool Is(int width_, int height_, VideoMode::PixelFormat pixelFormat_,
          int jpegQuality_) {
    // Consider +/-5 on JPEG quality to be "close enough"
    return width == width_ && height == height_ &&
           pixelFormat == pixelFormat_ &&
           (pixelFormat != VideoMode::kMJPEG || jpegQuality_ == -1 ||
            (jpegQuality != -1 && std::abs(jpegQuality - jpegQuality_) <= 5));
  }
  bool IsLarger(int width_, int height_) {
    return width >= width_ && height >= height_;
  }
  bool IsLarger(const Image& oth) {
    return width >= oth.width && height >= oth.height;
  }
  bool IsSmaller(int width_, int height_) { return !IsLarger(width_, height_); }
  bool IsSmaller(const Image& oth) { return !IsLarger(oth); }

 private:
  std::vector<uchar> m_data;

 public:
  VideoMode::PixelFormat pixelFormat{VideoMode::kUnknown};
  int width{0};
  int height{0};
  int jpegQuality{-1};
};

}  // namespace cs

#endif  // CSCORE_IMAGE_H_
