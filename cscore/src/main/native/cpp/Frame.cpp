// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Frame.h"

#include <cstdlib>
#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Instance.h"
#include "SourceImpl.h"

using namespace cs;

Frame::Frame(SourceImpl& source, std::string_view error, Time time,
             WPI_TimestampSource timeSrc)
    : m_impl{source.AllocFrameImpl().release()} {
  m_impl->refcount = 1;
  m_impl->error = error;
  m_impl->time = time;
  m_impl->timeSource = timeSrc;
}

Frame::Frame(SourceImpl& source, std::unique_ptr<Image> image, Time time,
             WPI_TimestampSource timeSrc)
    : m_impl{source.AllocFrameImpl().release()} {
  m_impl->refcount = 1;
  m_impl->error.resize(0);
  m_impl->time = time;
  m_impl->timeSource = timeSrc;
  m_impl->images.push_back(image.release());
}

Image* Frame::GetNearestImage(int width, int height) const {
  if (!m_impl) {
    return nullptr;
  }
  std::scoped_lock lock(m_impl->mutex);
  Image* found = nullptr;

  // Ideally we want the smallest image at least width/height in size
  for (auto i : m_impl->images) {
    if (i->IsLarger(width, height) && (!found || (i->IsSmaller(*found)))) {
      found = i;
    }
  }
  if (found) {
    return found;
  }

  // Find the largest image (will be less than width/height)
  for (auto i : m_impl->images) {
    if (!found || (i->IsLarger(*found))) {
      found = i;
    }
  }
  if (found) {
    return found;
  }

  // Shouldn't reach this, but just in case...
  return m_impl->images.empty() ? nullptr : m_impl->images[0];
}

Image* Frame::GetNearestImage(int width, int height,
                              VideoMode::PixelFormat pixelFormat,
                              int jpegQuality) const {
  if (!m_impl) {
    return nullptr;
  }
  std::scoped_lock lock(m_impl->mutex);
  Image* found = nullptr;

  // We want the smallest image at least width/height (or the next largest),
  // but the primary search order is in order of conversion cost.
  // If we don't find exactly what we want, we prefer non-JPEG source images
  // (because JPEG source images require a decompression step).
  // While the searching takes a little time, it pales in comparison to the
  // image processing to come, so it's worth spending a little extra time
  // looking for the most efficient conversion.

  // 1) Same width, height, pixelFormat, and (possibly) JPEG quality
  //    (e.g. exactly what we want)
  for (auto i : m_impl->images) {
    if (i->Is(width, height, pixelFormat, jpegQuality)) {
      return i;
    }
  }

  // 2) Same width, height, different (but non-JPEG) pixelFormat (color conv)
  // 2a) If we want JPEG output, prefer BGR over other pixel formats
  if (pixelFormat == VideoMode::kMJPEG) {
    for (auto i : m_impl->images) {
      if (i->Is(width, height, VideoMode::kBGR)) {
        return i;
      }
    }
  }

  for (auto i : m_impl->images) {
    if (i->Is(width, height) && i->pixelFormat != VideoMode::kMJPEG) {
      return i;
    }
  }

  // 3) Different width, height, same pixelFormat (only if non-JPEG) (resample)
  if (pixelFormat != VideoMode::kMJPEG) {
    // 3a) Smallest image at least width/height in size
    for (auto i : m_impl->images) {
      if (i->IsLarger(width, height) && i->pixelFormat == pixelFormat &&
          (!found || (i->IsSmaller(*found)))) {
        found = i;
      }
    }
    if (found) {
      return found;
    }

    // 3b) Largest image (less than width/height)
    for (auto i : m_impl->images) {
      if (i->pixelFormat == pixelFormat && (!found || (i->IsLarger(*found)))) {
        found = i;
      }
    }
    if (found) {
      return found;
    }
  }

  // 4) Different width, height, different (but non-JPEG) pixelFormat
  //    (color conversion + resample)
  // 4a) Smallest image at least width/height in size
  for (auto i : m_impl->images) {
    if (i->IsLarger(width, height) && i->pixelFormat != VideoMode::kMJPEG &&
        (!found || (i->IsSmaller(*found)))) {
      found = i;
    }
  }
  if (found) {
    return found;
  }

  // 4b) Largest image (less than width/height)
  for (auto i : m_impl->images) {
    if (i->pixelFormat != VideoMode::kMJPEG &&
        (!found || (i->IsLarger(*found)))) {
      found = i;
    }
  }
  if (found) {
    return found;
  }

  // 5) Same width, height, JPEG pixelFormat (decompression).  As there may be
  //    multiple JPEG images, find the highest quality one.
  for (auto i : m_impl->images) {
    if (i->Is(width, height, VideoMode::kMJPEG) &&
        (!found || i->jpegQuality > found->jpegQuality)) {
      found = i;
      // consider one without a quality setting to be the highest quality
      // (e.g. directly from the camera)
      if (i->jpegQuality == -1) {
        break;
      }
    }
  }
  if (found) {
    return found;
  }

  // 6) Different width, height, JPEG pixelFormat (decompression)
  // 6a) Smallest image at least width/height in size
  for (auto i : m_impl->images) {
    if (i->IsLarger(width, height) && i->pixelFormat == VideoMode::kMJPEG &&
        (!found || (i->IsSmaller(*found)))) {
      found = i;
    }
  }
  if (found) {
    return found;
  }

  // 6b) Largest image (less than width/height)
  for (auto i : m_impl->images) {
    if (i->pixelFormat != VideoMode::kMJPEG &&
        (!found || (i->IsLarger(*found)))) {
      found = i;
    }
  }
  if (found) {
    return found;
  }

  // Shouldn't reach this, but just in case...
  return m_impl->images.empty() ? nullptr : m_impl->images[0];
}

