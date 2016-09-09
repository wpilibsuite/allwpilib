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
    if (m_enabled) m_source->DisableSink();
    m_source->RemoveSink();
  }
}

void SinkImpl::SetSource(std::shared_ptr<SourceImpl> source) {
  std::lock_guard<std::mutex> lock(m_mutex);
  bool was_enabled = m_enabled;
  if (m_source) {
    if (m_enabled) m_source->DisableSink();
    m_source->RemoveSink();
  }
  m_source = source;
  m_source->AddSink();
  if (was_enabled) m_source->EnableSink();
}
