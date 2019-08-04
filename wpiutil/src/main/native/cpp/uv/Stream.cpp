/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Stream.h"

#include "wpi/SmallVector.h"

using namespace wpi;
using namespace wpi::uv;

namespace {
class CallbackWriteReq : public WriteReq {
 public:
  CallbackWriteReq(ArrayRef<Buffer> bufs,
                   std::function<void(MutableArrayRef<Buffer>, Error)> callback)
      : m_bufs{bufs.begin(), bufs.end()} {
    finish.connect([=](Error err) { callback(m_bufs, err); });
  }

 private:
  SmallVector<Buffer, 4> m_bufs;
};
}  // namespace

namespace wpi {
namespace uv {

ShutdownReq::ShutdownReq() {
  error = [this](Error err) { GetStream().error(err); };
}

WriteReq::WriteReq() {
  error = [this](Error err) { GetStream().error(err); };
}

void Stream::Shutdown(const std::shared_ptr<ShutdownReq>& req) {
  if (Invoke(&uv_shutdown, req->GetRaw(), GetRawStream(),
             [](uv_shutdown_t* req, int status) {
               auto& h = *static_cast<ShutdownReq*>(req->data);
               if (status < 0)
                 h.ReportError(status);
               else
                 h.complete();
               h.Release();  // this is always a one-shot
             }))
    req->Keep();
}

void Stream::Shutdown(std::function<void()> callback) {
  auto req = std::make_shared<ShutdownReq>();
  if (callback) req->complete.connect(callback);
  Shutdown(req);
}

void Stream::StartRead() {
  Invoke(&uv_read_start, GetRawStream(), &Handle::AllocBuf,
         [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
           auto& h = *static_cast<Stream*>(stream->data);
           Buffer data = *buf;

           // nread=0 is simply ignored
           if (nread == UV_EOF)
             h.end();
           else if (nread > 0)
             h.data(data, static_cast<size_t>(nread));
           else if (nread < 0)
             h.ReportError(nread);

           // free the buffer
           h.FreeBuf(data);
         });
}

void Stream::Write(ArrayRef<Buffer> bufs,
                   const std::shared_ptr<WriteReq>& req) {
  if (Invoke(&uv_write, req->GetRaw(), GetRawStream(), bufs.data(), bufs.size(),
             [](uv_write_t* r, int status) {
               auto& h = *static_cast<WriteReq*>(r->data);
               if (status < 0) h.ReportError(status);
               h.finish(Error(status));
               h.Release();  // this is always a one-shot
             }))
    req->Keep();
}

void Stream::Write(
    ArrayRef<Buffer> bufs,
    std::function<void(MutableArrayRef<Buffer>, Error)> callback) {
  Write(bufs, std::make_shared<CallbackWriteReq>(bufs, callback));
}

int Stream::TryWrite(ArrayRef<Buffer> bufs) {
  int val = uv_try_write(GetRawStream(), bufs.data(), bufs.size());
  if (val < 0) {
    this->ReportError(val);
    return 0;
  }
  return val;
}

}  // namespace uv
}  // namespace wpi
