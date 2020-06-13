/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_COMPRESSIONCONTEXT_H_
#define CSCORE_COMPRESSIONCONTEXT_H_

#include <wpi/SmallVector.h>
#include <wpi/mutex.h>

extern "C" {
#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
}  // extern "C"

namespace cs {

class CompressionContext {
 public:
  // We put structs for holding current encoding information here
  // Note that there is no MJPEGContext because MJPEG does no interframe
  // compression (i.e. it is "contextless".)
  struct H264Context {
    // Stores encoder settings and data
    AVCodecContext* encodingContext;
    // Stores decoder settings and data
    AVCodecContext* decodingContext;

    // Parser for decoding
    AVCodecParserContext* parser;

    // Both frame and packet have contents that change per-frame... We keep them
    // here so we don't allocate new ones every frame. If we're encoding, frame
    // stores the raw input image in our given pix_fmt (currently BGR24) If
    // we're decoding, frame stores the raw output image
    AVFrame* frame;
    // Packet stores output bytes from the encoder
    AVPacket* packet;

    ~H264Context() {
      if (encodingContext != nullptr) avcodec_free_context(&encodingContext);
      if (decodingContext != nullptr) avcodec_free_context(&decodingContext);
      if (frame != nullptr) av_frame_free(&frame);
      if (packet != nullptr) av_packet_free(&packet);
    }
  };

  struct CompressionSettings {
    int jpegRequiredQuality;
    int jpegDefaultQuality;

    int64_t h264Bitrate;
  };

  static constexpr CompressionSettings defaultCompressionSettings{-1, 80,
                                                                  40000};

 public:
  CompressionContext();

  const H264Context& GetH264Context(int width, int height, int fps) const;

  void SetCompressionSettings(const CompressionSettings settings) {
    std::lock_guard lock{m_mutex};
    m_compressionSettings = settings;
  }
  const CompressionSettings& GetCompressionSettings() const {
    std::lock_guard lock{m_mutex};
    return m_compressionSettings;
  }

 private:
  mutable wpi::mutex m_mutex;

  CompressionSettings m_compressionSettings;

  static AVCodec* m_h264EncodingCodec;
  static AVCodec* m_h264DecodingCodec;
  wpi::SmallVector<H264Context, 4> m_h264Contexts;
};

}  // namespace cs

#endif  // CSCORE_COMPRESSIONCONTEXT_H_
