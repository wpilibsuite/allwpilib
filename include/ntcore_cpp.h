/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_CPP_H_
#define NTCORE_CPP_H_

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "llvm/ArrayRef.h"
#include "llvm/StringRef.h"

#include "nt_Value.h"

namespace nt {

using llvm::ArrayRef;
using llvm::StringRef;

/** NetworkTables Entry Information */
struct EntryInfo {
  /** Entry name */
  std::string name;

  /** Entry type */
  NT_Type type;

  /** Entry flags */
  unsigned int flags;

  /** Timestamp of last change to entry (type or value). */
  unsigned long long last_change;
};

/** NetworkTables Connection Information */
struct ConnectionInfo {
  std::string remote_id;
  std::string remote_ip;
  unsigned int remote_port;
  unsigned long long last_update;
  unsigned int protocol_version;
};

/** NetworkTables RPC Parameter Definition */
struct RpcParamDef {
  RpcParamDef() = default;
  RpcParamDef(StringRef name_, std::shared_ptr<Value> def_value_)
      : name(name_), def_value(def_value_) {}

  std::string name;
  std::shared_ptr<Value> def_value;
};

/** NetworkTables RPC Result Definition */
struct RpcResultDef {
  RpcResultDef() = default;
  RpcResultDef(StringRef name_, NT_Type type_) : name(name_), type(type_) {}

  std::string name;
  NT_Type type;
};

/** NetworkTables RPC Definition */
struct RpcDefinition {
  unsigned int version;
  std::string name;
  std::vector<RpcParamDef> params;
  std::vector<RpcResultDef> results;
};

/** NetworkTables RPC Call Data */
struct RpcCallInfo {
  unsigned int rpc_id;
  unsigned int call_uid;
  std::string name;
  std::string params;
};

/*
 * Table Functions
 */

/** Get Entry Value.
 * Returns copy of current entry value.
 * Note that one of the type options is "unassigned".
 *
 * @param name      entry name (UTF-8 string)
 * @return entry value
 */
std::shared_ptr<Value> GetEntryValue(StringRef name);

/** Set Default Entry Value
 * Returns copy of current entry value if it exists.
 * Otherwise, sets passed in value, and returns set value.
 * Note that one of the type options is "unassigned".
 *
 * @param name      entry name (UTF-8 string)
 * @param value     value to be set if name does not exist
 * @return False on error (value not set), True on success
 */
bool SetDefaultEntryValue(StringRef name, std::shared_ptr<Value> value);

/** Set Entry Value.
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, returns error and does not update value.
 *
 * @param name      entry name (UTF-8 string)
 * @param value     new entry value
 * @return False on error (type mismatch), True on success
 */
bool SetEntryValue(StringRef name, std::shared_ptr<Value> value);

/** Set Entry Type and Value.
 * Sets new entry value.  If type of new value differs from the type of the
 * currently stored entry, the currently stored entry type is overridden
 * (generally this will generate an Entry Assignment message).
 *
 * This is NOT the preferred method to update a value; generally
 * SetEntryValue() should be used instead, with appropriate error handling.
 *
 * @param name      entry name (UTF-8 string)
 * @param value     new entry value
 */
void SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value);

/** Set Entry Flags.
 */
void SetEntryFlags(StringRef name, unsigned int flags);

/** Get Entry Flags.
 */
unsigned int GetEntryFlags(StringRef name);

/** Delete Entry.
 * Deletes an entry.  This is a new feature in version 3.0 of the protocol,
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 *
 * @param name      entry name (UTF-8 string)
 */
void DeleteEntry(StringRef name);

/** Delete All Entries.
 * Deletes ALL table entries.  This is a new feature in version 3.0 of the
 * so this may not have an effect if any other node in the network is not
 * version 3.0 or newer.
 *
 * Note: GetConnections() can be used to determine the protocol version
 * of direct remote connection(s), but this is not sufficient to determine
 * if all nodes in the network are version 3.0 or newer.
 */
void DeleteAllEntries();

/** Get Entry Information.
 * Returns an array of entry information (name, entry type,
 * and timestamp of last change to type/value).  The results are optionally
 * filtered by string prefix and entry type to only return a subset of all
 * entries.
 *
 * @param prefix        entry name required prefix; only entries whose name
 *                      starts with this string are returned
 * @param types         bitmask of NT_Type values; 0 is treated specially
 *                      as a "don't care"
 * @return Array of entry information.
 */
