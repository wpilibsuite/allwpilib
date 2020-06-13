/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Frame.h"

#include <cstdlib>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}  // extern "C"

#include "FramePool.h"
#include "Instance.h"
#include "Log.h"

using namespace cs;

Frame::Frame(FramePool& framePool, const wpi::Twine& error, Time time)
    : m_impl{framePool.AllocFrameImpl().release()} {
  m_impl->refcount = 1;
  m_impl->error = error.str();
  m_impl->time = time;
}

Frame::Frame(FramePool& framePool, std::unique_ptr<Image> image, Time time)
    : m_impl{framePool.AllocFrameImpl().release()} {
  m_impl->refcount = 1;
  m_impl->error.resize(0);
  m_impl->time = time;
  m_impl->images.push_back(image.release());
}

Image* Frame::GetNearestImage(int width, int height) const {
  if (!m_impl) return nullptr;
  std::scoped_lock lock(m_impl->mutex);
  Image* found = nullptr;

  // Ideally we want the smallest image at least width/height in size
  for (auto i : m_impl->images) {
    if (i->IsLarger(width, height) && (!found || (i->IsSmaller(*found))))
      found = i;
  }
  if (found) return found;

  // Find the largest image (will be less than width/height)
  for (auto i : m_impl->images) {
    if (!found || (i->IsLarger(*found))) found = i;
  }
  if (found) return found;

  // Shouldn't reach this, but just in case...
  return m_impl->images.empty() ? nullptr : m_impl->images[0];
}

Image* Frame::GetNearestImage(
    int width, int height, VideoMode::PixelFormat pixelFormat,
    const CompressionContext::CompressionSettings& compressionSettings) const {
  if (!m_impl) return nullptr;
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
    if (i->Is(width, height, pixelFormat, compressionSettings)) return i;
  }

  // 2) Same width, height, different (but non-JPEG) pixelFormat (color conv)
  // 2a) If we want JPEG output, prefer BGR over other pixel formats
  if (pixelFormat == VideoMode::kMJPEG) {
    for (auto i : m_impl->images) {
      if (i->Is(width, height, VideoMode::kBGR)) return i;
    }
  }

  for (auto i : m_impl->images) {
    if (i->Is(width, height) && i->pixelFormat != VideoMode::kMJPEG) return i;
  }

  // 3) Different width, height, same pixelFormat (only if non-JPEG) (resample)
  if (pixelFormat != VideoMode::kMJPEG) {
    // 3a) Smallest image at least width/height in size
    for (auto i : m_impl->images) {
      if (i->IsLarger(width, height) && i->pixelFormat == pixelFormat &&
          (!found || (i->IsSmaller(*found))))
        found = i;
    }
    if (found) return found;

    // 3b) Largest image (less than width/height)
    for (auto i : m_impl->images) {
      if (i->pixelFormat == pixelFormat && (!found || (i->IsLarger(*found))))
        found = i;
    }
    if (found) return found;
  }

  // 4) Different width, height, different (but non-JPEG) pixelFormat
  //    (color conversion + resample)
  // 4a) Smallest image at least width/height in size
  for (auto i : m_impl->images) {
    if (i->IsLarger(width, height) && i->pixelFormat != VideoMode::kMJPEG &&
        (!found || (i->IsSmaller(*found))))
      found = i;
  }
  if (found) return found;

  // 4b) Largest image (less than width/height)
  for (auto i : m_impl->images) {
    if (i->pixelFormat != VideoMode::kMJPEG &&
        (!found || (i->IsLarger(*found))))
      found = i;
  }
  if (found) return found;

  // 5) Same width, height, JPEG pixelFormat (decompression).  As there may be
  //    multiple JPEG images, find the highest quality one.
  for (auto i : m_impl->images) {
    if (i->Is(width, height, VideoMode::kMJPEG) &&
        (!found || i->jpegQuality > found->jpegQuality)) {
      found = i;
      // consider one without a quality setting to be the highest quality
      // (e.g. directly from the camera)
      if (i->jpegQuality == -1) break;
    }
  }
  if (found) return found;

  // 6) Different width, height, JPEG pixelFormat (decompression)
  // 6a) Smallest image at least width/height in size
  for (auto i : m_impl->images) {
    if (i->IsLarger(width, height) && i->pixelFormat == VideoMode::kMJPEG &&
        (!found || (i->IsSmaller(*found))))
      found = i;
  }
  if (found) return found;

  // 6b) Largest image (less than width/height)
  for (auto i : m_impl->images) {
    if (i->pixelFormat != VideoMode::kMJPEG &&
        (!found || (i->IsLarger(*found))))
      found = i;
  }
  if (found) return found;

  // Shouldn't reach this, but just in case...
  return m_impl->images.empty() ? nullptr : m_impl->images[0];
}

