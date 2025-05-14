// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WebSocketSerializer.h"

#include <random>

using namespace wpi::detail;

static constexpr uint8_t FLAG_MASKING = 0x80;
static constexpr size_t WRITE_ALLOC_SIZE = 4096;

static std::span<uint8_t> BuildHeader(std::span<uint8_t, 10> header,
                                      bool server,
                                      const wpi::WebSocket::Frame& frame) {
  uint8_t* pHeader = header.data();

  // opcode (includes FIN bit)
  *pHeader++ = frame.opcode;

  // payload length
  uint64_t size = 0;
  for (auto&& buf : frame.data) {
    size += buf.len;
  }
  if (size < 126) {
    *pHeader++ = (server ? 0x00 : FLAG_MASKING) | size;
  } else if (size <= 0xffff) {
    *pHeader++ = (server ? 0x00 : FLAG_MASKING) | 126;
    *pHeader++ = (size >> 8) & 0xff;
    *pHeader++ = size & 0xff;
  } else {
    *pHeader++ = (server ? 0x00 : FLAG_MASKING) | 127;
    *pHeader++ = (size >> 56) & 0xff;
    *pHeader++ = (size >> 48) & 0xff;
    *pHeader++ = (size >> 40) & 0xff;
    *pHeader++ = (size >> 32) & 0xff;
    *pHeader++ = (size >> 24) & 0xff;
    *pHeader++ = (size >> 16) & 0xff;
    *pHeader++ = (size >> 8) & 0xff;
    *pHeader++ = size & 0xff;
  }
  return header.subspan(0, pHeader - header.data());
}

size_t SerializedFrames::AddClientFrame(const WebSocket::Frame& frame) {
  uint8_t headerBuf[10];
  auto header = BuildHeader(headerBuf, false, frame);

  // allocate a buffer per frame
  size_t size = header.size() + 4;
  for (auto&& buf : frame.data) {
    size += buf.len;
  }
  m_allocBufs.emplace_back(uv::Buffer::Allocate(size));
  m_bufs.emplace_back(m_allocBufs.back());

  char* internalBuf = m_allocBufs.back().data().data();
  std::memcpy(internalBuf, header.data(), header.size());
  internalBuf += header.size();

  // generate masking key
  static std::random_device rd;
  static std::default_random_engine gen{rd()};
  std::uniform_int_distribution<unsigned int> dist(0, 255);
  uint8_t key[4];
  for (uint8_t& v : key) {
    v = dist(gen);
  }
  std::memcpy(internalBuf, key, 4);
  internalBuf += 4;

  // copy and mask data
  int n = 0;
  for (auto&& buf : frame.data) {
    for (auto&& ch : buf.data()) {
      *internalBuf++ = static_cast<uint8_t>(ch) ^ key[n++];
      if (n >= 4) {
        n = 0;
      }
    }
  }
  return size;
}

size_t SerializedFrames::AddServerFrame(const WebSocket::Frame& frame) {
  uint8_t headerBuf[10];
  auto header = BuildHeader(headerBuf, true, frame);

  // manage allocBufs to efficiently store header
  if (m_allocBufs.empty() ||
      (m_allocBufPos + header.size()) > WRITE_ALLOC_SIZE) {
    m_allocBufs.emplace_back(uv::Buffer::Allocate(WRITE_ALLOC_SIZE));
    m_allocBufPos = 0;
  }
  char* internalBuf = m_allocBufs.back().data().data() + m_allocBufPos;
  std::memcpy(internalBuf, header.data(), header.size());
  m_bufs.emplace_back(internalBuf, header.size());
  m_allocBufPos += header.size();
  // servers can just send the buffers directly without masking
  m_bufs.append(frame.data.begin(), frame.data.end());
  size_t sent = header.size();
  for (auto&& buf : frame.data) {
    sent += buf.len;
  }
  return sent;
}
