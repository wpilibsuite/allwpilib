// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameraservercv/CameraServerCv.h"

#include <atomic>
#include <vector>

#include <cameraserver/CameraServer.h>
#include <fmt/format.h>
#include <networktables/BooleanTopic.h>
#include <networktables/IntegerTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringArrayTopic.h>
#include <networktables/StringTopic.h>
#include <wpi/DenseMap.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/mutex.h>

#include "cameraserver/CameraServerShared.h"
#include "ntcore_cpp.h"

using namespace frc;

cs::CvSink CameraServerCv::GetVideo() {
  cs::VideoSource source = CameraServer::GetInternalPrimarySource();
  if (!source) {
    return cs::CvSink{};
  }
  return GetVideo(std::move(source));
}

cs::CvSink CameraServerCv::GetVideo(const cs::VideoSource& camera) {
  return GetVideo(camera, cs::VideoMode::PixelFormat::kBGR);
}

cs::CvSink CameraServerCv::GetVideo(const cs::VideoSource& camera,
                                    cs::VideoMode::PixelFormat pixelFormat) {
  wpi::SmallString<64> name{"opencv_"};
  name += camera.GetName();

  auto sink = CameraServer::GetInternalCvSink(name);
  if (sink.has_value()) {
    auto sinkHandle = sink.value();
    if (!sinkHandle) {
      return cs::CvSink{};
    }
    return cs::CvSink{sink.value(), cs::VideoMode::PixelFormat::kBGR};
  }

  cs::CvSink newsink{name.str(), pixelFormat};
  newsink.SetSource(camera);
  CameraServer::AddServer(newsink);
  return newsink;
}

cs::CvSink CameraServerCv::GetVideo(std::string_view name) {
  cs::VideoSource source = CameraServer::GetInternalVideoSource(name);
  if (!source) {
    return cs::CvSink{};
  }
  return GetVideo(source);
}

cs::CvSink CameraServerCv::GetVideo(std::string_view name,
                                    cs::VideoMode::PixelFormat pixelFormat) {
  cs::VideoSource source = CameraServer::GetInternalVideoSource(name);
  if (!source) {
    return cs::CvSink{};
  }
  return GetVideo(source, pixelFormat);
}

cs::CvSource CameraServerCv::PutVideo(std::string_view name, int width,
                                      int height) {
  cs::CvSource source{name, cs::VideoMode::kMJPEG, width, height, 30};
  CameraServer::StartAutomaticCapture(source);
  return source;
}
