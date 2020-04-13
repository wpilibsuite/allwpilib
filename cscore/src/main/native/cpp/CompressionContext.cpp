/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CompressionContext.h"

using namespace cs;

CompressionContext::CompressionContext(int h264BitRate, int mjpgRequiredQuality, int mjpgDefaultQuality) :
        mjpegRequiredQuality{mjpegRequiredQuality},
        mjpegDefaultQuality{mjpegDefaultQuality},
        h264BitRate{h264BitRate} {
    // The OpenMAX codec is only built for Raspian (so that we can use the Pi GPU encoder)
    h264Codec = avcodec_find_encoder_by_name("h264_omx");
    if (!h264Codec) {
        // No hardware-accelerated encoding is available, so we will use "libx264rgb"
        // Note that the "libx264" encoder, which works in YUYV, is also built but not used at this time
        h264Codec = avcodec_find_encoder_by_name("libx264rgb");
        // XXX (for review): throw may be inappropriate here
        if (!h264Codec) throw std::runtime_error("No compatible H264 codecs found");
    }
}

CompressionContext::H264Context CompressionContext::GetH264Context(int width, int height) {
    H264Context ret;
    for (auto ctx : h264CodecContexts) {
        if (ctx.codecContext->width == width && ctx.codecContext->height == height) ret = ctx;
    }
    if (!ret.codecContext) {
        // XXX (for review): once again not sure if throw is appropriate here
        ret.codecContext = avcodec_alloc_context3(this->h264Codec);
        if (!ret.codecContext) throw std::runtime_error("Could not allocate video codec context");

        ret.codecContext->bit_rate = h264BitRate;

        ret.codecContext->width = width;
        ret.codecContext->height = height;

        // Set FPS
        // TODO: variable fps?
        ret.codecContext->time_base = AVRational{1, 25};
        ret.codecContext->framerate = AVRational{25, 1};

        // Emit one intra frame every ten frames
        ret.codecContext->gop_size = 10;
        // Max number of B-frames between non-B-frames
        // Only important thing for us is that the output is delayed by this number of frames+1
        ret.codecContext->max_b_frames = 1;
        // 24-bit (total) BGR is always what we're going to be converting from
        // TODO: we're using libx264rgb so is bgr->rgb going to be a performance issue?
        ret.codecContext->pix_fmt = AV_PIX_FMT_BGR24;

        // Optimize encoding for minimal latency
        // TODO: do we need any other options set?
        av_opt_set(ret.codecContext->priv_data, "tune", "zerolatency", 0);
        av_opt_set(ret.codecContext->priv_data, "preset", "ultrafast", 0);

        int err = avcodec_open2(ret.codecContext, h264Codec, nullptr);
        if (err < 0) throw std::runtime_error("Couldn't open codec");

        ret.frame = av_frame_alloc();
        if (!ret.frame) throw std::runtime_error("Couldn't allocate an input video frame");
        err = av_frame_get_buffer(ret.frame, 32);
        if (err < 0) throw std::runtime_error("Couldn't allocate input video frame data");

        ret.packet = av_packet_alloc();
        if (!ret.packet) throw std::runtime_error("Couldn't allocate an output video packet");
    }

    return ret;
}