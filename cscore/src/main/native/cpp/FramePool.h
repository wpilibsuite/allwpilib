/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_FRAMEPOOL_H_
#define CSCORE_FRAMEPOOL_H_

#include <memory>
#include <utility>
#include <vector>

#include <wpi/StringRef.h>

#include "Frame.h"
#include "Image.h"

namespace cs {

class FramePool {
 public:
  FramePool() = default;
  FramePool(const FramePool&) = delete;
  FramePool& operator=(const FramePool&) = delete;
  ~FramePool();

  /**
   * Creates a frame from an already-created image.
   */
  Frame MakeFrame(std::unique_ptr<Image> image, Frame::Time time) {
    return Frame{*this, std::move(image), time};
  }

  /**
   * Creates an error frame.
   */
  Frame MakeErrorFrame(const wpi::Twine& msg, Frame::Time time) {
    return Frame{*this, msg, time};
  }

  /**
   * Creates an empty frame.
   */
  Frame MakeEmptyFrame() { return Frame{*this, wpi::StringRef{}, 0}; }

  /**
   * Allocates an image.
   */
  std::unique_ptr<Image> AllocImage(size_t size);

  std::unique_ptr<Image> AllocImage(VideoMode::PixelFormat pixelFormat,
                                    int width, int height, size_t size) {
    auto image = AllocImage(size);
    image->pixelFormat = pixelFormat;
    image->width = width;
    image->height = height;
    return image;
  }

  void ReleaseImage(std::unique_ptr<Image> image);

  std::unique_ptr<Frame::Impl> AllocFrameImpl();

  void ReleaseFrameImpl(std::unique_ptr<Frame::Impl> data);

 private:
  wpi::mutex m_mutex;
  bool m_destroy = false;
  std::vector<std::unique_ptr<Frame::Impl>> m_framesAvail;
  std::vector<std::unique_ptr<Image>> m_imagesAvail;
};

}  // namespace cs

#endif  // CSCORE_FRAMEPOOL_H_
