// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SshSession.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

#include <algorithm>
#include <string>

#include <fmt/format.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <wpi/Logger.h>

using namespace rtns;

#define INFO(fmt, ...) WPI_INFO(m_logger, fmt __VA_OPT__(, ) __VA_ARGS__)

SshSession::SshSession(std::string_view host, int port, std::string_view user,
                       std::string_view pass, wpi::Logger& logger)
    : m_host{host},
      m_port{port},
      m_username{user},
      m_password{pass},
      m_logger{logger} {
  // Create a new SSH session.
  m_session = ssh_new();
  if (!m_session) {
    throw SshException("The SSH session could not be allocated.");
  }

  // Set the host, user, and port.
  ssh_options_set(m_session, SSH_OPTIONS_HOST, m_host.c_str());
  ssh_options_set(m_session, SSH_OPTIONS_USER, m_username.c_str());
  ssh_options_set(m_session, SSH_OPTIONS_PORT, &m_port);

  // Set timeout to 3 seconds.
  int64_t timeout = 3L;
  ssh_options_set(m_session, SSH_OPTIONS_TIMEOUT, &timeout);

  // Set other miscellaneous options.
  ssh_options_set(m_session, SSH_OPTIONS_STRICTHOSTKEYCHECK, "no");
}

SshSession::~SshSession() {
  ssh_disconnect(m_session);
  ssh_free(m_session);
}

void SshSession::Open() {
  // Connect to the server.
  int rc = ssh_connect(m_session);
  if (rc != SSH_OK) {
    throw SshException(ssh_get_error(m_session));
  }

  // Authenticate with password.
  rc = ssh_userauth_password(m_session, nullptr, m_password.c_str());
  if (rc != SSH_AUTH_SUCCESS) {
    throw SshException(ssh_get_error(m_session));
  }
}

void SshSession::Execute(std::string_view cmd) {
  // Allocate a new channel.
  ssh_channel channel = ssh_channel_new(m_session);
  if (!channel) {
    throw SshException(ssh_get_error(m_session));
  }

  // Open the channel.
  int rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK) {
    throw SshException(ssh_get_error(m_session));
  }

  // Execute the command.
  std::string command{cmd};
  rc = ssh_channel_request_exec(channel, command.c_str());
  if (rc != SSH_OK) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    throw SshException(ssh_get_error(m_session));
  }
  uint32_t exitCode = 0;
#if LIBSSH_VERSION_MAJOR == 0 && LIBSSH_VERSION_MINOR >= 11
  ssh_channel_get_exit_state(channel, &exitCode, nullptr, nullptr);
#else
  exitCode = ssh_channel_get_exit_status(channel);
#endif
  INFO("{} {}", exitCode, cmd);

  // Log output.
  char buf[512];
  int read = ssh_channel_read(channel, buf, sizeof(buf), 0);
  if (read != 0) {
    if (read < static_cast<int>(sizeof(buf) / sizeof(buf[0]))) {
      buf[read] = 0;
    } else {
      buf[(sizeof(buf) / sizeof(buf[0])) - 1] = 0;
    }
    INFO("stdout: {} {}", read, buf);
  }

  read = ssh_channel_read(channel, buf, sizeof(buf), 1);
  if (read != 0) {
    if (read < static_cast<int>(sizeof(buf) / sizeof(buf[0]))) {
      buf[read] = 0;
    } else {
      buf[(sizeof(buf) / sizeof(buf[0])) - 1] = 0;
    }
    INFO("stderr: {} {}", read, buf);
  }

  // Close and free channel.
  ssh_channel_close(channel);
  ssh_channel_free(channel);
}

std::string SshSession::ExecuteResult(std::string_view cmd, int* exitStatus) {
  // Allocate a new channel.
  ssh_channel channel = ssh_channel_new(m_session);
  if (!channel) {
    throw SshException(ssh_get_error(m_session));
  }

  // Open the channel.
  int rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK) {
    throw SshException(ssh_get_error(m_session));
  }

  // Execute the command.
  std::string command{cmd};
  rc = ssh_channel_request_exec(channel, command.c_str());
  if (rc != SSH_OK) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    throw SshException(ssh_get_error(m_session));
  }
  uint32_t exitCode = 0;
#if LIBSSH_VERSION_MAJOR == 0 && LIBSSH_VERSION_MINOR >= 11
  ssh_channel_get_exit_state(channel, &exitCode, nullptr, nullptr);
#else
  exitCode = ssh_channel_get_exit_status(channel);
#endif
  INFO("{} {}", exitCode, cmd);

  std::string result;
  if (exitStatus) {
    *exitStatus = exitCode;
  }

  // Log output.
  char buf[512];
  int read = ssh_channel_read(channel, buf, sizeof(buf), 0);
  if (read != 0) {
    if (read < static_cast<int>(sizeof(buf) / sizeof(buf[0]))) {
      buf[read] = 0;
    } else {
      buf[(sizeof(buf) / sizeof(buf[0])) - 1] = 0;
    }
    result = buf;
    INFO("stdout: {} {}", read, buf);
  }

  read = ssh_channel_read(channel, buf, sizeof(buf), 1);
  if (read != 0) {
    if (read < static_cast<int>(sizeof(buf) / sizeof(buf[0]))) {
      buf[read] = 0;
    } else {
      buf[(sizeof(buf) / sizeof(buf[0])) - 1] = 0;
    }
    INFO("stderr: {} {}", read, buf);
  }

  // Close and free channel.
  ssh_channel_close(channel);
  ssh_channel_free(channel);

  return result;
}

void SshSession::Put(std::string_view path, std::string_view contents) {
  // Allocate the SFTP session.
  sftp_session sftp = sftp_new(m_session);
  if (!sftp) {
    throw SshException(ssh_get_error(m_session));
  }

  // Initialize.
  int rc = sftp_init(sftp);
  if (rc != SSH_OK) {
    sftp_free(sftp);
    throw SshException(ssh_get_error(m_session));
  }

  // Copy.
  sftp_file file =
      sftp_open(sftp, path.data(), O_WRONLY | O_CREAT | O_TRUNC, S_IFMT);
  if (!file) {
    sftp_free(sftp);
    throw SshException(ssh_get_error(m_session));
  }

  // Send 150K at a time.
  static constexpr size_t kChunkSize = 150000;
  for (size_t i = 0; i < contents.size(); i += kChunkSize) {
    size_t len = (std::min)(kChunkSize, contents.size() - i);
    size_t written = sftp_write(file, contents.data() + i, len);
    if (written != len) {
      sftp_close(file);
      sftp_free(sftp);
      throw SshException(ssh_get_error(m_session));
    }
  }

  INFO("[SFTP] Deployed {}!", path);

  // Close file, free memory.
  sftp_close(file);
  sftp_free(sftp);
}