Image* Frame::Convert(Image* image, VideoMode::PixelFormat pixelFormat,
                      const CompressionContext& compressionContext) {
  if (!image || image->Is(image->width, image->height, pixelFormat,
                          compressionContext.GetCompressionSettings()))
    return image;
  Image* cur = image;

  // If the source image is compressed, we need to decode it before we can do
  // anything else with it.  Note that if the destination format is compressed,
  // we still need to do this (unless it was already a compressed, in which
  // case we would have returned above).
  if (cur->pixelFormat == VideoMode::kMJPEG) {
    cur = ConvertMJPEGToBGR(cur);
    if (pixelFormat == VideoMode::kBGR) return cur;
  } else if (cur->pixelFormat == VideoMode::kH264) {
    cur = ConvertH264ToBGR(cur, compressionContext);
    if (pixelFormat == VideoMode::kBGR) return cur;
  }

  // Color convert
  switch (pixelFormat) {
    case VideoMode::kRGB565:
      // If source is YUYV or Gray, need to convert to BGR first
      if (cur->pixelFormat == VideoMode::kYUYV) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR))
          cur = newImage;
        else
          cur = ConvertYUYVToBGR(cur);
      } else if (cur->pixelFormat == VideoMode::kGray) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR))
          cur = newImage;
        else
          cur = ConvertGrayToBGR(cur);
      }
      return ConvertBGRToRGB565(cur);
    case VideoMode::kGray:
      // If source is YUYV or RGB565, need to convert to BGR first
      if (cur->pixelFormat == VideoMode::kYUYV) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR))
          cur = newImage;
        else
          cur = ConvertYUYVToBGR(cur);
      } else if (cur->pixelFormat == VideoMode::kRGB565) {
        // Check to see if BGR version already exists...
        if (Image* newImage =
                GetExistingImage(cur->width, cur->height, VideoMode::kBGR))
          cur = newImage;
        else
          cur = ConvertRGB565ToBGR(cur);
      }
      return ConvertBGRToGray(cur);
    case VideoMode::kBGR:
    case VideoMode::kMJPEG:
    case VideoMode::kH264:
      if (cur->pixelFormat == VideoMode::kYUYV) {
        cur = ConvertYUYVToBGR(cur);
      } else if (cur->pixelFormat == VideoMode::kRGB565) {
        cur = ConvertRGB565ToBGR(cur);
      } else if (cur->pixelFormat == VideoMode::kGray) {
        // We never get to this branch in a compressed source video mode
        // because we convert cur from compressed to BGR in the beginning of
        // this function.
        if (pixelFormat == VideoMode::kBGR) {
          return ConvertGrayToBGR(cur);
        } else if (pixelFormat == VideoMode::kMJPEG) {
          return ConvertGrayToMJPEG(
              cur,
              compressionContext.GetCompressionSettings().jpegDefaultQuality);
        } else if (pixelFormat == VideoMode::kH264) {
          cur = ConvertGrayToBGR(cur);
          return ConvertBGRToH264(cur, compressionContext);
        }
      }
      break;
    case VideoMode::kYUYV:
    default:
      return nullptr;  // Unsupported
  }

  // Compress if destination is JPEG or H264
  if (pixelFormat == VideoMode::kMJPEG)
    cur = ConvertBGRToMJPEG(
        cur, compressionContext.GetCompressionSettings().jpegDefaultQuality);
  else if (pixelFormat == VideoMode::kH264)
    cur = ConvertBGRToH264(cur, compressionContext);

  return cur;
}

