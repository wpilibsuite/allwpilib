/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <cscore_oo.h>

#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableCameraWrapper;

namespace detail {
constexpr const char* kProtocol = "camera_server://";
std::shared_ptr<SendableCameraWrapper>& GetSendableCameraWrapper(int source);
void CreateSendableCameraWrapperSendable(std::function<std::string()> urlGetter,
                                         SendableBuilder& builder);
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
  SendableCameraWrapper(CS_Source source, const private_init&)
      : m_uri(detail::kProtocol) {
    CS_Status status = 0;
    auto name = cs::GetSourceName(source, &status);
    detail::AddToSendableRegistry(this, name);
    m_uri += name;
  }

  /**
   * Gets a sendable wrapper object for the given video source, creating the
   * wrapper if one does not already exist for the source.
   *
   * @param source the video source to wrap
   * @return a sendable wrapper object for the video source, usable in
   * Shuffleboard via ShuffleboardTab::Add() and ShuffleboardLayout::Add()
   */
  static SendableCameraWrapper& Wrap(const cs::VideoSource& source) {
    return Wrap(source.GetHandle());
  }
  static SendableCameraWrapper& Wrap(CS_Source source) {
    auto& wrapper = detail::GetSendableCameraWrapper(static_cast<int>(source));
    if (!wrapper)
      wrapper = std::make_shared<SendableCameraWrapper>(source, private_init{});
    return *wrapper;
  }

  void InitSendable(SendableBuilder& builder) override {
    detail::CreateSendableCameraWrapperSendable([this] { return m_uri; },
                                                builder);
  }

 private:
  std::string m_uri;
};

}  // namespace frc
