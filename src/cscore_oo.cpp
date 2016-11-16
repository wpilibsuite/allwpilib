/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cscore_oo.h"

using namespace cs;

std::vector<VideoProperty> VideoSource::EnumerateProperties() const {
  llvm::SmallVector<CS_Property, 32> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSourceProperties(m_handle, handles_buf, &status);

  std::vector<VideoProperty> properties;
  properties.reserve(handles.size());
  for (CS_Property handle : handles)
    properties.emplace_back(VideoProperty{handle});
  return properties;
}

std::vector<VideoSink> VideoSource::EnumerateSinks() {
  llvm::SmallVector<CS_Sink, 16> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSourceSinks(m_handle, handles_buf, &status);

  std::vector<VideoSink> sinks;
  sinks.reserve(handles.size());
  for (int handle : handles)
    sinks.emplace_back(VideoSink{handle});
  return sinks;
}

std::vector<VideoSource> VideoSource::EnumerateSources() {
  llvm::SmallVector<CS_Source, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::cs::EnumerateSourceHandles(handles_buf, &status);

  std::vector<VideoSource> sources;
  sources.reserve(handles.size());
  for (int handle : handles)
    sources.emplace_back(VideoSource{handle});
  return sources;
}

std::vector<VideoSink> VideoSink::EnumerateSinks() {
  llvm::SmallVector<CS_Sink, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::cs::EnumerateSinkHandles(handles_buf, &status);

  std::vector<VideoSink> sinks;
  sinks.reserve(handles.size());
  for (int handle : handles)
    sinks.emplace_back(VideoSink{handle});
  return sinks;
}
