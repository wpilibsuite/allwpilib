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
#include <wpi/raw_ostream.h>
#include <wpi/raw_uv_ostream.h>
#include <wpi/uv/Request.h>

#include "HALSimWSServer.h"

namespace uv = wpi::uv;

namespace wpilibws {

bool HALSimHttpConnection::IsValidWsUpgrade(wpi::StringRef protocol) {
  auto hws = HALSimWeb::GetInstance();
  if (m_request.GetUrl() != hws->GetServerUri()) {
    MySendError(404, "invalid websocket address");
    return false;
  }

  return true;
}

void HALSimHttpConnection::ProcessWsUpgrade() {
  m_websocket->open.connect_extended([this](auto conn, wpi::StringRef) {
    conn.disconnect();  // one-shot

    m_buffers = std::make_unique<BufferPool>();
    m_exec =
        UvExecFunc::Create(m_stream.GetLoop(), [](auto out, LoopFunc func) {
          func();
          out.set_value();
        });

    auto hws = HALSimWeb::GetInstance();
    if (!hws) {
      Log(503);
      m_websocket->Fail(503, "HALSimWeb unavailable");
      return;
    }

    if (!hws->RegisterWebsocket(GetSharedFromThis())) {
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
    auto hws = HALSimWeb::GetInstance();
    if (!m_isWsConnected || !hws) {
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
    hws->OnNetValueChanged(j);
  });

  m_websocket->closed.connect([this](uint16_t, wpi::StringRef) {
    // unset the global, allow another websocket to connect
    if (m_isWsConnected) {
      wpi::errs() << "HALWebSim: websocket disconnected\n";
      m_isWsConnected = false;

      auto hws = HALSimWeb::GetInstance();
      if (hws) {
        hws->CloseWebsocket(GetSharedFromThis());
      }
    }
  });
}

void HALSimHttpConnection::OnSimValueChanged(const wpi::json& msg) {
  // render json to buffers
  wpi::SmallVector<uv::Buffer, 4> sendBufs;
  wpi::raw_uv_ostream os{sendBufs, [this]() -> uv::Buffer {
                           std::lock_guard lock(m_buffers_mutex);
                           return m_buffers->Allocate();
                         }};
  os << msg;

  // call the websocket send function on the uv loop
  m_exec->Call([this, sendBufs]() mutable {
    m_websocket->SendText(sendBufs, [this](auto bufs, wpi::uv::Error err) {
      {
        std::lock_guard lock(m_buffers_mutex);
        m_buffers->Release(bufs);
      }

      if (err) {
        wpi::errs() << err.str() << "\n";
        wpi::errs().flush();
      }
    });
  });
}

class SendfileReq : public uv::RequestImpl<SendfileReq, uv_fs_t> {
 public:
  SendfileReq(uv_file out, uv_file in, int64_t inOffset, size_t len)
      : m_out(out), m_in(in), m_inOffset(inOffset), m_len(len) {
    error = [this](uv::Error err) { GetLoop().error(err); };
  }

  uv::Loop& GetLoop() const {
    return *static_cast<uv::Loop*>(GetRaw()->loop->data);
  }

  int Send(uv::Loop& loop) {
    int err = uv_fs_sendfile(loop.GetRaw(), GetRaw(), m_out, m_in, m_inOffset,
                             m_len, [](uv_fs_t* req) {
                               auto& h = *static_cast<SendfileReq*>(req->data);
                               if (req->result < 0) {
                                 h.ReportError(req->result);
                                 h.complete();
                                 h.Release();
                                 return;
                               }

                               h.m_inOffset += req->result;
                               h.m_len -= req->result;
                               if (h.m_len == 0) {
                                 // done
                                 h.complete();
                                 h.Release();  // this is always a one-shot
                                 return;
                               }

                               // need to send more
                               h.Send(h.GetLoop());
                             });
    if (err < 0) {
      ReportError(err);
      complete();
    }
    return err;
  }

  wpi::sig::Signal<> complete;

 private:
  uv_file m_out;
  uv_file m_in;
  int64_t m_inOffset;
  size_t m_len;
};

void Sendfile(uv::Loop& loop, uv_file out, uv_file in, int64_t inOffset,
              size_t len, std::function<void()> complete) {
  auto req = std::make_shared<SendfileReq>(out, in, inOffset, len);
  if (complete) req->complete.connect(complete);
  int err = req->Send(loop);
  if (err >= 0) req->Keep();
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

  // close after write completes if we aren't keeping alive
  // since we're using sendfile, set socket to blocking
  m_stream.SetBlocking(true);
  Sendfile(m_stream.GetLoopRef(), uv_open_osfhandle(outfd), infd, 0,
           status.getSize(),
           [infd, closeAfter = !m_keepAlive, stream = &m_stream] {
             wpi::sys::fs::file_t file = uv_get_osfhandle(infd);
             wpi::sys::fs::closeFile(file);

             if (closeAfter)
               stream->Close();
             else
               stream->SetBlocking(false);
           });
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
    wpi::sys::path::native(path, nativePath, wpi::sys::path::Style::posix);

    if (path.startswith("/user/")) {
      std::string prefix = (wpi::sys::path::get_separator() + "user" +
                            wpi::sys::path::get_separator())
                               .str();
      wpi::sys::path::replace_path_prefix(nativePath, prefix, m_webroot_user);
    } else {
      wpi::sys::path::replace_path_prefix(
          nativePath, wpi::sys::path::get_separator(), m_webroot_sys);
    }

    if (wpi::sys::fs::is_directory(nativePath)) {
      wpi::sys::path::append(nativePath, "index.html");
    }

    if (!wpi::sys::fs::exists(nativePath) ||
        wpi::sys::fs::is_directory(nativePath)) {
      MySendError(404, "Resource '" + path + "' not found");
    } else {
      auto contentType = wpi::MimeTypeFromPath(path);
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

}  // namespace wpilibws