Image* Frame::ConvertH264ToBGR(Image* image,
                               const CompressionContext& compressionContext) {
  if (!image || !m_impl || image->pixelFormat != VideoMode::kH264)
    return nullptr;
  std::scoped_lock lock(m_impl->mutex);

  // Allocate an BGR image
  auto newImage =
      m_impl->framePool.AllocImage(VideoMode::kBGR, image->width, image->height,
                                   image->width * image->height * 3);

  auto compressionCtx = compressionContext.GetH264Context(
      image->width, image->height, 0);  // TODO: Pipe in FPS

  int ret;

  // Send raw H264-encoded bytes to libavcodec
  ret = avcodec_send_packet(compressionCtx.decodingContext,
                            compressionCtx.packet);
  if (ret < 0) {
    WPI_WARNING(Instance::GetInstance().GetLogger(),
                "Couldn't send H264 packet to be decoded by libavcodec");
    return nullptr;
  }

  // Get decoded bytes back
  while (ret >= 0) {
    ret = avcodec_receive_frame(compressionCtx.decodingContext,
                                compressionCtx.frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      return nullptr;  // TODO: Is this the appropriate response when we reach
                       // the end of the image?
    }

    newImage->vec().insert(
        newImage->vec().end(), compressionCtx.packet->data,
        compressionCtx.packet->data + compressionCtx.packet->size);
  }
  // TODO: Probably need to do some more work on handling bad packets here

  // Save the result
  Image* rv = newImage.release();
  m_impl->images.push_back(rv);
  return rv;
}

