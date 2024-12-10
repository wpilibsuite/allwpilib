// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/WebServer.h"

#include <unistd.h>

#include <memory>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <wpi/DenseMap.h>
#include <wpi/Signal.h>
#include <wpi/StringMap.h>
#include <wpi/json.h>
#include <wpi/fs.h>
#include <wpi/print.h>

#include "wpinet/EventLoopRunner.h"
#include "wpinet/HttpServerConnection.h"
#include "wpinet/HttpUtil.h"
#include "wpinet/UrlParser.h"
#include "wpinet/raw_uv_ostream.h"
#include "wpinet/uv/GetAddrInfo.h"
#include "wpinet/uv/Stream.h"
#include "wpinet/uv/Tcp.h"
#include "wpinet/uv/Timer.h"

using namespace wpi;

namespace {
class MyHttpConnection : public wpi::HttpServerConnection,
                         public std::enable_shared_from_this<MyHttpConnection> {
 public:
  explicit MyHttpConnection(std::shared_ptr<wpi::uv::Stream> stream,
                            std::string_view path)
      : HttpServerConnection{std::move(stream)}, m_path{path} {}

 protected:
  void ProcessRequest() override;
  void SendFileResponse(int code, std::string_view codeText,
                        std::string_view contentType, fs::path filename,
                        std::string_view extraHeader = {});

  std::string m_path;
};

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
}  // namespace

static void Sendfile(uv::Loop& loop, uv_file out, uv_file in, int64_t inOffset,
                     size_t len, std::function<void()> complete) {
  auto req = std::make_shared<SendfileReq>(out, in, inOffset, len);
  if (complete) {
    req->complete.connect(complete);
  }
  int err = req->Send(loop);
  if (err >= 0) {
    req->Keep();
  }
}

static std::string_view GetMimeType(std::string_view ext) {
  static const wpi::StringMap<std::string> map{
      {"css", "text/css"},
      {"csv", "text/csv"},
      {"gif", "image/gif"},
      {"htm", "text/html"},
      {"html", "text/html"},
      {"ico", "image/vnd.microsoft.icon"},
      {"jar", "application/java-archive"},
      {"jpeg", "image/jpeg"},
      {"jpg", "image/jpeg"},
      {"js", "text/javascript"},
      {"json", "text/json"},
      {"mjs", "text/javascript"},
      {"pdf", "application/pdf"},
      {"png", "image/png"},
      {"sh", "application/x-sh"},
      {"svg", "image/svg+xml"},
      {"txt", "text/plain"},
      {"webp", "image/webp"},
      {"xhtml", "application/xhtml+xml"},
      {"xml", "application/xml"},
      {"zip", "application/zip"},
  };
  auto it = map.find(ext);
  if (it == map.end()) {
    return "application/octet-stream";
  }
  return it->second;
}

void MyHttpConnection::SendFileResponse(int code, std::string_view codeText,
                                        std::string_view contentType,
                                        fs::path filename,
                                        std::string_view extraHeader) {
  // open file
  std::error_code ec;
  auto infile = fs::OpenFileForRead(filename, ec);
  if (ec) {
    SendError(404);
    return;
  }
  int infd = fs::FileToFd(infile, ec, fs::OF_None);
  if (ec) {
    fs::CloseFile(infile);
    SendError(404);
    return;
  }

  // get file size
  auto size = fs::file_size(filename, ec);
  if (ec) {
    SendError(404);
    ::close(infd);
    return;
  }

  uv_os_fd_t outfd;
  int err = uv_fileno(m_stream.GetRawHandle(), &outfd);
  if (err < 0) {
    m_stream.GetLoopRef().ReportError(err);
    SendError(404);
    ::close(infd);
    return;
  }

  wpi::SmallVector<uv::Buffer, 4> toSend;
  wpi::raw_uv_ostream os{toSend, 4096};
  BuildHeader(os, code, codeText, contentType, size, extraHeader);
  SendData(os.bufs(), false);

  // close after write completes if we aren't keeping alive
  // since we're using sendfile, set socket to blocking
  m_stream.SetBlocking(true);
  Sendfile(m_stream.GetLoopRef(), outfd, infd, 0, size,
           [infd, closeAfter = !m_keepAlive, stream = &m_stream] {
             ::close(infd);
             if (closeAfter) {
               stream->Close();
             } else {
               stream->SetBlocking(false);
             }
           });
}

