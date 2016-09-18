/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SourceImpl.h"

#include "llvm/STLExtras.h"

using namespace cs;

SourceImpl::SourceImpl(llvm::StringRef name)
    : m_name{name}, m_frame{*this, nullptr} {}

SourceImpl::~SourceImpl() {
  // Wake up anyone who is waiting.  This also clears the current frame,
  // which is good because its destructor will call back into the class.
  EnableSink();
  Wakeup();
  // Everything else can clean up itself.
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

void SourceImpl::StartFrame() {
  std::lock_guard<std::mutex> lock{m_mutex};
  if (m_frameData) return;
  if (m_framesAvail.empty()) {
    m_frameData = llvm::make_unique<Frame::Data>();
  } else {
    m_frameData = std::move(m_framesAvail.back());
    m_framesAvail.pop_back();
    m_frameData->refcount = 0;
  }
}

//TODO: Image& SourceImpl::AddImage(std::size_t size) {}

void SourceImpl::FinishFrame() {
  {
    std::lock_guard<std::mutex> lock{m_mutex};
    std::lock_guard<std::mutex> lock2{m_frameMutex};
    m_frame = Frame{*this, m_frameData.release()};
  }
  m_frameCv.notify_all();
}

void SourceImpl::ReleaseFrame(Frame::Data* data) {
  std::lock_guard<std::mutex> lock{m_mutex};
  // Return the image to the pool
  m_imagesAvail.emplace_back(std::move(data->image));
  // Return the frame to the pool
  m_framesAvail.emplace_back(data);
}
