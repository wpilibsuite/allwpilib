/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RpcServer.h"

#include <queue>

#include "Log.h"

using namespace nt;

ATOMIC_STATIC_INIT(RpcServer)

class RpcServer::Thread : public SafeThread {
 public:
  Thread(std::function<void()> on_start, std::function<void()> on_exit)
      : m_on_start(on_start), m_on_exit(on_exit) {}

  void Main();

  std::queue<RpcCall> m_call_queue;

  std::function<void()> m_on_start;
  std::function<void()> m_on_exit;
};

RpcServer::RpcServer() {
  m_terminating = false;
}

RpcServer::~RpcServer() {
  Logger::GetInstance().SetLogger(nullptr);
  m_terminating = true;
  m_poll_cond.notify_all();
}

void RpcServer::Start() {
  auto thr = m_owner.GetThread();
  if (!thr) m_owner.Start(new Thread(m_on_start, m_on_exit));
}

void RpcServer::Stop() { m_owner.Stop(); }

void RpcServer::ProcessRpc(StringRef name, std::shared_ptr<Message> msg,
                           RpcCallback func, unsigned int conn_id,
                           SendMsgFunc send_response) {
  if (func) {
    auto thr = m_owner.GetThread();
    if (!thr) return;
    thr->m_call_queue.emplace(name, msg, func, conn_id, send_response);
    thr->m_cond.notify_one();
  } else {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_poll_queue.emplace(name, msg, func, conn_id, send_response);
    m_poll_cond.notify_one();
  }
}

bool RpcServer::PollRpc(bool blocking, RpcCallInfo* call_info) {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_poll_queue.empty()) {
    if (!blocking || m_terminating) return false;
    m_poll_cond.wait(lock);
  }

  auto& item = m_poll_queue.front();
  unsigned int call_uid = (item.conn_id << 16) | item.msg->seq_num_uid();
  call_info->rpc_id = item.msg->id();
  call_info->call_uid = call_uid;
  call_info->name = std::move(item.name);
  call_info->params = item.msg->str();
  m_response_map.insert(std::make_pair(std::make_pair(item.msg->id(), call_uid),
                                       item.send_response));
  m_poll_queue.pop();
  return true;
}

void RpcServer::PostRpcResponse(unsigned int rpc_id, unsigned int call_uid,
                                llvm::StringRef result) {
  auto i = m_response_map.find(std::make_pair(rpc_id, call_uid));
  if (i == m_response_map.end()) {
    WARNING("posting RPC response to nonexistent call (or duplicate response)");
    return;
  }
  (i->getSecond())(Message::RpcResponse(rpc_id, call_uid, result));
  m_response_map.erase(i);
}

void RpcServer::Thread::Main() {
  if (m_on_start) m_on_start();

  std::unique_lock<std::mutex> lock(m_mutex);
  std::string tmp;
  while (m_active) {
    while (m_call_queue.empty()) {
      m_cond.wait(lock);
      if (!m_active) goto done;
    }

    while (!m_call_queue.empty()) {
      if (!m_active) goto done;
      auto item = std::move(m_call_queue.front());
      m_call_queue.pop();

      DEBUG4("rpc calling " << item.name);

      if (item.name.empty() || !item.msg || !item.func || !item.send_response)
        continue;

      // Don't hold mutex during callback execution!
      lock.unlock();
      auto result = item.func(item.name, item.msg->str());
      item.send_response(Message::RpcResponse(item.msg->id(),
                                              item.msg->seq_num_uid(), result));
      lock.lock();
    }
  }

done:
  if (m_on_exit) m_on_exit();
}
