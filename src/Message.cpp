/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Message.h"

#include "Log.h"
#include "WireDecoder.h"
#include "WireEncoder.h"

#define kClearAllMagic 0xD06CB27Aul

using namespace nt;

std::shared_ptr<Message> Message::Read(WireDecoder& decoder,
                                       GetEntryTypeFunc get_entry_type) {
  unsigned int msg_type;
  if (!decoder.Read8(&msg_type)) return nullptr;
  auto msg =
      std::make_shared<Message>(static_cast<MsgType>(msg_type), private_init());
  switch (msg_type) {
    case kKeepAlive:
      break;
    case kClientHello: {
      unsigned int proto_rev;
      if (!decoder.Read16(&proto_rev)) return nullptr;
      msg->m_id = proto_rev;
      // This intentionally uses the provided proto_rev instead of
      // decoder.proto_rev().
      if (proto_rev >= 0x0300u) {
        if (!decoder.ReadString(&msg->m_str)) return nullptr;
      }
      break;
    }
    case kProtoUnsup: {
      if (!decoder.Read16(&msg->m_id)) return nullptr;  // proto rev
      break;
    }
    case kServerHelloDone:
      break;
    case kServerHello:
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received SERVER_HELLO in protocol < 3.0");
        return nullptr;
      }
      if (!decoder.Read8(&msg->m_flags)) return nullptr;
      if (!decoder.ReadString(&msg->m_str)) return nullptr;
      break;
    case kClientHelloDone:
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received CLIENT_HELLO_DONE in protocol < 3.0");
        return nullptr;
      }
      break;
    case kEntryAssign: {
      if (!decoder.ReadString(&msg->m_str)) return nullptr;      // name
      NT_Type type;
      if (!decoder.ReadType(&type)) return nullptr;              // entry type
      if (!decoder.Read16(&msg->m_id)) return nullptr;           // id
      if (!decoder.Read16(&msg->m_seq_num_uid)) return nullptr;  // seq num
      if (decoder.proto_rev() >= 0x0300u) {
        if (!decoder.Read8(&msg->m_flags)) return nullptr;  // flags
      }
      msg->m_value = decoder.ReadValue(type);
      if (!msg->m_value) return nullptr;
      break;
    }
    case kEntryUpdate: {
      if (!decoder.Read16(&msg->m_id)) return nullptr;           // id
      if (!decoder.Read16(&msg->m_seq_num_uid)) return nullptr;  // seq num
      NT_Type type;
      if (decoder.proto_rev() >= 0x0300u) {
        if (!decoder.ReadType(&type)) return nullptr;
      } else {
        type = get_entry_type(msg->m_id);
      }
      DEBUG4("update message data type: " << type);
      msg->m_value = decoder.ReadValue(type);
      if (!msg->m_value) return nullptr;
      break;
    }
    case kFlagsUpdate: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received FLAGS_UPDATE in protocol < 3.0");
        return nullptr;
      }
      if (!decoder.Read16(&msg->m_id)) return nullptr;
      if (!decoder.Read8(&msg->m_flags)) return nullptr;
      break;
    }
    case kEntryDelete: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received ENTRY_DELETE in protocol < 3.0");
        return nullptr;
      }
      if (!decoder.Read16(&msg->m_id)) return nullptr;
      break;
    }
    case kClearEntries: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received CLEAR_ENTRIES in protocol < 3.0");
        return nullptr;
      }
      unsigned long magic;
      if (!decoder.Read32(&magic)) return nullptr;
      if (magic != kClearAllMagic) {
        decoder.set_error(
            "received incorrect CLEAR_ENTRIES magic value, ignoring");
        return nullptr;
      }
      break;
    }
    case kExecuteRpc: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received EXECUTE_RPC in protocol < 3.0");
        return nullptr;
      }
      if (!decoder.Read16(&msg->m_id)) return nullptr;
      if (!decoder.Read16(&msg->m_seq_num_uid)) return nullptr;  // uid
      unsigned long size;
      if (!decoder.ReadUleb128(&size)) return nullptr;
      const char* params;
      if (!decoder.Read(&params, size)) return nullptr;
      msg->m_str = llvm::StringRef(params, size);
      break;
    }
    case kRpcResponse: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received RPC_RESPONSE in protocol < 3.0");
        return nullptr;
      }
      if (!decoder.Read16(&msg->m_id)) return nullptr;
      if (!decoder.Read16(&msg->m_seq_num_uid)) return nullptr;  // uid
      unsigned long size;
      if (!decoder.ReadUleb128(&size)) return nullptr;
      const char* results;
      if (!decoder.Read(&results, size)) return nullptr;
      msg->m_str = llvm::StringRef(results, size);
      break;
    }
    default:
      decoder.set_error("unrecognized message type");
      INFO("unrecognized message type: " << msg_type);
      return nullptr;
  }
  return msg;
}

std::shared_ptr<Message> Message::ClientHello(llvm::StringRef self_id) {
  auto msg = std::make_shared<Message>(kClientHello, private_init());
  msg->m_str = self_id;
  return msg;
}

