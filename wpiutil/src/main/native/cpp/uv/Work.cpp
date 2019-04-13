/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/uv/Work.h"

#include "wpi/uv/Loop.h"

namespace wpi {
namespace uv {

WorkReq::WorkReq() {
  error = [this](Error err) { GetLoop().error(err); };
}

void QueueWork(Loop& loop, const std::shared_ptr<WorkReq>& req) {
  int err = uv_queue_work(loop.GetRaw(), req->GetRaw(),
                          [](uv_work_t* req) {
                            auto& h = *static_cast<WorkReq*>(req->data);
                            h.work();
                          },
                          [](uv_work_t* req, int status) {
                            auto& h = *static_cast<WorkReq*>(req->data);
                            if (status < 0)
                              h.ReportError(status);
                            else
                              h.afterWork();
                            h.Release();  // this is always a one-shot
                          });
  if (err < 0)
    loop.ReportError(err);
  else
    req->Keep();
}

void QueueWork(Loop& loop, std::function<void()> work,
               std::function<void()> afterWork) {
  auto req = std::make_shared<WorkReq>();
  if (work) req->work.connect(work);
  if (afterWork) req->afterWork.connect(afterWork);
  QueueWork(loop, req);
}

}  // namespace uv
}  // namespace wpi
