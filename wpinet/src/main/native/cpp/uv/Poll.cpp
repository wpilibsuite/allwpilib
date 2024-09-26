// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Poll.h"

#include <functional>
#include <memory>
#include <utility>

#include "wpinet/uv/Loop.h"

namespace wpi::uv {

std::shared_ptr<Poll> Poll::Create(Loop& loop, int fd) {
  if (loop.IsClosing()) {
    return nullptr;
  }
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
  if (loop.IsClosing()) {
    return nullptr;
  }
  auto h = std::make_shared<Poll>(private_init{});
  int err = uv_poll_init_socket(loop.GetRaw(), h->GetRaw(), sock);
  if (err < 0) {
    loop.ReportError(err);
    return nullptr;
  }
  h->Keep();
  return h;
}

void Poll::Reuse(int fd, std::function<void()> callback) {
  if (IsLoopClosing() || IsClosing()) {
    return;
  }
  if (!m_reuseData) {
    m_reuseData = std::make_unique<ReuseData>();
  }
  m_reuseData->callback = std::move(callback);
  m_reuseData->isSocket = false;
  m_reuseData->fd = fd;
  uv_close(GetRawHandle(), [](uv_handle_t* handle) {
    Poll& h = *static_cast<Poll*>(handle->data);
    if (!h.m_reuseData || h.m_reuseData->isSocket) {
      return;  // just in case
    }
    auto data = std::move(h.m_reuseData);
    int err = uv_poll_init(h.GetLoopRef().GetRaw(), h.GetRaw(), data->fd);
    if (err < 0) {
      h.ReportError(err);
      return;
    }
    data->callback();
  });
}

void Poll::ReuseSocket(uv_os_sock_t sock, std::function<void()> callback) {
  if (IsLoopClosing() || IsClosing()) {
    return;
  }
  if (!m_reuseData) {
    m_reuseData = std::make_unique<ReuseData>();
  }
  m_reuseData->callback = std::move(callback);
  m_reuseData->isSocket = true;
  m_reuseData->sock = sock;
  uv_close(GetRawHandle(), [](uv_handle_t* handle) {
    Poll& h = *static_cast<Poll*>(handle->data);
    if (!h.m_reuseData || !h.m_reuseData->isSocket) {
      return;  // just in case
    }
    auto data = std::move(h.m_reuseData);
    int err = uv_poll_init(h.GetLoopRef().GetRaw(), h.GetRaw(), data->sock);
    if (err < 0) {
      h.ReportError(err);
      return;
    }
    data->callback();
  });
}

void Poll::Start(int events) {
  if (IsLoopClosing()) {
    return;
  }
  Invoke(&uv_poll_start, GetRaw(), events,
         [](uv_poll_t* handle, int status, int events) {
           Poll& h = *static_cast<Poll*>(handle->data);
           if (status < 0) {
             h.ReportError(status);
           } else {
             h.pollEvent(events);
           }
         });
}

}  // namespace wpi::uv
