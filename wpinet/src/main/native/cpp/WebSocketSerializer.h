// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <utility>

#include <wpi/SmallVector.h>
#include <wpi/SpanExtras.h>

#include "WebSocketDebug.h"
#include "wpinet/WebSocket.h"
#include "wpinet/uv/Buffer.h"

namespace wpi::detail {

class SerializedFrames {
 public:
  SerializedFrames() = default;
  SerializedFrames(const SerializedFrames&) = delete;
  SerializedFrames& operator=(const SerializedFrames&) = delete;
  ~SerializedFrames() { ReleaseBufs(); }

  size_t AddFrame(const WebSocket::Frame& frame, bool server) {
    if (server) {
      return AddServerFrame(frame);
    } else {
      return AddClientFrame(frame);
    }
  }

  size_t AddClientFrame(const WebSocket::Frame& frame);
  size_t AddServerFrame(const WebSocket::Frame& frame);

  void ReleaseBufs() {
    for (auto&& buf : m_allocBufs) {
      buf.Deallocate();
    }
    m_allocBufs.clear();
  }

  SmallVector<uv::Buffer, 4> m_allocBufs;
  SmallVector<uv::Buffer, 4> m_bufs;
  size_t m_allocBufPos = 0;
};

class WebSocketWriteReqBase {
 public:
  template <typename Stream, typename Req>
  int Continue(Stream& stream, std::shared_ptr<Req> req);

