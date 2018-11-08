/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_RPCSERVER_H_
#define NTCORE_RPCSERVER_H_

#include <utility>

#include <wpi/DenseMap.h>
#include <wpi/mutex.h>

#include "CallbackManager.h"
#include "Handle.h"
#include "IRpcServer.h"
#include "Log.h"

namespace nt {

namespace impl {

typedef std::pair<unsigned int, unsigned int> RpcIdPair;

struct RpcNotifierData : public RpcAnswer {
  RpcNotifierData(NT_Entry entry_, NT_RpcCall call_, StringRef name_,
                  StringRef params_, const ConnectionInfo& conn_,
                  IRpcServer::SendResponseFunc send_response_)
      : RpcAnswer{entry_, call_, name_, params_, conn_},
        send_response{send_response_} {}

  IRpcServer::SendResponseFunc send_response;
};

using RpcListenerData =
    ListenerData<std::function<void(const RpcAnswer& answer)>>;

class RpcServerThread
    : public CallbackThread<RpcServerThread, RpcAnswer, RpcListenerData,
                            RpcNotifierData> {
 public:
  RpcServerThread(int inst, wpi::Logger& logger)
      : m_inst(inst), m_logger(logger) {}

  bool Matches(const RpcListenerData& /*listener*/,
               const RpcNotifierData& data) {
    return !data.name.empty() && data.send_response;
  }

  void SetListener(RpcNotifierData* data, unsigned int /*listener_uid*/) {
    unsigned int local_id = Handle{data->entry}.GetIndex();
    unsigned int call_uid = Handle{data->call}.GetIndex();
    RpcIdPair lookup_uid{local_id, call_uid};
    m_response_map.insert(std::make_pair(lookup_uid, data->send_response));
  }

  void DoCallback(std::function<void(const RpcAnswer& call)> callback,
                  const RpcNotifierData& data) {
    DEBUG4("rpc calling " << data.name);
    unsigned int local_id = Handle{data.entry}.GetIndex();
    unsigned int call_uid = Handle{data.call}.GetIndex();
    RpcIdPair lookup_uid{local_id, call_uid};
    callback(data);
    {
      std::lock_guard<wpi::mutex> lock(m_mutex);
      auto i = m_response_map.find(lookup_uid);
      if (i != m_response_map.end()) {
        // post an empty response and erase it
        (i->getSecond())("");
        m_response_map.erase(i);
      }
    }
  }

  int m_inst;
  wpi::Logger& m_logger;
  wpi::DenseMap<RpcIdPair, IRpcServer::SendResponseFunc> m_response_map;
};

}  // namespace impl

class RpcServer : public IRpcServer,
                  public CallbackManager<RpcServer, impl::RpcServerThread> {
  friend class RpcServerTest;
  friend class CallbackManager<RpcServer, impl::RpcServerThread>;

 public:
  RpcServer(int inst, wpi::Logger& logger);

  void Start();

  unsigned int Add(std::function<void(const RpcAnswer& answer)> callback);
  unsigned int AddPolled(unsigned int poller_uid);
  void RemoveRpc(unsigned int rpc_uid) override;

  void ProcessRpc(unsigned int local_id, unsigned int call_uid, StringRef name,
                  StringRef params, const ConnectionInfo& conn,
                  SendResponseFunc send_response,
                  unsigned int rpc_uid) override;

  bool PostRpcResponse(unsigned int local_id, unsigned int call_uid,
                       wpi::StringRef result);

 private:
  int m_inst;
  wpi::Logger& m_logger;
};

}  // namespace nt

#endif  // NTCORE_RPCSERVER_H_