std::shared_ptr<Message> Message::ServerHello(unsigned int flags,
                                              llvm::StringRef self_id) {
  auto msg = std::make_shared<Message>(kServerHello, private_init());
  msg->m_str = self_id;
  msg->m_flags = flags;
  return msg;
}

std::shared_ptr<Message> Message::EntryAssign(llvm::StringRef name,
                                              unsigned int id,
                                              unsigned int seq_num,
                                              std::shared_ptr<Value> value,
                                              unsigned int flags) {
  auto msg = std::make_shared<Message>(kEntryAssign, private_init());
  msg->m_str = name;
  msg->m_value = value;
  msg->m_id = id;
  msg->m_flags = flags;
  msg->m_seq_num_uid = seq_num;
  return msg;
}

std::shared_ptr<Message> Message::EntryUpdate(unsigned int id,
                                              unsigned int seq_num,
                                              std::shared_ptr<Value> value) {
  auto msg = std::make_shared<Message>(kEntryUpdate, private_init());
  msg->m_value = value;
  msg->m_id = id;
  msg->m_seq_num_uid = seq_num;
  return msg;
}

std::shared_ptr<Message> Message::FlagsUpdate(unsigned int id,
                                              unsigned int flags) {
  auto msg = std::make_shared<Message>(kFlagsUpdate, private_init());
  msg->m_id = id;
  msg->m_flags = flags;
  return msg;
}

std::shared_ptr<Message> Message::EntryDelete(unsigned int id) {
  auto msg = std::make_shared<Message>(kEntryDelete, private_init());
  msg->m_id = id;
  return msg;
}

std::shared_ptr<Message> Message::ExecuteRpc(unsigned int id, unsigned int uid,
                                             llvm::StringRef params) {
  auto msg = std::make_shared<Message>(kExecuteRpc, private_init());
  msg->m_str = params;
  msg->m_id = id;
  msg->m_seq_num_uid = uid;
  return msg;
}

std::shared_ptr<Message> Message::RpcResponse(unsigned int id, unsigned int uid,
                                              llvm::StringRef results) {
  auto msg = std::make_shared<Message>(kRpcResponse, private_init());
  msg->m_str = results;
  msg->m_id = id;
  msg->m_seq_num_uid = uid;
  return msg;
}

void Message::Write(WireEncoder& encoder) const {
  switch (m_type) {
    case kKeepAlive:
      encoder.Write8(kKeepAlive);
      break;
    case kClientHello:
      encoder.Write8(kClientHello);
      encoder.Write16(encoder.proto_rev());
      if (encoder.proto_rev() < 0x0300u) return;
      encoder.WriteString(m_str);
      break;
    case kProtoUnsup:
      encoder.Write8(kProtoUnsup);
      encoder.Write16(encoder.proto_rev());
      break;
    case kServerHelloDone:
      encoder.Write8(kServerHelloDone);
      break;
    case kServerHello:
      if (encoder.proto_rev() < 0x0300u) return;  // new message in version 3.0
      encoder.Write8(kServerHello);
      encoder.Write8(m_flags);
      encoder.WriteString(m_str);
      break;
    case kClientHelloDone:
      if (encoder.proto_rev() < 0x0300u) return;  // new message in version 3.0
      encoder.Write8(kClientHelloDone);
      break;
    case kEntryAssign:
      encoder.Write8(kEntryAssign);
      encoder.WriteString(m_str);
      encoder.WriteType(m_value->type());
      encoder.Write16(m_id);
      encoder.Write16(m_seq_num_uid);
      if (encoder.proto_rev() >= 0x0300u) encoder.Write8(m_flags);
      encoder.WriteValue(*m_value);
      break;
    case kEntryUpdate:
      encoder.Write8(kEntryUpdate);
      encoder.Write16(m_id);
      encoder.Write16(m_seq_num_uid);
      if (encoder.proto_rev() >= 0x0300u) encoder.WriteType(m_value->type());
      encoder.WriteValue(*m_value);
      break;
    case kFlagsUpdate:
      if (encoder.proto_rev() < 0x0300u) return;  // new message in version 3.0
      encoder.Write8(kFlagsUpdate);
      encoder.Write16(m_id);
      encoder.Write8(m_flags);
      break;
    case kEntryDelete:
      if (encoder.proto_rev() < 0x0300u) return;  // new message in version 3.0
      encoder.Write8(kEntryDelete);
      encoder.Write16(m_id);
      break;
    case kClearEntries:
      if (encoder.proto_rev() < 0x0300u) return;  // new message in version 3.0
      encoder.Write8(kClearEntries);
      encoder.Write32(kClearAllMagic);
      break;
    case kExecuteRpc:
      if (encoder.proto_rev() < 0x0300u) return;  // new message in version 3.0
      encoder.Write8(kExecuteRpc);
      encoder.Write16(m_id);
      encoder.Write16(m_seq_num_uid);
      encoder.WriteString(m_str);
      break;
    case kRpcResponse:
      if (encoder.proto_rev() < 0x0300u) return;  // new message in version 3.0
      encoder.Write8(kRpcResponse);
      encoder.Write16(m_id);
      encoder.Write16(m_seq_num_uid);
      encoder.WriteString(m_str);
      break;
    default:
      break;
  }
}