Image* Frame::ConvertImpl(Image* image, VideoMode::PixelFormat pixelFormat,
                          int requiredJpegQuality, int defaultJpegQuality) {
  if (!image || image->Is(image->width, image->height, pixelFormat,
                          requiredJpegQuality)) {
    return image;
  }
  Image* cur = image;

  // If the source image is a JPEG, we need to decode it before we can do
  // anything else with it.  Note that if the destination format is JPEG, we
  // still need to do this (unless it was already a JPEG, in which case we
  // would have returned above).
  if (cur->pixelFormat == VideoMode::kMJPEG) {
    cur = ConvertMJPEGToBGR(cur);
    if (pixelFormat == VideoMode::kBGR) {
      return cur;
    }
  }

  // Color convert
  switch (pixelFormat) {
    case VideoMode::kRGB565:
      // If source is YUYV, UYVY, Gray, or Y16, need to convert to BGR first
      if (cur->pixelFormat == VideoMode::kYUYV) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertYUYVToBGR(cur);
        }
      } else if (cur->pixelFormat == VideoMode::kUYVY) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertUYVYToBGR(cur);
        }
      } else if (cur->pixelFormat == VideoMode::kGray) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertGrayToBGR(cur);
        }
      } else if (cur->pixelFormat == VideoMode::kY16) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else if (Image* newImage = GetExistingImage(cur->width, cur->height,
                                                      VideoMode::kGray)) {
          cur = ConvertGrayToBGR(newImage);
        } else {
          cur = ConvertGrayToBGR(ConvertY16ToGray(cur));
        }
      }
      return ConvertBGRToRGB565(cur);
    case VideoMode::kGray:
    case VideoMode::kY16:
      // If source is also grayscale, convert directly
      if (pixelFormat == VideoMode::kGray &&
          cur->pixelFormat == VideoMode::kY16) {
        return ConvertY16ToGray(cur);
      } else if (pixelFormat == VideoMode::kY16 &&
                 cur->pixelFormat == VideoMode::kGray) {
        return ConvertGrayToY16(cur);
      }
      // If source is YUYV, UYVY, convert directly to Gray
      // If RGB565, need to convert to BGR first
      if (cur->pixelFormat == VideoMode::kYUYV) {
        cur = ConvertYUYVToGray(cur);
      } else if (cur->pixelFormat == VideoMode::kUYVY) {
        cur = ConvertUYVYToGray(cur);
      } else if (cur->pixelFormat == VideoMode::kRGB565) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertRGB565ToBGR(cur);
        }
        cur = ConvertBGRToGray(cur);
      }
      if (pixelFormat == VideoMode::kY16) {
        cur = ConvertGrayToY16(cur);
      }
      return cur;
    case VideoMode::kBGR:
    case VideoMode::kMJPEG:
      if (cur->pixelFormat == VideoMode::kYUYV) {
        cur = ConvertYUYVToBGR(cur);
      } else if (cur->pixelFormat == VideoMode::kUYVY) {
        cur = ConvertUYVYToBGR(cur);
      } else if (cur->pixelFormat == VideoMode::kRGB565) {
        cur = ConvertRGB565ToBGR(cur);
      } else if (cur->pixelFormat == VideoMode::kGray) {
        if (pixelFormat == VideoMode::kBGR) {
          return ConvertGrayToBGR(cur);
        } else {
          return ConvertGrayToMJPEG(cur, defaultJpegQuality);
        }
      } else if (cur->pixelFormat == VideoMode::kY16) {
        // Check to see if Gray version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kGray)) {
          cur = newImage;
        } else {
          cur = ConvertY16ToGray(cur);
        }
        if (pixelFormat == VideoMode::kBGR) {
          return ConvertGrayToBGR(cur);
        } else {
          return ConvertGrayToMJPEG(cur, defaultJpegQuality);
        }
      }
      break;
    case VideoMode::kBGRA:
      // If source is RGB565, YUYV, UYVY, Gray or Y16, need to convert to BGR
      // first
      if (cur->pixelFormat == VideoMode::kRGB565) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertRGB565ToBGR(cur);
        }
      } else if (cur->pixelFormat == VideoMode::kYUYV) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertYUYVToBGR(cur);
        }
      } else if (cur->pixelFormat == VideoMode::kUYVY) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertUYVYToBGR(cur);
        }
      } else if (cur->pixelFormat == VideoMode::kGray) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else {
          cur = ConvertGrayToBGR(cur);
        }
      } else if (cur->pixelFormat == VideoMode::kY16) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR)) {
          cur = newImage;
        } else if (Image* newImage = GetExistingImage(cur->width, cur->height,
                                                      VideoMode::kGray)) {
          cur = ConvertGrayToBGR(newImage);
        } else {
          cur = ConvertGrayToBGR(ConvertY16ToGray(cur));
        }
      }
      return ConvertBGRToBGRA(cur);
    case VideoMode::kYUYV:
    case VideoMode::kUYVY:
    default:
      return nullptr;  // Unsupported
  }

  // Compress if destination is JPEG
  if (pixelFormat == VideoMode::kMJPEG) {
    cur = ConvertBGRToMJPEG(cur, defaultJpegQuality);
  }

  return cur;
}

