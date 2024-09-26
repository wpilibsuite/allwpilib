// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/uv/Work.h"

#include <functional>
#include <memory>
#include <utility>

#include "wpinet/uv/Loop.h"

namespace wpi::uv {

WorkReq::WorkReq() {
  error = [this](Error err) { GetLoop().error(err); };
}

void QueueWork(Loop& loop, const std::shared_ptr<WorkReq>& req) {
  if (loop.IsClosing()) {
    return;
  }
  int err = uv_queue_work(
      loop.GetRaw(), req->GetRaw(),
      [](uv_work_t* req) {
        auto& h = *static_cast<WorkReq*>(req->data);
        h.work();
      },
      [](uv_work_t* req, int status) {
        auto& h = *static_cast<WorkReq*>(req->data);
        if (status < 0) {
          h.ReportError(status);
        } else {
          h.afterWork();
        }
        h.Release();  // this is always a one-shot
      });
  if (err < 0) {
    loop.ReportError(err);
  } else {
    req->Keep();
  }
}

void QueueWork(Loop& loop, std::function<void()> work,
               std::function<void()> afterWork) {
  if (loop.IsClosing()) {
    return;
  }
  auto req = std::make_shared<WorkReq>();
  if (work) {
    req->work.connect(std::move(work));
  }
  if (afterWork) {
    req->afterWork.connect(std::move(afterWork));
  }
  QueueWork(loop, req);
}

}  // namespace wpi::uv
