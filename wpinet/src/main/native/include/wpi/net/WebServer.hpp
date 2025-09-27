// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WEBSERVER_H_
#define WPINET_WEBSERVER_H_

#pragma once

#include <memory>
#include <string_view>

namespace wpi {

/**
 * A web server using the HTTP protocol.
 */
class WebServer {
 public:
  WebServer(const WebServer&) = delete;
  WebServer& operator=(const WebServer&) = delete;

  /**
   * Get an instance of the WebServer class.
   *
   * This is a singleton to guarantee that there is only a single instance
   * regardless of how many times GetInstance is called.
   */
  static WebServer& GetInstance();

  /**
   * Create a web server at the given port.
   * Note that local ports less than 1024 won't work as a normal user. Also,
   * many ports are blocked by the FRC robot radio; check the game manual for
   * what is allowed through the radio firewall.
   *
   * @param port local port number
   * @param path local path to document root
   */
  void Start(unsigned int port, std::string_view path);

  /**
   * Stop web server running at the given port.
   *
   * @param port local port number
   */
  void Stop(unsigned int port);

 private:
  WebServer();

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace wpi

#endif  // WPINET_WEBSERVER_H_
