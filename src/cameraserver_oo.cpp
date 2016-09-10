/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cameraserver_oo.h"

using namespace cs;

std::vector<VideoProperty> VideoSource::EnumerateProperties() const {
  std::vector<VideoProperty> properties;
  llvm::SmallVector<CS_Property, 32> handles;
  CS_Status status = 0;
  EnumerateSourceProperties(m_handle, handles, &status);
  properties.reserve(handles.size());
  for (int handle : handles)
    properties.emplace_back(VideoProperty{handle});
  return properties;
}

std::vector<VideoSource> VideoSource::EnumerateSources() {
  std::vector<VideoSource> sources;
  llvm::SmallVector<CS_Source, 16> handles;
  CS_Status status = 0;
  ::cs::EnumerateSourceHandles(handles, &status);
  sources.reserve(handles.size());
  for (int handle : handles)
    sources.emplace_back(VideoSource{handle});
  return sources;
}

std::vector<VideoSink> VideoSink::EnumerateSinks() {
  std::vector<VideoSink> sinks;
  llvm::SmallVector<CS_Sink, 16> handles;
  CS_Status status = 0;
  ::cs::EnumerateSinkHandles(handles, &status);
  sinks.reserve(handles.size());
  for (int handle : handles)
    sinks.emplace_back(VideoSink{handle});
  return sinks;
}
