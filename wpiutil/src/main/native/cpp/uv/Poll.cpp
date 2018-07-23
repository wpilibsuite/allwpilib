/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Poll.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

std::shared_ptr<Poll> Poll::Create(Loop& loop, int fd) {
  auto h = std::make_shared<Poll>(private_init{});
  int err = uv_poll_init(loop.GetRaw(), h->GetRaw(), fd);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

std::shared_ptr<Poll> Poll::CreateSocket(Loop& loop, uv_os_sock_t sock) {
  auto h = std::make_shared<Poll>(private_init{});
  int err = uv_poll_init_socket(loop.GetRaw(), h->GetRaw(), sock);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Poll::Start(int events) {
  Invoke(&uv_poll_start, GetRaw(), events,
         [](uv_poll_t* handle, int status, int events) {
           Poll& h = *static_cast<Poll*>(handle->data);
           if (status < 0)
             h.ReportError(status);
           else
             h.pollEvent(events);
         });
}

}  // namespace uv
}  // namespace wpi
