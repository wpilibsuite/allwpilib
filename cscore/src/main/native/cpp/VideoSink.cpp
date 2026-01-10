// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cs/VideoSink.hpp"

#include <vector>

#include <fmt/format.h>

#include "wpi/util/json.hpp"

using namespace wpi::cs;

wpi::util::json VideoSink::GetConfigJsonObject() const {
  m_status = 0;
  return GetSinkConfigJsonObject(m_handle, &m_status);
}

std::vector<VideoProperty> VideoSink::EnumerateProperties() const {
  wpi::util::SmallVector<CS_Property, 32> handles_buf;
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
  wpi::util::SmallVector<CS_Sink, 16> handles_buf;
  CS_Status status = 0;
  auto handles = ::wpi::cs::EnumerateSinkHandles(handles_buf, &status);

  std::vector<VideoSink> sinks;
  sinks.reserve(handles.size());
  for (int handle : handles) {
    sinks.emplace_back(VideoSink{handle});
  }
  return sinks;
}
