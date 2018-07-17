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
#include "wpi/uv/Async.h"
#include "wpi/uv/Loop.h"

using namespace wpi;

class EventLoopRunner::Thread : public SafeThread {
 public:
  Thread() : m_loop(uv::Loop::Create()) {
    // set up async handles
    if (!m_loop) return;

    // run function
    auto doExec = uv::Async::Create(m_loop);
    if (!doExec) return;
    m_doExec = doExec;
    doExec->wakeup.connect([ async = doExec.get(), this ]() {
      uv::Loop& loop = async->GetLoopRef();
      {
        std::lock_guard<wpi::mutex> lock{m_mutex};
        for (auto&& func : m_exec) func(loop);
        m_exec.clear();
      }
      m_execDone.notify_all();
    });

    // exit loop
    auto doExit = uv::Async::Create(m_loop);
    if (!doExit) return;
    m_doExit = doExit;
    doExit->wakeup.connect([async = doExit.get()]() {
      // close all handles; this will (eventually) stop the loop
      async->GetLoopRef().Walk([](uv::Handle& h) { h.Close(); });
    });
  }

  void Main() {
    if (m_loop) m_loop->Run();
  }

  // the loop
  std::shared_ptr<uv::Loop> m_loop;

  // run function
  std::weak_ptr<uv::Async> m_doExec;
  wpi::SmallVector<std::function<void(uv::Loop&)>, 4> m_exec;
  wpi::condition_variable m_execDone;

  // exit loop (thread cleanup)
  std::weak_ptr<uv::Async> m_doExit;
};

EventLoopRunner::EventLoopRunner() { m_owner.Start(new Thread); }

EventLoopRunner::~EventLoopRunner() {
  if (auto thr = m_owner.GetThread()) {
    if (auto doExit = thr->m_doExit.lock()) doExit->Send();
  }
}

void EventLoopRunner::ExecAsync(std::function<void(uv::Loop&)> func) {
  if (auto thr = m_owner.GetThread()) {
    if (auto doExec = thr->m_doExec.lock()) {
      thr->m_exec.emplace_back(func);
      doExec->Send();
    }
  }
}

void EventLoopRunner::ExecSync(std::function<void(uv::Loop&)> func) {
  if (auto thr = m_owner.GetThread()) {
    if (auto doExec = thr->m_doExec.lock()) {
      thr->m_exec.emplace_back(func);
      doExec->Send();
      thr->m_execDone.wait(thr.GetLock(), [&] { return thr->m_exec.empty(); });
    }
  }
}
