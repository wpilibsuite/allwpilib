// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RpcServer.h"

using namespace nt;

RpcServer::RpcServer(int inst, wpi::Logger& logger)
    : m_inst(inst), m_logger(logger) {}

void RpcServer::Start() {
  DoStart(m_inst, m_logger);
}

unsigned int RpcServer::Add(
    std::function<void(const RpcAnswer& answer)> callback) {
  return DoAdd(callback);
}

unsigned int RpcServer::AddPolled(unsigned int poller_uid) {
  return DoAdd(poller_uid);
}

void RpcServer::RemoveRpc(unsigned int rpc_uid) {
  Remove(rpc_uid);
}

void RpcServer::ProcessRpc(unsigned int local_id, unsigned int call_uid,
                           std::string_view name, std::string_view params,
                           const ConnectionInfo& conn,
                           SendResponseFunc send_response,
                           unsigned int rpc_uid) {
  Send(rpc_uid, Handle(m_inst, local_id, Handle::kEntry).handle(),
       Handle(m_inst, call_uid, Handle::kRpcCall).handle(), name, params, conn,
       send_response);
}

bool RpcServer::PostRpcResponse(unsigned int local_id, unsigned int call_uid,
                                std::string_view result) {
  auto thr = GetThread();
  auto i = thr->m_response_map.find(impl::RpcIdPair{local_id, call_uid});
  if (i == thr->m_response_map.end()) {
    WARNING("posting RPC response to nonexistent call (or duplicate response)");
    return false;
  }
  (i->getSecond())(result);
  thr->m_response_map.erase(i);
  return true;
}
