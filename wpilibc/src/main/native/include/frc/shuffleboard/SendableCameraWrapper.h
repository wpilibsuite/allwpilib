// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>

#ifndef DYNAMIC_CAMERA_SERVER
#include <cscore_oo.h>
#include <fmt/format.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#else
namespace cs {
class VideoSource;
}  // namespace cs
using CS_Handle = int;        // NOLINT
using CS_Source = CS_Handle;  // NOLINT
#endif

#include <networktables/StringArrayTopic.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

class SendableCameraWrapper;

namespace detail {
constexpr const char* kProtocol = "camera_server://";
std::shared_ptr<SendableCameraWrapper>& GetSendableCameraWrapper(
    std::string_view cameraName);
void AddToSendableRegistry(wpi::Sendable* sendable, std::string_view name);
}  // namespace detail

/**
 * A wrapper to make video sources sendable and usable from Shuffleboard.
 */
class SendableCameraWrapper
    : public wpi::Sendable,
      public wpi::SendableHelper<SendableCameraWrapper> {
 private:
  struct private_init {};

 public:
  /**
   * Creates a new sendable wrapper. Private constructor to avoid direct
   * instantiation with multiple wrappers floating around for the same camera.
   *
   * @param cameraName the name of the camera to wrap
   */
  SendableCameraWrapper(std::string_view cameraName, const private_init&);

  /**
   * Creates a new sendable wrapper. Private constructor to avoid direct
   * instantiation with multiple wrappers floating around for the same camera.
   *
   * @param cameraName the name of the camera to wrap
   * @param cameraUrls camera URLs
   */
  SendableCameraWrapper(std::string_view cameraName,
                        std::span<const std::string> cameraUrls,
                        const private_init&);

  /**
   * Gets a sendable wrapper object for the given video source, creating the
   * wrapper if one does not already exist for the source.
   *
   * @param source the video source to wrap
   * @return a sendable wrapper object for the video source, usable in
   * Shuffleboard via ShuffleboardTab::Add() and ShuffleboardLayout::Add()
   */
  static SendableCameraWrapper& Wrap(const cs::VideoSource& source);
  static SendableCameraWrapper& Wrap(CS_Source source);

  static SendableCameraWrapper& Wrap(std::string_view cameraName,
                                     std::span<const std::string> cameraUrls);

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::string m_uri;
  nt::StringArrayPublisher m_streams;
};

#ifndef DYNAMIC_CAMERA_SERVER
inline SendableCameraWrapper::SendableCameraWrapper(std::string_view name,
                                                    const private_init&)
    : m_uri(detail::kProtocol) {
  detail::AddToSendableRegistry(this, name);
  m_uri += name;
}

inline SendableCameraWrapper::SendableCameraWrapper(
    std::string_view cameraName, std::span<const std::string> cameraUrls,
    const private_init&)
    : SendableCameraWrapper(cameraName, private_init{}) {
  m_streams = nt::NetworkTableInstance::GetDefault()
                  .GetStringArrayTopic(
                      fmt::format("/CameraPublisher/{}/streams", cameraName))
                  .Publish();
  m_streams.Set(cameraUrls);
}

inline SendableCameraWrapper& SendableCameraWrapper::Wrap(
    const cs::VideoSource& source) {
  return Wrap(source.GetHandle());
}

inline SendableCameraWrapper& SendableCameraWrapper::Wrap(CS_Source source) {
  CS_Status status = 0;
  auto name = cs::GetSourceName(source, &status);
  auto& wrapper = detail::GetSendableCameraWrapper(name);
  if (!wrapper) {
    wrapper = std::make_shared<SendableCameraWrapper>(name, private_init{});
  }
  return *wrapper;
}

inline SendableCameraWrapper& SendableCameraWrapper::Wrap(
    std::string_view cameraName, std::span<const std::string> cameraUrls) {
  auto& wrapper = detail::GetSendableCameraWrapper(cameraName);
  if (!wrapper) {
    wrapper = std::make_shared<SendableCameraWrapper>(cameraName, cameraUrls,
                                                      private_init{});
  }
  return *wrapper;
}
#endif

}  // namespace frc