Image* Frame::ConvertMJPEGToBGR(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kMJPEG) {
    return nullptr;
  }

  // Allocate an BGR image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kBGR, image->width, image->height,
                                image->width * image->height * 3);

  // Decode
  cv::Mat newMat = newImage->AsMat();
  cv::imdecode(image->AsInputArray(), cv::IMREAD_COLOR, &newMat);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertMJPEGToGray(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kMJPEG) {
    return nullptr;
  }

  // Allocate an grayscale image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kGray, image->width, image->height,
                                image->width * image->height);

  // Decode
  cv::Mat newMat = newImage->AsMat();
  cv::imdecode(image->AsInputArray(), cv::IMREAD_GRAYSCALE, &newMat);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertYUYVToBGR(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kYUYV) {
    return nullptr;
  }

  // Allocate a BGR image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kBGR, image->width, image->height,
                                image->width * image->height * 3);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_YUV2BGR_YUYV);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertYUYVToGray(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kYUYV) {
    return nullptr;
  }

  // Allocate a grayscale image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kGray, image->width, image->height,
                                image->width * image->height);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_YUV2GRAY_YUYV);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertUYVYToBGR(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kUYVY) {
    return nullptr;
  }

  // Allocate a BGR image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kBGR, image->width, image->height,
                                image->width * image->height * 3);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_YUV2BGR_UYVY);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertUYVYToGray(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kUYVY) {
    return nullptr;
  }

  // Allocate a grayscale image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kGray, image->width, image->height,
                                image->width * image->height);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_YUV2GRAY_UYVY);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertBGRToRGB565(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kBGR) {
    return nullptr;
  }

  // Allocate a RGB565 image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kRGB565, image->width, image->height,
                                image->width * image->height * 2);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_RGB2BGR565);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertRGB565ToBGR(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kRGB565) {
    return nullptr;
  }

  // Allocate a BGR image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kBGR, image->width, image->height,
                                image->width * image->height * 3);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_BGR5652RGB);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertBGRToGray(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kBGR) {
    return nullptr;
  }

  // Allocate a Grayscale image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kGray, image->width, image->height,
                                image->width * image->height);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_BGR2GRAY);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertGrayToBGR(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kGray) {
    return nullptr;
  }

  // Allocate a BGR image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kBGR, image->width, image->height,
                                image->width * image->height * 3);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_GRAY2BGR);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertBGRToMJPEG(Image* image, int quality) {
  if (!image || image->pixelFormat != VideoMode::kBGR) {
    return nullptr;
  }
  if (!m_impl) {
    return nullptr;
  }
  std::scoped_lock lock(m_impl->mutex);

  // Allocate a JPEG image.  We don't actually know what the resulting size
  // will be; while the destination will automatically grow, doing so will
  // cause an extra malloc, so we don't want to be too conservative here.
  // Per Wikipedia, Q=100 on a sample image results in 8.25 bits per pixel,
  // this is a little bit more conservative in assuming 50% space savings over
  // the equivalent BGR image.
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kMJPEG, image->width, image->height,
                                image->width * image->height * 1.5);

  // Compress
  if (m_impl->compressionParams.empty()) {
    m_impl->compressionParams.push_back(cv::IMWRITE_JPEG_QUALITY);
    m_impl->compressionParams.push_back(quality);
  } else {
    m_impl->compressionParams[1] = quality;
  }
  cv::imencode(".jpg", image->AsMat(), newImage->vec(),
               m_impl->compressionParams);

  // Save the result
  Image* rv = newImage.release();
  m_impl->images.push_back(rv);
  return rv;
}

