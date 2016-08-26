/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "cameraserver_oo.h"

using namespace cs;

std::vector<VideoSource> VideoSource::EnumerateSources() {
  std::vector<VideoSource> sources;
  llvm::SmallVector<int, 16> handles;
  CS_Status status = 0;
  EnumerateSourceHandles(handles, &status);
  sources.reserve(handles.size());
  for (int handle : handles)
    sources.emplace_back(VideoSource{handle});
  return sources;
}

std::vector<VideoSink> VideoSink::EnumerateSinks() {
  std::vector<VideoSink> sinks;
  llvm::SmallVector<int, 16> handles;
  CS_Status status = 0;
  EnumerateSinkHandles(handles, &status);
  sinks.reserve(handles.size());
  for (int handle : handles)
    sinks.emplace_back(VideoSink{handle});
  return sinks;
}
