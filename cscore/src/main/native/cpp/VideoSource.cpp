// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cs/VideoSource.hpp"

#include <vector>

#include <fmt/format.h>

#include "wpi/cs/VideoSink.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::cs;

wpi::util::json VideoSource::GetConfigJsonObject() const {
  m_status = 0;
  return GetSourceConfigJsonObject(m_handle, &m_status);
}

std::vector<VideoProperty> VideoSource::EnumerateProperties() const {
  wpi::util::SmallVector<CS_Property, 32> handles_buf;
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
  wpi::util::SmallVector<CS_Sink, 16> handles_buf;
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
  wpi::util::SmallVector<CS_Source, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::wpi::cs::EnumerateSourceHandles(handles_buf, &status);

  std::vector<VideoSource> sources;
  sources.reserve(handles.size());
  for (int handle : handles) {
    sources.emplace_back(VideoSource{handle});
  }
  return sources;
}