std::vector<EntryInfo> GetEntryInfo(StringRef prefix, unsigned int types);

/** Flush Entries.
 * Forces an immediate flush of all local entry changes to network.
 * Normally this is done on a regularly scheduled interval (see
 * NT_SetUpdateRate()).
 *
 * Note: flushes are rate limited to avoid excessive network traffic.  If
 * the time between calls is too short, the flush will occur after the minimum
 * time elapses (rather than immediately).
 */
void Flush();

/*
 * Callback Creation Functions
 */

void SetListenerOnStart(std::function<void()> on_start);
void SetListenerOnExit(std::function<void()> on_exit);

typedef std::function<void(unsigned int uid, StringRef name,
                           std::shared_ptr<Value> value, unsigned int flags)>
    EntryListenerCallback;

typedef std::function<void(unsigned int uid, bool connected,
                           const ConnectionInfo& conn)>
    ConnectionListenerCallback;

unsigned int AddEntryListener(StringRef prefix, EntryListenerCallback callback,
                              unsigned int flags);
void RemoveEntryListener(unsigned int entry_listener_uid);
unsigned int AddConnectionListener(ConnectionListenerCallback callback,
                                   bool immediate_notify);
void RemoveConnectionListener(unsigned int conn_listener_uid);

bool NotifierDestroyed();

/*
 * Remote Procedure Call Functions
 */

#if defined(_MSC_VER) && _MSC_VER < 1900
const double kTimeout_Indefinite = -1;
#else
constexpr double kTimeout_Indefinite = -1;
#endif

void SetRpcServerOnStart(std::function<void()> on_start);
void SetRpcServerOnExit(std::function<void()> on_exit);

typedef std::function<std::string(StringRef name, StringRef params,
                                  const ConnectionInfo& conn_info)>
    RpcCallback;

void CreateRpc(StringRef name, StringRef def, RpcCallback callback);
void CreatePolledRpc(StringRef name, StringRef def);

bool PollRpc(bool blocking, RpcCallInfo* call_info);
bool PollRpc(bool blocking, double time_out, RpcCallInfo* call_info);
void PostRpcResponse(unsigned int rpc_id, unsigned int call_uid,
                     StringRef result);

unsigned int CallRpc(StringRef name, StringRef params);
bool GetRpcResult(bool blocking, unsigned int call_uid, std::string* result);
bool GetRpcResult(bool blocking, unsigned int call_uid, double time_out,
                  std::string* result);
void CancelBlockingRpcResult(unsigned int call_uid);

std::string PackRpcDefinition(const RpcDefinition& def);
bool UnpackRpcDefinition(StringRef packed, RpcDefinition* def);
std::string PackRpcValues(ArrayRef<std::shared_ptr<Value>> values);
std::vector<std::shared_ptr<Value>> UnpackRpcValues(StringRef packed,
                                                    ArrayRef<NT_Type> types);

/*
 * Client/Server Functions
 */
void SetNetworkIdentity(StringRef name);
void StartServer(StringRef persist_filename, const char* listen_address,
                 unsigned int port);
void StopServer();
void StartClient();
void StartClient(const char* server_name, unsigned int port);
void StartClient(ArrayRef<std::pair<StringRef, unsigned int>> servers);
void StopClient();
void SetServer(const char* server_name, unsigned int port);
void SetServer(ArrayRef<std::pair<StringRef, unsigned int>> servers);
void StartDSClient(unsigned int port);
void StopDSClient();
void StopRpcServer();
void StopNotifier();
void SetUpdateRate(double interval);
std::vector<ConnectionInfo> GetConnections();

/*
 * Persistent Functions
 */
/* return error string, or nullptr if successful */
const char* SavePersistent(StringRef filename);
const char* LoadPersistent(
    StringRef filename, std::function<void(size_t line, const char* msg)> warn);

/*
 * Utility Functions
 */

/* timestamp */
unsigned long long Now();

/* logging */
typedef std::function<void(unsigned int level, const char* file,
                           unsigned int line, const char* msg)>
    LogFunc;
void SetLogger(LogFunc func, unsigned int min_level);

}  // namespace nt

#endif /* NTCORE_CPP_H_ */