Image* Frame::ConvertGrayToMJPEG(Image* image, int quality) {
  if (!image || image->pixelFormat != VideoMode::kGray) {
    return nullptr;
  }
  if (!m_impl) {
    return nullptr;
  }
  std::scoped_lock lock(m_impl->mutex);

  // Allocate a JPEG image.  We don't actually know what the resulting size
  // will be; while the destination will automatically grow, doing so will
  // cause an extra malloc, so we don't want to be too conservative here.
  // Per Wikipedia, Q=100 on a sample image results in 8.25 bits per pixel,
  // this is a little bit more conservative in assuming 25% space savings over
  // the equivalent grayscale image.
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kMJPEG, image->width, image->height,
                                image->width * image->height * 0.75);

  // Compress
  if (m_impl->compressionParams.empty()) {
    m_impl->compressionParams.push_back(cv::IMWRITE_JPEG_QUALITY);
    m_impl->compressionParams.push_back(quality);
  } else {
    m_impl->compressionParams[1] = quality;
  }
  cv::imencode(".jpg", image->AsMat(), newImage->vec(),
               m_impl->compressionParams);

  // Save the result
  Image* rv = newImage.release();
  m_impl->images.push_back(rv);
  return rv;
}

Image* Frame::ConvertGrayToY16(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kGray) {
    return nullptr;
  }

  // Allocate a Y16 image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kY16, image->width, image->height,
                                image->width * image->height * 2);

  // Convert with linear scaling
  image->AsMat().convertTo(newImage->AsMat(), CV_16U, 256);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertY16ToGray(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kY16) {
    return nullptr;
  }

  // Allocate a Grayscale image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kGray, image->width, image->height,
                                image->width * image->height);

  // Scale min to 0 and max to 255
  cv::normalize(image->AsMat(), newImage->AsMat(), 255, 0, cv::NORM_MINMAX);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::ConvertBGRToBGRA(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kBGR) {
    return nullptr;
  }

  // Allocate a RGB565 image
  auto newImage =
      m_impl->source.AllocImage(VideoMode::kBGRA, image->width, image->height,
                                image->width * image->height * 4);

  // Convert
  cv::cvtColor(image->AsMat(), newImage->AsMat(), cv::COLOR_BGR2BGRA);

  // Save the result
  Image* rv = newImage.release();
  if (m_impl) {
    std::scoped_lock lock(m_impl->mutex);
    m_impl->images.push_back(rv);
  }
  return rv;
}

