/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <cscore_c.h>

#include "frc/smartdashboard/SendableBase.h"

namespace cs {
class VideoSource;
}  // namespace cs

namespace frc {

/**
 * A wrapper to make video sources sendable and usable from Shuffleboard.
 */
class SendableCameraWrapper : public SendableBase {
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

}  // namespace frc
