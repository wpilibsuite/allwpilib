/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_REPEATPIPECONNECTOR_H_
#define WPIUTIL_WPI_UV_REPEATPIPECONNECTOR_H_

#include <wpi/raw_ostream.h>
#include <wpi/uv/Async.h>
#include <wpi/uv/Loop.h>
#include <wpi/uv/Pipe.h>
#include <wpi/uv/Timer.h>
#include <wpi/uv/util.h>

#include <memory>
#include <string>

namespace wpi {
namespace uv {

class RepeatPipeConnector
    : public std::enable_shared_from_this<RepeatPipeConnector> {
  struct private_init {};

 public:
  RepeatPipeConnector(Loop& loop, Timer::Time reconnectTime, bool ipc,
                      const private_init&)
      : m_reconnectTime{reconnectTime}, m_loop{loop}, m_ipc{ipc} {}

  static std::shared_ptr<RepeatPipeConnector> Create(Loop& loop,
                                                     Timer::Time reconnectTime,
                                                     bool ipc = false) {
    auto ptr = std::make_shared<RepeatPipeConnector>(loop, reconnectTime, ipc,
                                                     private_init{});
    ptr->m_this = ptr;
    return ptr;
  }

  static std::shared_ptr<RepeatPipeConnector> Create(
      const std::shared_ptr<Loop>& loop, Timer::Time reconnectTime,
      bool ipc = false) {
    return Create(*loop, reconnectTime, ipc);
  }

  void Start();

  void Close();

  sig::Signal<> closed;

  sig::Signal<Pipe&> connected;

  void SetAddress(const Twine& addr) {
    {
      std::lock_guard<wpi::mutex> lock(m_addressMutex);
      m_changeAddress = addr.str();
    }
    if (auto lock = m_asyncEnter.lock()) {
      lock->Send();
    }
  }

 private:
  void Connect();

  void Disconnect() {
    if (!m_connectingPipe) {
      Connect();
      return;
    }
    m_connectingPipe->Close();
  }

  std::weak_ptr<Async<>> m_asyncEnter;

  Timer::Time m_reconnectTime;

  std::string m_currentAddress;

  wpi::mutex m_addressMutex;
  std::string m_changeAddress;

  Pipe* m_connectingPipe{nullptr};
  Loop& m_loop;
  bool m_ipc;
  std::shared_ptr<RepeatPipeConnector> m_this;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_REPEATPIPECONNECTOR_H_
