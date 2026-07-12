// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <libssh/libssh.h>
#include <libssh/sftp.h>

namespace sftp {

struct Attributes {
  Attributes() = default;
  explicit Attributes(sftp_attributes&& attr);

  std::string name;
  uint32_t flags = 0;
  uint8_t type = 0;
  uint64_t size = 0;
};

/**
 * This is the exception that will be thrown if something goes wrong.
 */
class Exception : public std::runtime_error {
 public:
  explicit Exception(const std::string& msg) : std::runtime_error{msg} {}
  explicit Exception(sftp_session sftp);

  int err = 0;
};

class File {
 public:
  File() = default;
  explicit File(sftp_file&& handle) : m_handle{handle} {}
  ~File();

  Attributes Stat() const;

  void SetNonblocking() { sftp_file_set_nonblocking(m_handle); }
  void SetBlocking() { sftp_file_set_blocking(m_handle); }

  size_t Read(void* buf, uint32_t count);
  size_t Write(std::span<const uint8_t> data);

  void Seek(uint64_t offset);
  uint64_t Tell() const;
  void Rewind();

  void Sync();

  std::string_view GetName() const { return m_handle->name; }
  uint64_t GetOffset() const { return m_handle->offset; }
  bool IsEof() const { return m_handle->eof; }
  bool IsNonblocking() const { return m_handle->nonblocking; }

 private:
  sftp_file m_handle{nullptr};
};

/**
 * This class is a C++ implementation of the SshSessionController in
 * wpilibsuite/deploy-utils. It handles connecting to an SSH server, running
 * commands, and transferring files.
 */
class Session {
 public:
  /**
   * Constructs a new session controller.
   *
   * @param host The hostname of the server to connect to.
   * @param port The port that the sshd server is operating on.
   * @param user The username to login as.
   * @param pass The password for the given username.
   */
  Session(std::string_view host, int port, std::string_view user,
          std::string_view pass);

  /**
   * Destroys the controller object. This also disconnects the session from the
   * server.
   */
  ~Session();

  /**
   * Opens the SSH connection to the given host.
   */
  void Connect();

  /**
   * Disconnects the SSH connection.
   */
  void Disconnect();

  /**
   * Reads directory entries
   *
   * @param path remote path
   * @return vector of file attributes
   */
  std::vector<Attributes> ReadDir(const std::string& path);

  /**
   * Unlinks (deletes) a file.
   *
   * @param filename filename
   */
  void Unlink(const std::string& filename);

  /**
   * Opens a file.
   *
   * @param filename filename
   * @param accesstype O_RDONLY, O_WRONLY, or O_RDWR, combined with O_CREAT,
   *                   O_EXCL, or O_TRUNC
   * @param mode permissions to use if a new file is created
   * @return File
   */
  File Open(const std::string& filename, int accesstype, mode_t mode);

 private:
  ssh_session m_session{nullptr};
  sftp_session m_sftp{nullptr};
  std::string m_host;

  int m_port;

  std::string m_username;
  std::string m_password;
};

}  // namespace sftp
