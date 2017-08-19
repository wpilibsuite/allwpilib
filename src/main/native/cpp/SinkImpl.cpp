/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SinkImpl.h"

#include "Notifier.h"
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

void SinkImpl::Enable() {
  std::lock_guard<std::mutex> lock(m_mutex);
  ++m_enabledCount;
  if (m_enabledCount == 1) {
    if (m_source) m_source->EnableSink();
    Notifier::GetInstance().NotifySink(*this, CS_SINK_ENABLED);
  }
}

void SinkImpl::Disable() {
  std::lock_guard<std::mutex> lock(m_mutex);
  --m_enabledCount;
  if (m_enabledCount == 0) {
    if (m_source) m_source->DisableSink();
    Notifier::GetInstance().NotifySink(*this, CS_SINK_DISABLED);
  }
}

void SinkImpl::SetEnabled(bool enabled) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (enabled && m_enabledCount == 0) {
    if (m_source) m_source->EnableSink();
    m_enabledCount = 1;
    Notifier::GetInstance().NotifySink(*this, CS_SINK_ENABLED);
  } else if (!enabled && m_enabledCount > 0) {
    if (m_source) m_source->DisableSink();
    m_enabledCount = 0;
    Notifier::GetInstance().NotifySink(*this, CS_SINK_DISABLED);
  }
}

void SinkImpl::SetSource(std::shared_ptr<SourceImpl> source) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_source == source) return;
    if (m_source) {
      if (m_enabledCount > 0) m_source->DisableSink();
      m_source->RemoveSink();
    }
    m_source = source;
    if (m_source) {
      m_source->AddSink();
      if (m_enabledCount > 0) m_source->EnableSink();
    }
  }
  SetSourceImpl(source);
}

std::string SinkImpl::GetError() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_source) return "no source connected";
  return m_source->GetCurFrame().GetError();
}

llvm::StringRef SinkImpl::GetError(llvm::SmallVectorImpl<char>& buf) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_source) return "no source connected";
  // Make a copy as it's shared data
  llvm::StringRef error = m_source->GetCurFrame().GetError();
  buf.clear();
  buf.append(error.data(), error.data() + error.size());
  return llvm::StringRef{buf.data(), buf.size()};
}

void SinkImpl::SetSourceImpl(std::shared_ptr<SourceImpl> source) {}
