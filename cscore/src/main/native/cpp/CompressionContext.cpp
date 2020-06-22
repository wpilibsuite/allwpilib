/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CompressionContext.h"

using namespace cs;

CompressionContext::CompressionContext()
    : m_compressionSettings{defaultCompressionSettings} {
  if (m_h264EncodingCodec && m_h264DecodingCodec) return;

  // The OpenMAX codec is only built for Raspian (so that we can use the Pi GPU
  // encoder)
  m_h264EncodingCodec = avcodec_find_encoder_by_name("h264_omx");
  if (!m_h264EncodingCodec) {
    // No hardware-accelerated encoding is available, so we will use
    // "libx264rgb" Note that the "libx264" encoder, which works in YUYV, is
    // also built but not used at this time
    m_h264EncodingCodec = avcodec_find_encoder_by_name("libx264rgb");
    // TODO (for review): throw may be inappropriate here
    if (!m_h264EncodingCodec)
      throw std::runtime_error("No compatible H264 codecs for encoding found");
  }

  // Same situation as above
  m_h264DecodingCodec = avcodec_find_decoder_by_name("h264_omx");
  if (!m_h264DecodingCodec) {
    m_h264DecodingCodec = avcodec_find_decoder_by_name("libx264rgb");
    if (!m_h264DecodingCodec)
      throw std::runtime_error("No compatible H264 codecs for decoding found");
  }
}

const CompressionContext::H264Context& CompressionContext::GetH264Context(
    int width, int height, int fps) const {
  std::lock_guard lock{m_mutex};

  H264Context ret;
  for (auto ctx : m_h264Contexts) {
    // We only return a premade context if its width, height, fps, and bitrate
    // match This means that the user changing settings to many different values
    // will result in tons of different contexts
    if (ctx.encodingContext->width == width &&
        ctx.encodingContext->height == height &&
        ctx.encodingContext->framerate.den == fps &&
        ctx.encodingContext->bit_rate == m_compressionSettings.h264Bitrate)
      ret = ctx;
  }
  if (!ret.encodingContext) {
    // Set up encoding
    // TODO (for review): once again not sure if throw is appropriate here
    ret.encodingContext = avcodec_alloc_context3(this->m_h264EncodingCodec);
    if (!ret.encodingContext)
      throw std::runtime_error("Could not allocate encoding codec context");

    ret.encodingContext->bit_rate = m_compressionSettings.h264Bitrate;

    ret.encodingContext->width = width;
    ret.encodingContext->height = height;

    // Set FPS
    // TODO: Framerate is probably a little variable... Is this a problem?
    ret.encodingContext->time_base = AVRational{1, fps};
    ret.encodingContext->framerate = AVRational{fps, 1};

    // Emit one intra frame every ten frames
    ret.encodingContext->gop_size = 10;
    // Max number of B-frames between non-B-frames
    // Only important thing for us is that the output is delayed by this number
    // of frames+1
    ret.encodingContext->max_b_frames = 1;
    // 24-bit (total) BGR is always what we're going to be converting from
    // TODO: we're using libx264rgb so is bgr->rgb going to be a performance
    // issue? Does libavcodec even do this for us?
    ret.encodingContext->pix_fmt = AV_PIX_FMT_BGR24;

    // Optimize encoding for minimal latency
    // TODO: do we need any other options set for low latency?
    av_opt_set(ret.encodingContext->priv_data, "tune", "zerolatency", 0);
    av_opt_set(ret.encodingContext->priv_data, "preset", "ultrafast", 0);

    int err = avcodec_open2(ret.encodingContext, m_h264EncodingCodec, nullptr);
    if (err < 0) throw std::runtime_error("Couldn't open encoding codec");

    // Set up decoding
    ret.parser = av_parser_init(m_h264DecodingCodec->id);
    if (!ret.parser) throw std::runtime_error("Couldn't find decoding parser");

    ret.decodingContext = avcodec_alloc_context3(m_h264DecodingCodec);
    if (!ret.encodingContext)
      throw std::runtime_error("Could not allocate decoding codec context");

    err = avcodec_open2(ret.decodingContext, m_h264DecodingCodec, nullptr);
    if (err < 0) throw std::runtime_error("Couldn't open decoding codec");

    // Set up shared input and output buffers
    ret.frame = av_frame_alloc();
    if (!ret.frame)
      throw std::runtime_error("Couldn't allocate an input video frame");
    err = av_frame_get_buffer(ret.frame, 32);
    if (err < 0)
      throw std::runtime_error("Couldn't allocate input video frame data");

    ret.packet = av_packet_alloc();
    if (!ret.packet)
      throw std::runtime_error("Couldn't allocate an output video packet");
  }

  m_h264Contexts.emplace_back(std::move(ret));
  return m_h264Contexts.back();
}