void MyHttpConnection::ProcessRequest() {
  // fmt::print(stderr, "HTTP request: '{}'\n", m_request.GetUrl());
  wpi::UrlParser url{m_request.GetUrl(),
                     m_request.GetMethod() == wpi::HTTP_CONNECT};
  if (!url.IsValid()) {
    // failed to parse URL
    SendError(400);
    return;
  }

  std::string_view path;
  if (url.HasPath()) {
    path = url.GetPath();
  }
  // fmt::print(stderr, "path: \"{}\"\n", path);

  std::string_view query;
  if (url.HasQuery()) {
    query = url.GetQuery();
  }
  // fmt::print(stderr, "query: \"{}\"\n", query);
  HttpQueryMap qmap{query};

  const bool isGET = m_request.GetMethod() == wpi::HTTP_GET;
  if (isGET && wpi::starts_with(path, '/') && !wpi::contains(path, "..")) {
    fs::path fullpath = fmt::format("{}{}", m_path, path);
    std::error_code ec;
    bool isdir = fs::is_directory(fullpath, ec);
    if (isdir) {
      if (!wpi::ends_with(path, '/')) {
        // redirect to trailing / location
        SendResponse(301, "Moved Permanently", "text/plain", "",
                     fmt::format("Location: {}/\r\n\r\n", path));
        return;
      }
      // generate directory listing
      wpi::SmallString<64> formatBuf;
      if (qmap.Get("format", formatBuf).value_or("") == "json") {
        wpi::json j = wpi::json::array();
        for (auto&& entry : fs::directory_iterator{fullpath}) {
          bool subdir = entry.is_directory(ec);
          j.emplace_back(
              wpi::json{{"filename", entry.path().filename().string()},
                        {"size", subdir ? 0 : entry.file_size(ec)},
                        {"directory", subdir}});
        }
        SendResponse(200, "OK", "text/json", j.dump());
      } else {
        std::string html = fmt::format(
            "<html><head><title>{}</title></head><body>"
            "<table><tr><th>Name</th><th>Size</th></tr>\n",
            path);
        for (auto&& entry : fs::directory_iterator{fullpath}) {
          bool subdir = entry.is_directory(ec);
          html += fmt::format(
              "<tr><td><a href=\"{0}{1}\">{0}{1}</a></td><td>{2}</td></tr>",
              entry.path().filename().string(), subdir ? "/" : "",
              subdir ? "" : fmt::to_string(entry.file_size(ec)));
        }
        html += "</table></body></html>";
        SendResponse(200, "OK", "text/html", html);
      }
    } else {
      SendFileResponse(200, "OK", GetMimeType(wpi::rsplit(path, '.').second),
                       fullpath);
    }
  } else {
    SendError(404, "Resource not found");
  }
}

struct WebServer::Impl {
 public:
  EventLoopRunner runner;
  DenseMap<unsigned int, std::weak_ptr<uv::Tcp>> servers;
};

WebServer::WebServer() : m_impl{new Impl} {}

WebServer& WebServer::GetInstance() {
  static WebServer instance;
  return instance;
}

void WebServer::Start(unsigned int port, std::string_view path) {
  m_impl->runner.ExecSync([&](uv::Loop& loop) {
    auto server = uv::Tcp::Create(loop);
    if (!server) {
      wpi::print(stderr, "WebServer: Creating server failed\n");
      return;
    }

    // bind to local port
    server->Bind("", port);

    // when we get a connection, accept it
    server->connection.connect(
        [serverPtr = server.get(), path = std::string{path}] {
          auto client = serverPtr->Accept();
          if (!client) {
            wpi::print(stderr, "WebServer: Connecting to client failed\n");
            return;
          }

          // close on error
          client->error.connect([clientPtr = client.get()](uv::Error err) {
            clientPtr->Close();
          });

          auto conn = std::make_shared<MyHttpConnection>(client, path);
          client->SetData(conn);
        });

    // start listening for incoming connections
    server->Listen();

    m_impl->servers[port] = server;
  });
}

void WebServer::Stop(unsigned int port) {
  m_impl->runner.ExecSync([&](uv::Loop& loop) {
    if (auto server = m_impl->servers.lookup(port).lock()) {
      server->Close();
      m_impl->servers.erase(port);
    }
  });
}
