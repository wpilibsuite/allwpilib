/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_COMPRESSIONCONTEXT_H_
#define CSCORE_COMPRESSIONCONTEXT_H_

#include <wpi/SmallVector.h>

extern "C" {
    #include <libavcodec/avcodec.h>

    #include <libavutil/opt.h>
};

namespace cs {

class CompressionContext {
public:
    struct H264Context {
        // Stores encoder settings and data
        AVCodecContext* encodingContext;
        // Stores decoder settings and data
        AVCodecContext* decodingContext;

        // Parser for decoding
        AVCodecParserContext* parser;

        // Both frame and packet have contents that change per-frame... We keep them here so we don't allocate new ones every frame.
        // If we're encoding, frame stores the raw input image in our given pix_fmt (currently BGR24)
        // If we're decoding, frame stores the raw output image
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

public:
    CompressionContext(int h264BitRate = 40000, int mjpegRequiredQuality = -1, int mjpegDefaultQuality = 80);

    void SetH264BitRate(int64_t bitrate) { m_h264BitRate = bitrate; }

    void SetMJPEGQuality(int requiredQuality, int defaultQuality) {
        m_mjpegRequiredQuality = requiredQuality;
        m_mjpegDefaultQuality = defaultQuality;
    }

    const H264Context& GetH264Context(int width, int height, int fps) const;

private:
    int m_mjpegRequiredQuality;
    int m_mjpegDefaultQuality;

    int64_t m_h264BitRate;
    static const AVCodec* m_h264EncodingCodec;
    static const AVCodec* m_h264DecodingCodec;
    wpi::SmallVector<H264Context, 4> m_h264Contexts;
};

}  // namespace cs

#endif  // CSCORE_COMPRESSIONCONTEXT_H_