/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SourceImpl.h"

#include <algorithm>
#include <cstring>

#include "Log.h"

using namespace cs;

static constexpr std::size_t kMaxFramesAvail = 32;

SourceImpl::SourceImpl(llvm::StringRef name)
    : m_name{name}, m_frame{*this, nullptr} {}

SourceImpl::~SourceImpl() {
  // Wake up anyone who is waiting.  This also clears the current frame,
  // which is good because its destructor will call back into the class.
  Wakeup();
  // Set a flag so ReleaseFrame() doesn't re-add them to m_framesAvail.
  // Put in a block so we destroy before the destructor ends.
  {
    m_destroyFrames = true;
    auto frames = std::move(m_framesAvail);
  }
  // Everything else can clean up itself.
}

uint64_t SourceImpl::GetCurFrameTime() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  return m_frame.time();
}

Frame SourceImpl::GetCurFrame() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  return m_frame;
}

Frame SourceImpl::GetNextFrame() {
  std::unique_lock<std::mutex> lock{m_frameMutex};
  // TODO: handle spurious wakeup by comparing frame timestamps
  m_frameCv.wait(lock);
  return m_frame;
}

void SourceImpl::Wakeup() {
  {
    std::lock_guard<std::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, nullptr};
  }
  m_frameCv.notify_all();
}

bool SourceImpl::SetPixelFormat(VideoMode::PixelFormat pixelFormat,
                                CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.pixelFormat = pixelFormat;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetResolution(int width, int height, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.width = width;
  mode.height = height;
  return SetVideoMode(mode, status);
}

bool SourceImpl::SetFPS(int fps, CS_Status* status) {
  auto mode = GetVideoMode(status);
  if (!mode) return false;
  mode.fps = fps;
  return SetVideoMode(mode, status);
}

void SourceImpl::PutFrame(VideoMode::PixelFormat pixelFormat,
                          llvm::StringRef data, Frame::Time time) {
  std::unique_ptr<Frame::Data> frameData;
  {
    std::lock_guard<std::mutex> lock{m_mutex};
    // find the smallest existing frame that is at least big enough.
    int found = -1;
    for (std::size_t i = 0; i < m_framesAvail.size(); ++i) {
      // is it big enough?
      if (m_framesAvail[i] && m_framesAvail[i]->capacity >= data.size()) {
        // is it smaller than the last found?
        if (found < 0 ||
            m_framesAvail[i]->capacity < m_framesAvail[found]->capacity) {
          // yes, update
          found = i;
        }
      }
    }

    // if nothing found, allocate a new buffer
    if (found < 0)
      frameData.reset(new Frame::Data{data.size()});
    else
      frameData = std::move(m_framesAvail[found]);
  }

  // Initialize frame data
  frameData->refcount = 0;
  frameData->time = time;
  frameData->size = data.size();
  frameData->pixelFormat = pixelFormat;

  // Copy in image data
  DEBUG4("Copying data to " << ((void*)frameData->data) << " from "
                            << ((void*)data.data()) << " (" << data.size()
                            << " bytes)");
  std::memcpy(frameData->data, data.data(), data.size());

  // Update frame
  {
    std::lock_guard<std::mutex> lock{m_frameMutex};
    m_frame = Frame{*this, std::move(frameData)};
  }

  // Signal listeners
  m_frameCv.notify_all();
}

void SourceImpl::ReleaseFrame(std::unique_ptr<Frame::Data> data) {
  std::lock_guard<std::mutex> lock{m_mutex};
  if (m_destroyFrames) return;
  // Return the frame to the pool.  First try to find an empty slot, otherwise
  // add it to the end.
  auto it = std::find(m_framesAvail.begin(), m_framesAvail.end(), nullptr);
  if (it != m_framesAvail.end())
    (*it) = std::move(data);
  else if (m_framesAvail.size() > kMaxFramesAvail) {
    // Replace smallest buffer; don't need to check for null because the above
    // find would have found it.
    auto it2 = std::min_element(m_framesAvail.begin(), m_framesAvail.end(),
                                [](const std::unique_ptr<Frame::Data>& a,
                                   const std::unique_ptr<Frame::Data>& b) {
                                  return a->capacity < b->capacity;
                                });
    if ((*it2)->capacity < data->capacity)
      *it2 = std::move(data);
  } else
    m_framesAvail.emplace_back(std::move(data));
}
