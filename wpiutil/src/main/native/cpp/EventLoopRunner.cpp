/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/EventLoopRunner.h"

#include "wpi/SmallVector.h"
#include "wpi/condition_variable.h"
#include "wpi/mutex.h"
#include "wpi/uv/AsyncFunction.h"
#include "wpi/uv/Loop.h"

using namespace wpi;

class EventLoopRunner::Thread : public SafeThread {
 public:
  using UvExecFunc = uv::AsyncFunction<void(LoopFunc)>;

  Thread() : m_loop(uv::Loop::Create()) {
    // set up async handles
    if (!m_loop) return;

    // run function
    m_doExec = UvExecFunc::Create(
        m_loop, [loop = m_loop.get()](auto out, LoopFunc func) {
          func(*loop);
          out.set_value();
        });
  }

  void Main() {
    if (m_loop) m_loop->Run();
  }

  // the loop
  std::shared_ptr<uv::Loop> m_loop;

  // run function
  std::weak_ptr<UvExecFunc> m_doExec;
};

EventLoopRunner::EventLoopRunner() { m_owner.Start(); }

EventLoopRunner::~EventLoopRunner() { Stop(); }

void EventLoopRunner::Stop() {
  ExecAsync([](uv::Loop& loop) {
    // close all handles; this will (eventually) stop the loop
    loop.Walk([](uv::Handle& h) { h.Close(); });
  });
  m_owner.Join();
}

void EventLoopRunner::ExecAsync(LoopFunc func) {
  if (auto thr = m_owner.GetThread()) {
    if (auto doExec = thr->m_doExec.lock()) {
      doExec->Call(func);
    }
  }
}

void EventLoopRunner::ExecSync(LoopFunc func) {
  wpi::future<void> f;
  if (auto thr = m_owner.GetThread()) {
    if (auto doExec = thr->m_doExec.lock()) {
      f = doExec->Call(func);
    }
  }
  if (f.valid()) f.wait();
}

std::shared_ptr<uv::Loop> EventLoopRunner::GetLoop() {
  if (auto thr = m_owner.GetThread()) return thr->m_loop;
  return nullptr;
}
