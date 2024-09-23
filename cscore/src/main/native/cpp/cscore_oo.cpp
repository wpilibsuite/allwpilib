// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cscore_oo.h"

#include <string>
#include <vector>

#include <fmt/format.h>
#include <wpi/json.h>

using namespace cs;

wpi::json VideoSource::GetConfigJsonObject() const {
  m_status = 0;
  return GetSourceConfigJsonObject(m_handle, &m_status);
}

wpi::json VideoSink::GetConfigJsonObject() const {
  m_status = 0;
  return GetSinkConfigJsonObject(m_handle, &m_status);
}

std::vector<VideoProperty> VideoSource::EnumerateProperties() const {
  wpi::SmallVector<CS_Property, 32> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSourceProperties(m_handle, handles_buf, &status);

  std::vector<VideoProperty> properties;
  properties.reserve(handles.size());
  for (CS_Property handle : handles) {
    properties.emplace_back(VideoProperty{handle});
  }
  return properties;
}

std::vector<VideoSink> VideoSource::EnumerateSinks() {
  wpi::SmallVector<CS_Sink, 16> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSourceSinks(m_handle, handles_buf, &status);

  std::vector<VideoSink> sinks;
  sinks.reserve(handles.size());
  for (int handle : handles) {
    sinks.emplace_back(VideoSink{handle});
  }
  return sinks;
}

std::vector<VideoSource> VideoSource::EnumerateSources() {
  wpi::SmallVector<CS_Source, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::cs::EnumerateSourceHandles(handles_buf, &status);

  std::vector<VideoSource> sources;
  sources.reserve(handles.size());
  for (int handle : handles) {
    sources.emplace_back(VideoSource{handle});
  }
  return sources;
}

std::vector<VideoProperty> VideoSink::EnumerateProperties() const {
  wpi::SmallVector<CS_Property, 32> handles_buf;
  CS_Status status = 0;
  auto handles = EnumerateSinkProperties(m_handle, handles_buf, &status);

  std::vector<VideoProperty> properties;
  properties.reserve(handles.size());
  for (CS_Property handle : handles) {
    properties.emplace_back(VideoProperty{handle});
  }
  return properties;
}

std::vector<VideoSink> VideoSink::EnumerateSinks() {
  wpi::SmallVector<CS_Sink, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::cs::EnumerateSinkHandles(handles_buf, &status);

  std::vector<VideoSink> sinks;
  sinks.reserve(handles.size());
  for (int handle : handles) {
    sinks.emplace_back(VideoSink{handle});
  }
  return sinks;
}

std::string AxisCamera::HostToUrl(std::string_view host) {
  return fmt::format("http://{}/mjpg/video.mjpg", host);
}