  SmallVector<uv::Buffer, 4> m_userBufs;
  SerializedFrames m_frames;
  SmallVector<int, 0> m_continueFrameOffs;
  size_t m_continueBufPos = 0;
  size_t m_continueFramePos = 0;
};

template <typename Stream, typename Req>
int WebSocketWriteReqBase::Continue(Stream& stream, std::shared_ptr<Req> req) {
  if (m_continueBufPos >= m_frames.m_bufs.size()) {
    return 0;  // nothing more to send
  }

  // try writing everything remaining
  std::span bufs = std::span{m_frames.m_bufs}.subspan(m_continueBufPos);
  int numBytes = 0;
  for (auto&& buf : bufs) {
    numBytes += buf.len;
  }

  int sentBytes = stream.TryWrite(bufs);
  WS_DEBUG("TryWrite({}) -> {} (expected {})\n", bufs.size(), sentBytes,
           numBytes);
  if (sentBytes < 0) {
    return sentBytes;  // error
  }

  if (sentBytes == numBytes) {
    m_continueBufPos = m_frames.m_bufs.size();
    return 0;  // nothing more to send
  }

  // we didn't send everything; deal with the leftovers

  // figure out what the last (partially) frame sent actually was
  auto offIt = m_continueFrameOffs.begin() + m_continueFramePos;
  auto offEnd = m_continueFrameOffs.end();
  while (offIt != offEnd && *offIt < sentBytes) {
    ++offIt;
  }
  assert(offIt != offEnd);

  // build a list of buffers to send as a normal write:
  SmallVector<uv::Buffer, 4> writeBufs;
  auto bufIt = bufs.begin();
  auto bufEnd = bufs.end();

  // start with the remaining portion of the last buffer actually sent
  int pos = 0;
  while (bufIt != bufEnd && pos < sentBytes) {
    pos += (bufIt++)->len;
  }
  if (bufIt != bufs.begin() && pos != sentBytes) {
    writeBufs.emplace_back(
        wpi::take_back((bufIt - 1)->bytes(), pos - sentBytes));
  }

  // continue through the last buffer of the last partial frame
  while (bufIt != bufEnd && offIt != offEnd && pos < *offIt) {
    pos += bufIt->len;
    writeBufs.emplace_back(*bufIt++);
  }
  if (offIt != offEnd) {
    ++offIt;
  }

  // if writeBufs is still empty, write all of the next frame
  if (writeBufs.empty()) {
    while (bufIt != bufEnd && offIt != offEnd && pos < *offIt) {
      pos += bufIt->len;
      writeBufs.emplace_back(*bufIt++);
    }
    if (offIt != offEnd) {
      ++offIt;
    }
  }

  m_continueFramePos = offIt - m_continueFrameOffs.begin();
  m_continueBufPos = &(*bufIt) - &(*m_frames.m_bufs.begin());

  if (writeBufs.empty()) {
    WS_DEBUG("Write Done\n");
    return 0;
  }
  WS_DEBUG("Write({})\n", writeBufs.size());
  stream.Write(writeBufs, req);
  return 1;
}

template <typename MakeReq, typename Stream>
std::span<const WebSocket::Frame> TrySendFrames(
    bool server, Stream& stream, std::span<const WebSocket::Frame> frames,
    MakeReq&& makeReq,
    std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {
  WS_DEBUG("TrySendFrames({})\n", frames.size());
  auto frameIt = frames.begin();
  auto frameEnd = frames.end();
  while (frameIt != frameEnd) {
    auto frameStart = frameIt;

    // build buffers to send
    SerializedFrames sendFrames;
    SmallVector<int, 32> frameOffs;
    int numBytes = 0;
    while (frameIt != frameEnd) {
      frameOffs.emplace_back(numBytes);
      numBytes += sendFrames.AddFrame(*frameIt++, server);
      if ((server && (numBytes >= 65536 || frameOffs.size() > 32)) ||
          (!server && numBytes >= 8192)) {
        // don't waste too much memory or effort on header generation or masking
        break;
      }
    }

    // try to send
    int sentBytes = stream.TryWrite(sendFrames.m_bufs);
    WS_DEBUG("TryWrite({}) -> {} (expected {})\n", sendFrames.m_bufs.size(),
             sentBytes, numBytes);

    if (sentBytes == 0) {
      // we haven't started a frame yet; clean up any bufs that have actually
      // sent, and return unsent frames
      SmallVector<uv::Buffer, 4> bufs;
      for (auto it = frames.begin(); it != frameStart; ++it) {
        bufs.append(it->data.begin(), it->data.end());
      }
      callback(bufs, {});
      return {frameStart, frameEnd};
    } else if (sentBytes < 0) {
      // error
      SmallVector<uv::Buffer, 4> bufs;
      for (auto&& frame : frames) {
        bufs.append(frame.data.begin(), frame.data.end());
      }
      callback(bufs, uv::Error{sentBytes});
      return frames;
    } else if (sentBytes != numBytes) {
      // we didn't send everything; deal with the leftovers

      // figure out what the last (partially) frame sent actually was
      auto offIt = frameOffs.begin();
      auto offEnd = frameOffs.end();
      bool isFin = true;
      while (offIt != offEnd && *offIt < sentBytes) {
        ++offIt;
        isFin = (frameStart->opcode & WebSocket::kFlagFin) != 0;
        ++frameStart;
      }

      if (offIt != offEnd && *offIt == sentBytes && isFin) {
        // we finished at a normal FIN frame boundary; no need for a Write()
        SmallVector<uv::Buffer, 4> bufs;
        for (auto it = frames.begin(); it != frameStart; ++it) {
          bufs.append(it->data.begin(), it->data.end());
        }
        callback(bufs, {});
        return {frameStart, frameEnd};
      }

      // build a list of buffers to send as a normal write:
      SmallVector<uv::Buffer, 4> writeBufs;
      auto bufIt = sendFrames.m_bufs.begin();
      auto bufEnd = sendFrames.m_bufs.end();

      // start with the remaining portion of the last buffer actually sent
      int pos = 0;
      while (bufIt != bufEnd && pos < sentBytes) {
        pos += (bufIt++)->len;
      }
      if (bufIt != sendFrames.m_bufs.begin() && pos != sentBytes) {
        writeBufs.emplace_back(
            wpi::take_back((bufIt - 1)->bytes(), pos - sentBytes));
      }

      // continue through the last buffer of the last partial frame
      while (bufIt != bufEnd && offIt != offEnd && pos < *offIt) {
        pos += bufIt->len;
        writeBufs.emplace_back(*bufIt++);
      }
      if (offIt != offEnd) {
        ++offIt;
      }

      // move allocated buffers into request
      auto req = makeReq(std::move(callback));
      req->m_frames.m_allocBufs = std::move(sendFrames.m_allocBufs);
      req->m_frames.m_allocBufPos = sendFrames.m_allocBufPos;

      // if partial frame was non-FIN, put any additional non-FIN frames into
      // continuation (so the caller isn't responsible for doing this)
      size_t continuePos = 0;
      while (frameStart != frameEnd && !isFin) {
        req->m_continueFrameOffs.emplace_back(continuePos);
        if (offIt != offEnd) {
          // we already generated the wire buffers for this frame, use them
          while (pos < *offIt && bufIt != bufEnd) {
            pos += bufIt->len;
            continuePos += bufIt->len;
            req->m_frames.m_bufs.emplace_back(*bufIt++);
          }
          ++offIt;
        } else {
          // WS_DEBUG("generating frame for continuation {} {}\n",
          //          frameStart->opcode, frameStart->data.size());
          // need to generate and add this frame
          continuePos += req->m_frames.AddFrame(*frameStart, server);
        }
        isFin = (frameStart->opcode & WebSocket::kFlagFin) != 0;
        ++frameStart;
      }

      // only the non-returned user buffers are added to the request
      for (auto it = frames.begin(); it != frameStart; ++it) {
        req->m_userBufs.append(it->data.begin(), it->data.end());
      }

      WS_DEBUG("Write({})\n", writeBufs.size());
      stream.Write(writeBufs, req);
      return {frameStart, frameEnd};
    }
  }

  // nothing left to send
  SmallVector<uv::Buffer, 4> bufs;
  for (auto&& frame : frames) {
    bufs.append(frame.data.begin(), frame.data.end());
  }
  callback(bufs, {});
  return {};
}

}  // namespace wpi::detail
