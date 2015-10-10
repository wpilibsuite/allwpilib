/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RpcServer.h"

#include "Log.h"

using namespace nt;

ATOMIC_STATIC_INIT(RpcServer)

RpcServer::RpcServer() {
  m_active = false;
  m_terminating = false;
}

RpcServer::~RpcServer() {
  Logger::GetInstance().SetLogger(nullptr);
  Stop();
  m_terminating = true;
  m_poll_cond.notify_all();
}

void RpcServer::Start() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_active) return;
    m_active = true;
  }
  {
    std::lock_guard<std::mutex> lock(m_shutdown_mutex);
    m_shutdown = false;
  }
  m_thread = std::thread(&RpcServer::ThreadMain, this);
}

void RpcServer::Stop() {
  m_active = false;
  if (m_thread.joinable()) {
    // send notification so the thread terminates
    m_call_cond.notify_one();
    // join with timeout
    std::unique_lock<std::mutex> lock(m_shutdown_mutex);
    auto timeout_time =
        std::chrono::steady_clock::now() + std::chrono::seconds(1);
    if (m_shutdown_cv.wait_until(lock, timeout_time,
                                 [&] { return m_shutdown; }))
      m_thread.join();
    else
      m_thread.detach();  // timed out, detach it
  }
}

void RpcServer::ProcessRpc(StringRef name, std::shared_ptr<Message> msg,
                           RpcCallback func, unsigned int conn_id,
                           SendMsgFunc send_response) {
  std::unique_lock<std::mutex> lock(m_mutex);

  if (func)
    m_call_queue.emplace(name, msg, func, conn_id, send_response);
  else
    m_poll_queue.emplace(name, msg, func, conn_id, send_response);

  lock.unlock();

  if (func)
    m_call_cond.notify_one();
  else
    m_poll_cond.notify_one();
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

void RpcServer::ThreadMain() {
  std::unique_lock<std::mutex> lock(m_mutex);
  std::string tmp;
  while (m_active) {
    while (m_call_queue.empty()) {
      m_call_cond.wait(lock);
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
  // use condition variable to signal thread shutdown
  {
    std::lock_guard<std::mutex> lock(m_shutdown_mutex);
    m_shutdown = true;
    m_shutdown_cv.notify_one();
  }
}
