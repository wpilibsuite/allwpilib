/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_COMPRESSIONCONTEXT_H_
#define CSCORE_COMPRESSIONCONTEXT_H_

#include <wpi/SmallVector.h>

#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>

namespace cs {

class CompressionContext {
public:
    struct H264Context {
        // Stores encoder settings and data
        AVCodecContext* codecContext;

        // Both frame and packet have contents that change per-frame... We keep them here so we don't allocate new ones every frame.
        // Frame stores the raw input image in our pix_fmt (currently BGR24)
        AVFrame* frame;
        // Packet stores output bytes from the encoder
        AVPacket* packet;

        ~H264Context() {
            if (codecContext != nullptr) avcodec_free_context(&codecContext);
            if (frame != nullptr) av_frame_free(&frame);
            if (packet != nullptr) av_packet_free(&packet);
        }
    };

public:
    const int mjpegRequiredQuality;
    const int mjpegDefaultQuality;

    const int64_t h264BitRate;

    CompressionContext(int h264BitRate = 40000, int mjpgRequiredQual = -1, int mjpgDefaultQuality = 80);

    H264Context GetH264Context(int width, int height);

private:
    const AVCodec* h264Codec;
    wpi::SmallVector<H264Context, 4> h264CodecContexts;
};

}  // namespace cs

#endif  // CSCORE_COMPRESSIONCONTEXT_H_