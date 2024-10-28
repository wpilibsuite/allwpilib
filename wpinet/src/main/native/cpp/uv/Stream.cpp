// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Stream.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <wpi/Logger.h>
#include <wpi/SmallVector.h>
#include <wpi/raw_ostream.h>

using namespace wpi;
using namespace wpi::uv;

namespace {
class CallbackWriteReq : public WriteReq {
 public:
  CallbackWriteReq(std::span<const Buffer> bufs,
                   std::function<void(std::span<Buffer>, Error)> callback)
      : m_bufs{bufs.begin(), bufs.end()} {
    finish.connect(
        [this, f = std::move(callback)](Error err) { f(m_bufs, err); });
  }

 private:
  SmallVector<Buffer, 4> m_bufs;
};
}  // namespace

namespace wpi::uv {

ShutdownReq::ShutdownReq() {
  error = [this](Error err) { GetStream().error(err); };
}

WriteReq::WriteReq() {
  error = [this](Error err) { GetStream().error(err); };
}

void Stream::Shutdown(const std::shared_ptr<ShutdownReq>& req) {
  if (IsLoopClosing()) {
    return;
  }
  if (Invoke(&uv_shutdown, req->GetRaw(), GetRawStream(),
             [](uv_shutdown_t* req, int status) {
               auto& h = *static_cast<ShutdownReq*>(req->data);
               if (status < 0) {
                 h.ReportError(status);
               } else {
                 h.complete();
               }
               h.Release();  // this is always a one-shot
             })) {
    req->Keep();
  }
}

void Stream::Shutdown(std::function<void()> callback) {
  if (IsLoopClosing()) {
    return;
  }
  auto req = std::make_shared<ShutdownReq>();
  if (callback) {
    req->complete.connect(std::move(callback));
  }
  Shutdown(req);
}

void Stream::StartRead() {
  if (IsLoopClosing()) {
    return;
  }
  Invoke(&uv_read_start, GetRawStream(), &Handle::AllocBuf,
         [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
           auto& h = *static_cast<Stream*>(stream->data);
           Buffer data = *buf;

           // nread=0 is simply ignored
           if (nread == UV_EOF) {
             h.end();
           } else if (nread > 0) {
             h.data(data, static_cast<size_t>(nread));
           } else if (nread < 0) {
             h.ReportError(nread);
           }

           // free the buffer
           h.FreeBuf(data);
         });
}

static std::string BufsToString(std::span<const Buffer> bufs) {
  std::string str;
  wpi::raw_string_ostream stros{str};
  size_t count = 0;
  for (auto buf : bufs) {
    for (auto ch : buf.bytes()) {
      stros << fmt::format("{:02x},", static_cast<unsigned int>(ch) & 0xff);
      if (count++ > 30) {
        goto extra;
      }
    }
  }
  goto done;
extra: {
  size_t total = 0;
  for (auto buf : bufs) {
    total += buf.len;
  }
  stros << fmt::format("... (total {})", total);
}
done:
  return str;
}

void Stream::Write(std::span<const Buffer> bufs,
                   const std::shared_ptr<WriteReq>& req) {
  if (IsLoopClosing()) {
    return;
  }
  if (auto logger = GetLogger()) {
    WPI_DEBUG4(*logger, "uv::Write({})", BufsToString(bufs));
  }
  if (Invoke(&uv_write, req->GetRaw(), GetRawStream(), bufs.data(), bufs.size(),
             [](uv_write_t* r, int status) {
               auto& h = *static_cast<WriteReq*>(r->data);
               if (status < 0) {
                 h.ReportError(status);
               }
               auto ptr = h.Release();  // one-shot, but finish() may Keep()
               h.finish(Error(status));
             })) {
    req->Keep();
  }
}

void Stream::Write(std::span<const Buffer> bufs,
                   std::function<void(std::span<Buffer>, Error)> callback) {
  Write(bufs, std::make_shared<CallbackWriteReq>(bufs, std::move(callback)));
}

int Stream::TryWrite(std::span<const Buffer> bufs) {
  if (IsLoopClosing()) {
    return UV_ECANCELED;
  }
  if (auto logger = GetLogger()) {
    WPI_DEBUG4(*logger, "uv::TryWrite({})", BufsToString(bufs));
  }
  int val = uv_try_write(GetRawStream(), bufs.data(), bufs.size());
  if (val == UV_EAGAIN) {
    return 0;
  }
  if (val < 0) {
    this->ReportError(val);
    return val;
  }
  return val;
}

int Stream::TryWrite2(std::span<const Buffer> bufs, Stream& send) {
  if (IsLoopClosing()) {
    return UV_ECANCELED;
  }
  int val = uv_try_write2(GetRawStream(), bufs.data(), bufs.size(),
                          send.GetRawStream());
  if (val == UV_EAGAIN) {
    return 0;
  }
  if (val < 0) {
    this->ReportError(val);
    return val;
  }
  return val;
}

}  // namespace wpi::uv
