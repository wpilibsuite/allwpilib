// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Sftp.h"

#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

using namespace sftp;

Attributes::Attributes(sftp_attributes&& attr)
    : name{attr->name}, flags{attr->flags}, type{attr->type}, size{attr->size} {
  sftp_attributes_free(attr);
}

static std::string GetError(sftp_session sftp) {
  switch (sftp_get_error(sftp)) {
    case SSH_FX_EOF:
      return "end of file";
    case SSH_FX_NO_SUCH_FILE:
      return "no such file";
    case SSH_FX_PERMISSION_DENIED:
      return "permission denied";
    case SSH_FX_FAILURE:
      return "SFTP failure";
    case SSH_FX_BAD_MESSAGE:
      return "SFTP bad message";
    case SSH_FX_NO_CONNECTION:
      return "SFTP no connection";
    case SSH_FX_CONNECTION_LOST:
      return "SFTP connection lost";
    case SSH_FX_OP_UNSUPPORTED:
      return "SFTP operation unsupported";
    case SSH_FX_INVALID_HANDLE:
      return "SFTP invalid handle";
    case SSH_FX_NO_SUCH_PATH:
      return "no such path";
    case SSH_FX_FILE_ALREADY_EXISTS:
      return "file already exists";
    case SSH_FX_WRITE_PROTECT:
      return "write protected filesystem";
    case SSH_FX_NO_MEDIA:
      return "no media inserted";
    default:
      return ssh_get_error(sftp->session);
  }
}

Exception::Exception(sftp_session sftp)
    : runtime_error{GetError(sftp)}, err{sftp_get_error(sftp)} {}

File::~File() {
  if (m_handle) {
    sftp_close(m_handle);
  }
}

Attributes File::Stat() const {
  sftp_attributes attr = sftp_fstat(m_handle);
  if (!attr) {
    throw Exception{m_handle->sftp};
  }
  return Attributes{std::move(attr)};
}

size_t File::Read(void* buf, uint32_t count) {
  auto rv = sftp_read(m_handle, buf, count);
  if (rv < 0) {
    throw Exception{m_handle->sftp};
  }
  return rv;
}

size_t File::Write(std::span<const uint8_t> data) {
  auto rv = sftp_write(m_handle, data.data(), data.size());
  if (rv < 0) {
    throw Exception{m_handle->sftp};
  }
  return rv;
}

void File::Seek(uint64_t offset) {
  if (sftp_seek64(m_handle, offset) < 0) {
    throw Exception{m_handle->sftp};
  }
}

uint64_t File::Tell() const {
  return sftp_tell64(m_handle);
}

void File::Rewind() {
  sftp_rewind(m_handle);
}

void File::Sync() {
  if (sftp_fsync(m_handle) < 0) {
    throw Exception{m_handle->sftp};
  }
}

Session::Session(std::string_view host, int port, std::string_view user,
                 std::string_view pass)
    : m_host{host}, m_port{port}, m_username{user}, m_password{pass} {
  // Create a new SSH session.
  m_session = ssh_new();
  if (!m_session) {
    throw Exception{"The SSH session could not be allocated."};
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

Session::~Session() {
  if (m_sftp) {
    sftp_free(m_sftp);
  }
  if (m_session) {
    ssh_free(m_session);
  }
}

void Session::Connect() {
  // Connect to the server.
  int rc = ssh_connect(m_session);
  if (rc != SSH_OK) {
    throw Exception{ssh_get_error(m_session)};
  }

  // Authenticate with password.
  rc = ssh_userauth_password(m_session, nullptr, m_password.c_str());
  if (rc != SSH_AUTH_SUCCESS) {
    throw Exception{ssh_get_error(m_session)};
  }

  // Allocate the SFTP session.
  m_sftp = sftp_new(m_session);
  if (!m_sftp) {
    throw Exception{ssh_get_error(m_session)};
  }

  // Initialize.
  rc = sftp_init(m_sftp);
  if (rc != SSH_OK) {
    sftp_free(m_sftp);
    m_sftp = nullptr;
    throw Exception{ssh_get_error(m_session)};
  }
}

void Session::Disconnect() {
  if (m_sftp) {
    sftp_free(m_sftp);
    m_sftp = nullptr;
  }
  ssh_disconnect(m_session);
}

std::vector<Attributes> Session::ReadDir(const std::string& path) {
  sftp_dir dir = sftp_opendir(m_sftp, path.c_str());
  if (!dir) {
    throw Exception{m_sftp};
  }

  std::vector<Attributes> rv;
  while (sftp_attributes attr = sftp_readdir(m_sftp, dir)) {
    rv.emplace_back(std::move(attr));
  }

  sftp_closedir(dir);
  return rv;
}

void Session::Unlink(const std::string& filename) {
  if (sftp_unlink(m_sftp, filename.c_str()) < 0) {
    throw Exception{m_sftp};
  }
}

File Session::Open(const std::string& filename, int accesstype, mode_t mode) {
  sftp_file f = sftp_open(m_sftp, filename.c_str(), accesstype, mode);
  if (!f) {
    throw Exception{m_sftp};
  }
  return File{std::move(f)};
}
