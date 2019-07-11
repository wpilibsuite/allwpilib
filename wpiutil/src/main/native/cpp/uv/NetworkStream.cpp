/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/NetworkStream.h"

namespace wpi {
namespace uv {

ConnectReq::ConnectReq() {
  error = [this](Error err) { GetStream().error(err); };
}

void NetworkStream::Listen(int backlog) {
  Invoke(&uv_listen, GetRawStream(), backlog,
         [](uv_stream_t* handle, int status) {
           auto& h = *static_cast<NetworkStream*>(handle->data);
           if (status < 0)
             h.ReportError(status);
           else
             h.connection();
         });
}

void NetworkStream::Listen(std::function<void()> callback, int backlog) {
  connection.connect(callback);
  Listen(backlog);
}

}  // namespace uv
}  // namespace wpi
