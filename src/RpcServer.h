/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_RPCSERVER_H_
#define NT_RPCSERVER_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "llvm/DenseMap.h"
#include "atomic_static.h"
#include "Message.h"
#include "ntcore_cpp.h"

namespace nt {

class RpcServer {
  friend class RpcServerTest;
 public:
  static RpcServer& GetInstance() {
    ATOMIC_STATIC(RpcServer, instance);
    return instance;
  }
  ~RpcServer();

  typedef std::function<void(std::shared_ptr<Message>)> SendMsgFunc;

  void Start();
  void Stop();

  bool active() const { return m_active; }

  void ProcessRpc(StringRef name, std::shared_ptr<Message> msg,
                  RpcCallback func, unsigned int conn_id,
                  SendMsgFunc send_response);

  bool PollRpc(bool blocking, RpcCallInfo* call_info);
  void PostRpcResponse(unsigned int rpc_id, unsigned int call_uid,
                       llvm::StringRef result);

 private:
  RpcServer();

  void ThreadMain();

  std::atomic_bool m_active;
  std::atomic_bool m_terminating;

  std::mutex m_mutex;
  std::condition_variable m_call_cond, m_poll_cond;

  struct RpcCall {
    RpcCall(StringRef name_, std::shared_ptr<Message> msg_, RpcCallback func_,
            unsigned int conn_id_, SendMsgFunc send_response_)
        : name(name_),
          msg(msg_),
          func(func_),
          conn_id(conn_id_),
          send_response(send_response_) {}

    std::string name;
    std::shared_ptr<Message> msg;
    RpcCallback func;
    unsigned int conn_id;
    SendMsgFunc send_response;
  };
  std::queue<RpcCall> m_call_queue, m_poll_queue;

  llvm::DenseMap<std::pair<unsigned int, unsigned int>, SendMsgFunc>
      m_response_map;

  std::thread m_thread;
  std::mutex m_shutdown_mutex;
  std::condition_variable m_shutdown_cv;
  bool m_shutdown = false;

  ATOMIC_STATIC_DECL(RpcServer)
};

}  // namespace nt

#endif  // NT_RPCSERVER_H_