Image* Frame::ConvertMJPEGToBGR(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kMJPEG) return nullptr;

  // Allocate an BGR image
  auto newImage =
      m_impl->framePool.AllocImage(VideoMode::kBGR, image->width, image->height,
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

Image* Frame::ConvertYUYVToBGR(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kYUYV) return nullptr;

  // Allocate a BGR image
  auto newImage =
      m_impl->framePool.AllocImage(VideoMode::kBGR, image->width, image->height,
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

Image* Frame::ConvertBGRToRGB565(Image* image) {
  if (!image || image->pixelFormat != VideoMode::kBGR) return nullptr;

  // Allocate a RGB565 image
  auto newImage = m_impl->framePool.AllocImage(
      VideoMode::kRGB565, image->width, image->height,
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
  if (!image || image->pixelFormat != VideoMode::kRGB565) return nullptr;

  // Allocate a BGR image
  auto newImage =
      m_impl->framePool.AllocImage(VideoMode::kBGR, image->width, image->height,
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
  if (!image || image->pixelFormat != VideoMode::kBGR) return nullptr;

  // Allocate a Grayscale image
  auto newImage =
      m_impl->framePool.AllocImage(VideoMode::kGray, image->width,
                                   image->height, image->width * image->height);

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
  if (!image || image->pixelFormat != VideoMode::kGray) return nullptr;

  // Allocate a BGR image
  auto newImage =
      m_impl->framePool.AllocImage(VideoMode::kBGR, image->width, image->height,
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
  if (!image || image->pixelFormat != VideoMode::kBGR) return nullptr;
  if (!m_impl) return nullptr;
  std::scoped_lock lock(m_impl->mutex);

  // Allocate a JPEG image.  We don't actually know what the resulting size
  // will be; while the destination will automatically grow, doing so will
  // cause an extra malloc, so we don't want to be too conservative here.
  // Per Wikipedia, Q=100 on a sample image results in 8.25 bits per pixel,
  // this is a little bit more conservative in assuming 50% space savings over
  // the equivalent BGR image.
  auto newImage = m_impl->framePool.AllocImage(
      VideoMode::kMJPEG, image->width, image->height,
      image->width * image->height * 1.5);

  // Compress
  if (m_impl->mjpegParams.empty()) {
    m_impl->mjpegParams.push_back(cv::IMWRITE_JPEG_QUALITY);
    m_impl->mjpegParams.push_back(quality);
  } else {
    m_impl->mjpegParams[1] = quality;
  }
  cv::imencode(".jpg", image->AsMat(), newImage->vec(), m_impl->mjpegParams);

  // Save the result
  Image* rv = newImage.release();
  rv->jpegQuality = quality;
  m_impl->images.push_back(rv);
  return rv;
}

Image* Frame::ConvertGrayToMJPEG(Image* image, int quality) {
  if (!image || image->pixelFormat != VideoMode::kGray) return nullptr;
  if (!m_impl) return nullptr;
  std::scoped_lock lock(m_impl->mutex);

  // Allocate a JPEG image.  We don't actually know what the resulting size
  // will be; while the destination will automatically grow, doing so will
  // cause an extra malloc, so we don't want to be too conservative here.
  // Per Wikipedia, Q=100 on a sample image results in 8.25 bits per pixel,
  // this is a little bit more conservative in assuming 25% space savings over
  // the equivalent grayscale image.
  auto newImage = m_impl->framePool.AllocImage(
      VideoMode::kMJPEG, image->width, image->height,
      image->width * image->height * 0.75);

  // Compress
  if (m_impl->mjpegParams.empty()) {
    m_impl->mjpegParams.push_back(cv::IMWRITE_JPEG_QUALITY);
    m_impl->mjpegParams.push_back(quality);
  } else {
    m_impl->mjpegParams[1] = quality;
  }
  cv::imencode(".jpg", image->AsMat(), newImage->vec(), m_impl->mjpegParams);

  // Save the result
  Image* rv = newImage.release();
  rv->jpegQuality = quality;
  m_impl->images.push_back(rv);
  return rv;
}

Image* Frame::ConvertBGRToH264(Image* image,
                               const CompressionContext& compressionContext) {
  if (!m_impl || image->pixelFormat != VideoMode::kBGR) return nullptr;
  std::scoped_lock lock(m_impl->mutex);

  // TODO: Figure out a good conservative size estimate; right now we assume
  // compression is probably no worse than JPEG.
  auto newImage = m_impl->framePool.AllocImage(
      VideoMode::kH264, image->width, image->height,
      image->width * image->height * 0.75);

  auto h264Ctx = compressionContext.GetH264Context(image->width, image->height,
                                                   0);  // TODO: Pipe in FPS
  h264Ctx.frame->data[0] = reinterpret_cast<uint8_t*>(image->data());

  int ret;

  // Send raw BGR image data to libavcodec
  ret = avcodec_send_frame(h264Ctx.encodingContext, h264Ctx.frame);
  if (ret < 0) {
    WPI_WARNING(Instance::GetInstance().GetLogger(),
                "Couldn't send frame to be encoded by libavcodec");
    return nullptr;
  }

  // Get back H264-encoded bytes
  while (ret >= 0) {
    ret = avcodec_receive_packet(h264Ctx.encodingContext, h264Ctx.packet);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      // We're done encoding the frame we just sent... Usually we only have to
      // go through the loop once to get to this point.
      Image* rv = newImage.release();
      m_impl->images.push_back(rv);
      return rv;
    } else if (ret < 0) {
      WPI_WARNING(Instance::GetInstance().GetLogger(),
                  "Couldn't receive encoded packet from libavcodec");
      return nullptr;
    }

    newImage->vec().insert(newImage->vec().end(), h264Ctx.packet->data,
                           h264Ctx.packet->data + h264Ctx.packet->size);
  }

  // Save the result
  Image* rv = newImage.release();
  rv->h264Bitrate = compressionContext.GetCompressionSettings().h264Bitrate;
  m_impl->images.push_back(rv);
  return rv;
}

Image* Frame::GetImage(int width, int height,
                       VideoMode::PixelFormat pixelFormat,
                       const CompressionContext& compressionContext) {
  if (!m_impl) return nullptr;
  std::scoped_lock lock(m_impl->mutex);
  Image* cur = GetNearestImage(width, height, pixelFormat,
                               compressionContext.GetCompressionSettings());
  if (!cur || cur->Is(width, height, pixelFormat,
                      compressionContext.GetCompressionSettings()))
    return cur;

  WPI_DEBUG4(Instance::GetInstance().GetLogger(),
             "converting image from " << cur->width << "x" << cur->height
                                      << " type " << cur->pixelFormat << " to "
                                      << width << "x" << height << " type "
                                      << pixelFormat);

  // If the source image is compressed, we need to decode it before we can do
  // anything else with it.  Note that if the destination format is compressed,
  // still need to do this (unless the width/height/compression were the same,
  // in which case we already returned the existing compressed image above).
  if (cur->pixelFormat == VideoMode::kMJPEG)
    cur = ConvertMJPEGToBGR(cur);
  else if (cur->pixelFormat == VideoMode::kH264)
    cur = ConvertH264ToBGR(cur, compressionContext);

  // It's possible that we passed a malformed frame to be decoded
  if (!cur) return nullptr;

  // Resize
  if (!cur->Is(width, height)) {
    // Allocate an image.
    auto newImage = m_impl->framePool.AllocImage(
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
  return Convert(cur, pixelFormat, compressionContext);
}

bool Frame::GetCv(cv::Mat& image, int width, int height,
                  const CompressionContext& compressionContext) {
  Image* rawImage =
      GetImage(width, height, VideoMode::kBGR, compressionContext);
  if (!rawImage) return false;
  rawImage->AsMat().copyTo(image);
  return true;
}

void Frame::ReleaseFrame() {
  m_impl->framePool.ReleaseFrameImpl(std::unique_ptr<Impl>(m_impl));
  m_impl = nullptr;
}
