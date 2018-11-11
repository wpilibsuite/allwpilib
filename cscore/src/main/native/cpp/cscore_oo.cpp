/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cscore_oo.h"

#include <wpi/json.h>

using namespace cs;

wpi::json VideoSource::GetConfigJsonObject() const {
  m_status = 0;
  return GetSourceConfigJsonObject(m_handle, &m_status);
}

std::vector<VideoProperty> VideoSource::EnumerateProperties() const {
  wpi::SmallVector<CS_Property, 32> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSourceProperties(m_handle, handles_buf, &status);

  std::vector<VideoProperty> properties;
  properties.reserve(handles.size());
  for (CS_Property handle : handles)
    properties.emplace_back(VideoProperty{handle});
  return properties;
}

std::vector<VideoSink> VideoSource::EnumerateSinks() {
  wpi::SmallVector<CS_Sink, 16> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSourceSinks(m_handle, handles_buf, &status);

  std::vector<VideoSink> sinks;
  sinks.reserve(handles.size());
  for (int handle : handles) sinks.emplace_back(VideoSink{handle});
  return sinks;
}

std::vector<VideoSource> VideoSource::EnumerateSources() {
  wpi::SmallVector<CS_Source, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::cs::EnumerateSourceHandles(handles_buf, &status);

  std::vector<VideoSource> sources;
  sources.reserve(handles.size());
  for (int handle : handles) sources.emplace_back(VideoSource{handle});
  return sources;
}

std::vector<VideoProperty> VideoSink::EnumerateProperties() const {
  wpi::SmallVector<CS_Property, 32> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSinkProperties(m_handle, handles_buf, &status);

  std::vector<VideoProperty> properties;
  properties.reserve(handles.size());
  for (CS_Property handle : handles)
    properties.emplace_back(VideoProperty{handle});
  return properties;
}

std::vector<VideoSink> VideoSink::EnumerateSinks() {
  wpi::SmallVector<CS_Sink, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::cs::EnumerateSinkHandles(handles_buf, &status);

  std::vector<VideoSink> sinks;
  sinks.reserve(handles.size());
  for (int handle : handles) sinks.emplace_back(VideoSink{handle});
  return sinks;
}
