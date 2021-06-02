// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimHttpConnection.h"

#include <uv.h>

#include <wpi/MimeTypes.h>
#include <wpi/SmallVector.h>
#include <wpi/UrlParser.h>
#include <wpi/fs.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include <wpi/raw_uv_ostream.h>
#include <wpi/uv/Request.h>

namespace uv = wpi::uv;

using namespace wpilibws;

bool HALSimHttpConnection::IsValidWsUpgrade(wpi::StringRef protocol) {
  if (m_request.GetUrl() != m_server->GetServerUri()) {
    MySendError(404, "invalid websocket address");
    return false;
  }

  return true;
}

void HALSimHttpConnection::ProcessWsUpgrade() {
  m_websocket->open.connect_extended([this](auto conn, wpi::StringRef) {
    conn.disconnect();  // one-shot

    if (!m_server->RegisterWebsocket(shared_from_this())) {
      Log(409);
      m_websocket->Fail(409, "Only a single simulation websocket is allowed");
      return;
    }

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
      m_websocket->Fail(400, err);
      return;
    }
    m_server->OnNetValueChanged(j);
  });

  m_websocket->closed.connect([this](uint16_t, wpi::StringRef) {
    // unset the global, allow another websocket to connect
    if (m_isWsConnected) {
      wpi::errs() << "HALWebSim: websocket disconnected\n";
      m_isWsConnected = false;

      m_server->CloseWebsocket(shared_from_this());
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
  std::string fn = filename.str();
  std::error_code ec;

  // get file size
  auto size = fs::file_size(fn, ec);
  if (ec) {
    MySendError(404, "error getting file size");
    return;
  }

  // open file
  wpi::raw_fd_istream is{fn, ec, true};
  if (ec) {
    MySendError(404, "error opening file");
    return;
  }

  wpi::SmallVector<uv::Buffer, 4> toSend;
  wpi::raw_uv_ostream os{toSend, 4096};
  BuildHeader(os, code, codeText, contentType, size, extraHeader);
  SendData(os.bufs(), false);

  Log(code);

  // Read the file byte by byte
  wpi::SmallVector<uv::Buffer, 4> bodyData;
  wpi::raw_uv_ostream bodyOs{bodyData, 4096};

  std::string fileBuf;
  size_t oldSize = 0;

  while (fileBuf.size() < size) {
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
  if (url.HasPath()) {
    path = url.GetPath();
  }

  if (m_request.GetMethod() == wpi::HTTP_GET && path.startswith("/") &&
      !path.contains("..") && !path.contains("//")) {
    // convert to fs native representation
    fs::path nativePath;
    if (path.startswith("/user/")) {
      nativePath = fs::path{std::string{m_server->GetWebrootSys()}} /
                   fs::path{std::string{path.drop_front(6)},
                            fs::path::format::generic_format};
    } else {
      nativePath = fs::path{std::string{m_server->GetWebrootSys()}} /
                   fs::path{std::string{path.drop_front(1)},
                            fs::path::format::generic_format};
    }

    if (fs::is_directory(nativePath)) {
      nativePath.append("index.html");
    }

    if (!fs::exists(nativePath) || fs::is_directory(nativePath)) {
      MySendError(404, "Resource '" + path + "' not found");
    } else {
      auto contentType = wpi::MimeTypeFromPath(nativePath.string());
      SendFileResponse(200, "OK", contentType, nativePath.string());
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
