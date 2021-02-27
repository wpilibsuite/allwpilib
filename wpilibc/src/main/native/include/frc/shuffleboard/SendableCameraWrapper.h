// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>

#ifndef DYNAMIC_CAMERA_SERVER
#include <cscore_oo.h>
#else
namespace cs {
class VideoSource;
}  // namespace cs
typedef int CS_Handle;
typedef CS_Handle CS_Source;
#endif

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableCameraWrapper;

namespace detail {
constexpr const char* kProtocol = "camera_server://";
std::shared_ptr<SendableCameraWrapper>& GetSendableCameraWrapper(
    CS_Source source);
void AddToSendableRegistry(Sendable* sendable, std::string name);
}  // namespace detail

/**
 * A wrapper to make video sources sendable and usable from Shuffleboard.
 */
class SendableCameraWrapper : public Sendable,
                              public SendableHelper<SendableCameraWrapper> {
 private:
  struct private_init {};

 public:
  /**
   * Creates a new sendable wrapper. Private constructor to avoid direct
   * instantiation with multiple wrappers floating around for the same camera.
   *
   * @param source the source to wrap
   */
  SendableCameraWrapper(CS_Source source, const private_init&);

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

  void InitSendable(SendableBuilder& builder) override;

 private:
  std::string m_uri;
};

#ifndef DYNAMIC_CAMERA_SERVER
inline SendableCameraWrapper::SendableCameraWrapper(CS_Source source,
                                                    const private_init&)
    : m_uri(detail::kProtocol) {
  CS_Status status = 0;
  auto name = cs::GetSourceName(source, &status);
  detail::AddToSendableRegistry(this, name);
  m_uri += name;
}

inline SendableCameraWrapper& SendableCameraWrapper::Wrap(
    const cs::VideoSource& source) {
  return Wrap(source.GetHandle());
}

inline SendableCameraWrapper& SendableCameraWrapper::Wrap(CS_Source source) {
  auto& wrapper = detail::GetSendableCameraWrapper(source);
  if (!wrapper) {
    wrapper = std::make_shared<SendableCameraWrapper>(source, private_init{});
  }
  return *wrapper;
}
#endif

}  // namespace frc
