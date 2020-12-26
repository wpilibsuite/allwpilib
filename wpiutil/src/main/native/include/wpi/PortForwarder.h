// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_PORTFORWARDER_H_
#define WPIUTIL_WPI_PORTFORWARDER_H_

#pragma once

#include <memory>

#include "wpi/Twine.h"

namespace wpi {

/**
 * Forward ports to another host.  This is primarily useful for accessing
 * Ethernet-connected devices from a computer tethered to the RoboRIO USB port.
 */
class PortForwarder {
 public:
  PortForwarder(const PortForwarder&) = delete;
  PortForwarder& operator=(const PortForwarder&) = delete;

  /**
   * Get an instance of the PortForwarder class.
   *
   * This is a singleton to guarantee that there is only a single instance
   * regardless of how many times GetInstance is called.
   */
  static PortForwarder& GetInstance();

  /**
   * Forward a local TCP port to a remote host and port.
   * Note that local ports less than 1024 won't work as a normal user.
   *
   * @param port       local port number
   * @param remoteHost remote IP address / DNS name
   * @param remotePort remote port number
   */
  void Add(unsigned int port, const Twine& remoteHost, unsigned int remotePort);

  /**
   * Stop TCP forwarding on a port.
   *
   * @param port local port number
   */
  void Remove(unsigned int port);

 private:
  PortForwarder();

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_PORTFORWARDER_H_
