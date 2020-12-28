// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/uv/NetworkStream.h"

namespace wpi::uv {

ConnectReq::ConnectReq() {
  error = [this](Error err) { GetStream().error(err); };
}

void NetworkStream::Listen(int backlog) {
  Invoke(&uv_listen, GetRawStream(), backlog,
         [](uv_stream_t* handle, int status) {
           auto& h = *static_cast<NetworkStream*>(handle->data);
           if (status < 0) {
             h.ReportError(status);
           } else {
             h.connection();
           }
         });
}

void NetworkStream::Listen(std::function<void()> callback, int backlog) {
  connection.connect(callback);
  Listen(backlog);
}

}  // namespace wpi::uv
