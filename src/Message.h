/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_MESSAGE_H_
#define NT_MESSAGE_H_

#include <memory>

#include "Value.h"

namespace ntimpl {

class WireDecoder;
class WireEncoder;

class Message {
 public:
  enum MsgType {
    kUnknown = -1,
    kKeepAlive = 0x00,
    kClientHello = 0x01,
    kProtoUnsup = 0x02,
    kServerHelloDone = 0x03,
    kServerHello = 0x04,
    kClientHelloDone = 0x05,
    kEntryAssign = 0x10,
    kEntryUpdate = 0x11,
    kFlagsUpdate = 0x12,
    kEntryDelete = 0x13,
    kClearEntries = 0x14,
    kExecuteRpc = 0x20,
    kRpcResponse = 0x21
  };

  Message() : m_type(kUnknown), m_id(0), m_flags(0), m_seq_num_uid(0) {}

  MsgType type() const { return m_type; }
  bool Is(MsgType type) const { return type == m_type; }

  // Read and write from wire representation
  void Write(WireEncoder& encoder) const;
  static bool Read(WireDecoder& decoder,
                   NT_Type (*get_entry_type)(unsigned int id), Message* msg);

  // Create messages without data
  static Message KeepAlive() { return Message(kKeepAlive); }
  static Message ProtoUnsup() { return Message(kProtoUnsup); }
  static Message ServerHelloDone() { return Message(kServerHelloDone); }
  static Message ClientHelloDone() { return Message(kClientHelloDone); }
  static Message ClearEntries() { return Message(kClearEntries); }

  // Create messages with data
  static Message ClientHello(llvm::StringRef self_id);
  static Message ServerHello(unsigned int flags, llvm::StringRef self_id);
  static Message EntryAssign(llvm::StringRef name, unsigned int id,
                             unsigned int seq_num, std::shared_ptr<Value> value,
                             unsigned int flags);
  static Message EntryUpdate(unsigned int id, unsigned int seq_num,
                             std::shared_ptr<Value> value);
  static Message FlagsUpdate(unsigned int id, unsigned int flags);
  static Message EntryDelete(unsigned int id);
  static Message ExecuteRpc(unsigned int id, unsigned int uid,
                            llvm::ArrayRef<NT_Value> params);
  static Message ExecuteRpc(unsigned int id, unsigned int uid,
                            llvm::StringRef params);
  static Message RpcResponse(unsigned int id, unsigned int uid,
                             llvm::ArrayRef<NT_Value> results);
  static Message RpcResponse(unsigned int id, unsigned int uid,
                             llvm::StringRef results);

  Message(const Message&) = delete;
  Message& operator=(const Message&) = delete;
  Message(Message&&) = default;
  Message& operator=(Message&&) = default;

 private:
  Message(MsgType type);

  MsgType m_type;

  // Message data.  Use varies by message type.
  StringValue m_str;
  std::shared_ptr<Value> m_value;
  unsigned int m_id;  // also used for proto_rev
  unsigned int m_flags;
  unsigned int m_seq_num_uid;
};

}  // namespace ntimpl

#endif  // NT_MESSAGE_H_
