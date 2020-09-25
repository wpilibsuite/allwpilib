/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimHttpConnection.h"

#include <uv.h>

#include <wpi/FileSystem.h>
#include <wpi/MimeTypes.h>
#include <wpi/Path.h>
#include <wpi/SmallVector.h>
#include <wpi/UrlParser.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include <wpi/raw_uv_ostream.h>
#include <wpi/uv/Request.h>

namespace uv = wpi::uv;

using namespace wpilibws;

bool HALSimHttpConnection::IsValidWsUpgrade(wpi::StringRef protocol) {
  wpi::errs() << "Request URL " << m_request.GetUrl() << "\n";
  if (!m_request.GetUrl().startswith(m_server->GetServerUri()) &&
      m_request.GetUrl().count("/") > 1) {
    MySendError(404, "invalid websocket address");
    return false;
  }

  if (!m_server->AcceptableWebsocket(m_request.GetUrl())) {
    MySendError(409, "Identifier already in use.");
    return false;
  }

  return true;
}

void HALSimHttpConnection::ProcessWsUpgrade() {
  m_websocket->open.connect_extended([this](auto conn, wpi::StringRef) {
    conn.disconnect();  // one-shot

    // Can ignore the return result because IsValidWsUpgrade validates this
    // already
    m_server->RegisterWebsocket(m_request.GetUrl(), shared_from_this());

    Log(200);
    m_isWsConnected = true;
    wpi::errs() << "HALWebSim: websocket connected\n";
  });

  // parse incoming JSON, dispatch to parent
  m_websocket->text.connect([this](wpi::StringRef msg, bool) {
    if (!m_isWsConnected) {
      return;
    }

    wpi::json j;
    try {
      j = wpi::json::parse(msg);
    } catch (const wpi::json::parse_error& e) {
      std::string err("JSON parse failed: ");
      err += e.what();
      m_websocket->Fail(1002, err);
      return;
    }
    m_server->OnNetValueChanged(j);
  });

  m_websocket->closed.connect([this](uint16_t, wpi::StringRef) {
    // unset the global, allow another websocket to connect
    if (m_isWsConnected) {
      wpi::errs() << "HALWebSim: websocket disconnected\n";
      m_isWsConnected = false;

      m_server->CloseWebsocket(m_request.GetUrl());
    }
  });
}

void HALSimHttpConnection::OnSimValueChanged(const wpi::json& msg) {
  // render json to buffers
  wpi::SmallVector<uv::Buffer, 4> sendBufs;
  wpi::raw_uv_ostream os{sendBufs, [this]() -> uv::Buffer {
                           std::lock_guard lock(m_buffers_mutex);
                           return m_buffers.Allocate();
                         }};
  os << msg;

  // call the websocket send function on the uv loop
  m_server->GetExec().Send([self = shared_from_this(), sendBufs] {
    self->m_websocket->SendText(sendBufs,
                                [self](auto bufs, wpi::uv::Error err) {
                                  {
                                    std::lock_guard lock(self->m_buffers_mutex);
                                    self->m_buffers.Release(bufs);
                                  }

                                  if (err) {
                                    wpi::errs() << err.str() << "\n";
                                    wpi::errs().flush();
                                  }
                                });
  });
}

void HALSimHttpConnection::SendFileResponse(int code,
                                            const wpi::Twine& codeText,
                                            const wpi::Twine& contentType,
                                            const wpi::Twine& filename,
                                            const wpi::Twine& extraHeader) {
  // open file
  int infd;
  if (wpi::sys::fs::openFileForRead(filename, infd)) {
    MySendError(404, "error opening file");
    return;
  }

  // get status (to get file size)
  wpi::sys::fs::file_status status;
  if (wpi::sys::fs::status(infd, status)) {
    MySendError(404, "error getting file size");
    wpi::sys::fs::file_t file = uv_get_osfhandle(infd);
    wpi::sys::fs::closeFile(file);
    return;
  }

  uv_os_fd_t outfd;
  int err = uv_fileno(m_stream.GetRawHandle(), &outfd);
  if (err < 0) {
    m_stream.GetLoopRef().ReportError(err);
    MySendError(404, "error getting fd");
    wpi::sys::fs::file_t file = uv_get_osfhandle(infd);
    wpi::sys::fs::closeFile(file);
    return;
  }

  wpi::SmallVector<uv::Buffer, 4> toSend;
  wpi::raw_uv_ostream os{toSend, 4096};
  BuildHeader(os, code, codeText, contentType, status.getSize(), extraHeader);
  SendData(os.bufs(), false);

  Log(code);

  // Read the file byte by byte
  wpi::SmallVector<uv::Buffer, 4> bodyData;
  wpi::raw_uv_ostream bodyOs{bodyData, 4096};

  wpi::raw_fd_istream is{infd, true};
  std::string fileBuf;
  size_t oldSize = 0;

  while (fileBuf.size() < status.getSize()) {
    oldSize = fileBuf.size();
    fileBuf.resize(oldSize + 1);
    is.read(&(*fileBuf.begin()) + oldSize, 1);
  }

  bodyOs << fileBuf;

  SendData(bodyOs.bufs(), false);
  if (!m_keepAlive) {
    m_stream.Close();
  }
}

void HALSimHttpConnection::ProcessRequest() {
  wpi::UrlParser url{m_request.GetUrl(),
                     m_request.GetMethod() == wpi::HTTP_CONNECT};
  if (!url.IsValid()) {
    // failed to parse URL
    MySendError(400, "Invalid URL");
    return;
  }

  wpi::StringRef path;
  if (url.HasPath()) path = url.GetPath();

  if (m_request.GetMethod() == wpi::HTTP_GET && path.startswith("/") &&
      !path.contains("..")) {
    // convert to fs native representation
    wpi::SmallVector<char, 32> nativePath;
    wpi::sys::path::native(path, nativePath);

    if (path.startswith("/user/")) {
      std::string prefix = (wpi::sys::path::get_separator() + "user" +
                            wpi::sys::path::get_separator())
                               .str();
      wpi::sys::path::replace_path_prefix(nativePath, prefix,
                                          m_server->GetWebrootUser());
    } else {
      wpi::sys::path::replace_path_prefix(nativePath,
                                          wpi::sys::path::get_separator(),
                                          m_server->GetWebrootSys());
    }

    if (wpi::sys::fs::is_directory(nativePath)) {
      wpi::sys::path::append(nativePath, "index.html");
    }

    if (!wpi::sys::fs::exists(nativePath) ||
        wpi::sys::fs::is_directory(nativePath)) {
      MySendError(404, "Resource '" + path + "' not found");
    } else {
      auto contentType = wpi::MimeTypeFromPath(wpi::Twine(nativePath).str());
      SendFileResponse(200, "OK", contentType, nativePath);
    }
  } else {
    MySendError(404, "Resource not found");
  }
}

void HALSimHttpConnection::MySendError(int code, const wpi::Twine& message) {
  Log(code);
  SendError(code, message);
}

void HALSimHttpConnection::Log(int code) {
  auto method = wpi::http_method_str(m_request.GetMethod());
  wpi::errs() << method << " " << m_request.GetUrl() << " HTTP/"
              << m_request.GetMajor() << "." << m_request.GetMinor() << " "
              << code << "\n";
}
