/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SinkImpl.h"

#include "SourceImpl.h"

using namespace cs;

SinkImpl::SinkImpl(llvm::StringRef name) : m_name{name} {}

SinkImpl::~SinkImpl() {
  if (m_source) {
    if (m_enabledCount > 0) m_source->DisableSink();
    m_source->RemoveSink();
  }
}

void SinkImpl::SetDescription(llvm::StringRef description) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_description = description;
}

llvm::StringRef SinkImpl::GetDescription(
    llvm::SmallVectorImpl<char>& buf) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  buf.append(m_description.begin(), m_description.end());
  return llvm::StringRef{buf.data(), buf.size()};
}

void SinkImpl::SetSource(std::shared_ptr<SourceImpl> source) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_source) {
      if (m_enabledCount > 0) m_source->DisableSink();
      m_source->RemoveSink();
    }
    m_source = source;
    m_source->AddSink();
    if (m_enabledCount > 0) m_source->EnableSink();
  }
  SetSourceImpl(source);
}

std::string SinkImpl::GetError() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_source) return "no source connected";
  auto frame = m_source->GetCurFrame();
  if (frame) return std::string{};  // no error
  return llvm::StringRef{frame};
}

llvm::StringRef SinkImpl::GetError(llvm::SmallVectorImpl<char>& buf) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_source) return "no source connected";
  auto frame = m_source->GetCurFrame();
  if (frame) return llvm::StringRef{};  // no error
  buf.append(frame.data(), frame.data() + frame.size());
  return llvm::StringRef{buf.data(), buf.size()};
}

void SinkImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {}