Image* Frame::GetImageImpl(int width, int height,
                           VideoMode::PixelFormat pixelFormat,
                           int requiredJpegQuality, int defaultJpegQuality) {
  if (!m_impl) {
    return nullptr;
  }
  std::scoped_lock lock(m_impl->mutex);
  Image* cur = GetNearestImage(width, height, pixelFormat, requiredJpegQuality);
  if (!cur || cur->Is(width, height, pixelFormat, requiredJpegQuality)) {
    return cur;
  }

  WPI_DEBUG4(Instance::GetInstance().logger,
             "converting image from {}x{} type {} to {}x{} type {}", cur->width,
             cur->height, static_cast<int>(cur->pixelFormat), width, height,
             static_cast<int>(pixelFormat));

  // If the source image is a JPEG, we need to decode it before we can do
  // anything else with it.  Note that if the destination format is JPEG, we
  // still need to do this (unless the width/height/compression were the same,
  // in which case we already returned the existing JPEG above).
  if (cur->pixelFormat == VideoMode::kMJPEG) {
    cur = ConvertMJPEGToBGR(cur);
  }

  // Resize
  if (!cur->Is(width, height)) {
    // Allocate an image.
    auto newImage = m_impl->source.AllocImage(
        cur->pixelFormat, width, height,
        width * height * (cur->size() / (cur->width * cur->height)));

    // Resize
    cv::Mat newMat = newImage->AsMat();
    cv::resize(cur->AsMat(), newMat, newMat.size(), 0, 0);

    // Save the result
    cur = newImage.release();
    m_impl->images.push_back(cur);
  }

  // Convert to output format
  return ConvertImpl(cur, pixelFormat, requiredJpegQuality, defaultJpegQuality);
}

bool Frame::GetCv(cv::Mat& image, int width, int height,
                  VideoMode::PixelFormat pixelFormat) {
  Image* rawImage = GetImage(width, height, pixelFormat);
  if (!rawImage) {
    return false;
  }
  rawImage->AsMat().copyTo(image);
  return true;
}

void Frame::ReleaseFrame() {
  for (auto image : m_impl->images) {
    m_impl->source.ReleaseImage(std::unique_ptr<Image>(image));
  }
  m_impl->images.clear();
  m_impl->source.ReleaseFrameImpl(std::unique_ptr<Impl>(m_impl));
  m_impl = nullptr;
}

namespace cs {
std::unique_ptr<Image> CreateImageFromBGRA(cs::SourceImpl* source, size_t width,
                                           size_t height, size_t stride,
                                           const uint8_t* data) {
  cv::Mat finalImage{static_cast<int>(height), static_cast<int>(width), CV_8UC4,
                     const_cast<uint8_t*>(data), stride};
  std::unique_ptr<Image> dest = source->AllocImage(
      VideoMode::PixelFormat::kBGR, width, height, width * height * 3);
  cv::cvtColor(finalImage, dest->AsMat(), cv::COLOR_BGRA2BGR);
  return dest;
}
}  // namespace cs
