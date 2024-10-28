// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

#include <libssh/libssh.h>
#include <wpi/Logger.h>

namespace rtns {
/**
 * This class is a C++ implementation of the SshSessionController in
 * wpilibsuite/deploy-utils. It handles connecting to an SSH server, running
 * commands, and transferring files.
 */
class SshSession {
 public:
  /**
   * This is the exception that will be thrown by any of the methods in this
   * class if something goes wrong.
   */
  class SshException : public std::runtime_error {
   public:
    explicit SshException(const char* msg) : runtime_error(msg) {}
  };

  /**
   * Constructs a new session controller.
   *
   * @param host The hostname of the server to connect to.
   * @param port The port that the sshd server is operating on.
   * @param user The username to login as.
   * @param pass The password for the given username.
   * @param logger A reference to a logger to log messages to.
   */
  SshSession(std::string_view host, int port, std::string_view user,
             std::string_view pass, wpi::Logger& logger);

  /**
   * Destroys the controller object. This also disconnects the session from the
   * server.
   */
  ~SshSession();

  /**
   * Opens the SSH connection to the given host.
   */
  void Open();

  /**
   * Executes a command and logs the output (if there is any).
   *
   * @param cmd The command to execute on the server.
   */
  void Execute(std::string_view cmd);

  std::string ExecuteResult(std::string_view cmd, int* exitStatus);

  /**
   * Puts a file on the server using SFTP.
   *
   * @param path The path to the file to put (on the server).
   * @param contents The contents of the file.
   */
  void Put(std::string_view path, std::string_view contents);

 private:
  ssh_session m_session;
  std::string m_host;

  int m_port;

  std::string m_username;
  std::string m_password;

  wpi::Logger& m_logger;
};
}  // namespace rtns
