// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimHttpConnection.h"

#include <uv.h>

#include <cstdio>
#include <string>
#include <string_view>

#include <wpi/MemoryBuffer.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/print.h>
#include <wpinet/MimeTypes.h>
#include <wpinet/UrlParser.h>
#include <wpinet/raw_uv_ostream.h>
#include <wpinet/uv/Request.h>

namespace uv = wpi::uv;

using namespace wpilibws;

bool HALSimHttpConnection::IsValidWsUpgrade(std::string_view protocol) {
  if (m_request.GetUrl() != m_server->GetServerUri()) {
    MySendError(404, "invalid websocket address");
    return false;
  }

  return true;
}

void HALSimHttpConnection::ProcessWsUpgrade() {
  m_websocket->open.connect_extended([this](auto conn, auto) {
    conn.disconnect();  // one-shot

    if (!m_server->RegisterWebsocket(shared_from_this())) {
      Log(409);
      m_websocket->Fail(409, "Only a single simulation websocket is allowed");
      return;
    }

    Log(200);
    m_isWsConnected = true;
    std::fputs("HALWebSim: websocket connected\n", stderr);
  });

  // parse incoming JSON, dispatch to parent
  m_websocket->text.connect([this](auto msg, bool) {
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

  m_websocket->closed.connect([this](uint16_t, auto) {
    // unset the global, allow another websocket to connect
    if (m_isWsConnected) {
      std::fputs("HALWebSim: websocket disconnected\n", stderr);
      m_isWsConnected = false;

      m_server->CloseWebsocket(shared_from_this());
    }
  });
}

void HALSimHttpConnection::OnSimValueChanged(const wpi::json& msg) {
  // Skip sending if this message is not in the allowed filter list
  try {
    auto& type = msg.at("type").get_ref<const std::string&>();
    if (!m_server->CanSendMessage(type)) {
      return;
    }
  } catch (wpi::json::exception& e) {
    wpi::print(stderr, "Error with message: {}\n", e.what());
  }

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
                                    wpi::print(stderr, "{}\n", err.str());
                                    std::fflush(stderr);
                                  }
                                });
  });
}

void HALSimHttpConnection::SendFileResponse(int code, std::string_view codeText,
                                            std::string_view contentType,
                                            std::string_view filename,
                                            std::string_view extraHeader) {
  std::error_code ec;

  // get file size
  auto size = fs::file_size(filename, ec);
  if (ec) {
    MySendError(404, "error getting file size");
    return;
  }

  // open file
  auto fileBuffer = wpi::MemoryBuffer::GetFile(filename);
  if (!fileBuffer) {
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

  bodyOs << fileBuffer.value()->GetBuffer();

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

  std::string_view path;
  if (url.HasPath()) {
    path = url.GetPath();
  }

  if (m_request.GetMethod() == wpi::HTTP_GET && wpi::starts_with(path, '/') &&
      !wpi::contains(path, "..") && !wpi::contains(path, "//")) {
    // convert to fs native representation
    fs::path nativePath;
    if (auto userPath = wpi::remove_prefix(path, "/user/")) {
      nativePath = fs::path{m_server->GetWebrootSys()} /
                   fs::path{*userPath, fs::path::format::generic_format};
    } else {
      nativePath =
          fs::path{m_server->GetWebrootSys()} /
          fs::path{wpi::drop_front(path), fs::path::format::generic_format};
    }

    if (fs::is_directory(nativePath)) {
      nativePath.append("index.html");
    }

    if (!fs::exists(nativePath) || fs::is_directory(nativePath)) {
      MySendError(404, fmt::format("Resource '{}' not found", path));
    } else {
      auto contentType = wpi::MimeTypeFromPath(nativePath.string());
      SendFileResponse(200, "OK", contentType, nativePath.string());
    }
  } else {
    MySendError(404, "Resource not found");
  }
}

void HALSimHttpConnection::MySendError(int code, std::string_view message) {
  Log(code);
  SendError(code, message);
}

void HALSimHttpConnection::Log(int code) {
  auto method = wpi::http_method_str(m_request.GetMethod());
  wpi::print(stderr, "{} {} HTTP/{}.{} {}\n", method, m_request.GetUrl(),
             m_request.GetMajor(), m_request.GetMinor(), code);
}
