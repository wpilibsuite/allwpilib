/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Message.h"

#include "WireDecoder.h"
#include "WireEncoder.h"

static constexpr unsigned long kClearAllMagic = 0xD06CB27Aul;

using namespace ntimpl;

bool Message::Read(WireDecoder& decoder,
                   NT_Type (*get_entry_type)(unsigned int id), Message* msg) {
  unsigned int msg_type;
  if (!decoder.Read8(&msg_type)) return false;
  switch (msg_type) {
    case kKeepAlive:
      *msg = KeepAlive();
      break;
    case kClientHello: {
      unsigned int proto_rev;
      if (!decoder.Read16(&proto_rev)) return false;
      StringValue self_id;
      // This intentionally uses the provided proto_rev instead of
      // decoder.proto_rev().
      if (proto_rev >= 0x0300u) {
        if (!decoder.ReadString(&self_id)) return false;
      }
      *msg = Message(kClientHello);
      msg->m_str = std::move(self_id);
      msg->m_id = proto_rev;
      break;
    }
    case kProtoUnsup: {
      unsigned int proto_rev;
      if (!decoder.Read16(&proto_rev)) return false;
      *msg = ProtoUnsup();
      msg->m_id = proto_rev;
      break;
    }
    case kServerHelloDone:
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received SERVER_HELLO_DONE in protocol < 3.0");
        return false;
      }
      *msg = ServerHelloDone();
      break;
    case kClientHelloDone:
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received CLIENT_HELLO_DONE in protocol < 3.0");
        return false;
      }
      *msg = ClientHelloDone();
      break;
    case kEntryAssign: {
      StringValue name;
      if (!decoder.ReadString(&name)) return false;
      NT_Type type;
      if (!decoder.ReadType(&type)) return false;
      unsigned int id, seq_num;
      if (!decoder.Read16(&id)) return false;
      if (!decoder.Read16(&seq_num)) return false;
      unsigned int flags = 0;
      if (decoder.proto_rev() >= 0x0300u) {
        if (!decoder.Read8(&flags)) return false;
      }
      Value value;
      if (!decoder.ReadValue(type, &value)) return false;
      *msg = Message(kEntryAssign);
      msg->m_str = std::move(name);
      msg->m_value = std::make_shared<Value>(std::move(value));
      msg->m_id = id;
      msg->m_flags = flags;
      msg->m_seq_num_uid = seq_num;
      break;
    }
    case kEntryUpdate: {
      unsigned int id, seq_num;
      if (!decoder.Read16(&id)) return false;
      if (!decoder.Read16(&seq_num)) return false;
      NT_Type type;
      if (decoder.proto_rev() >= 0x0300u) {
        unsigned int itype;
        if (!decoder.Read8(&itype)) return false;
        type = static_cast<NT_Type>(itype);
      } else
        type = get_entry_type(id);
      Value value;
      if (!decoder.ReadValue(type, &value)) return false;
      *msg = Message(kEntryUpdate);
      msg->m_value = std::make_shared<Value>(std::move(value));
      msg->m_id = id;
      msg->m_seq_num_uid = seq_num;
      break;
    }
    case kFlagsUpdate: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received FLAGS_UPDATE in protocol < 3.0");
        return false;
      }
      unsigned int id, flags;
      if (!decoder.Read16(&id)) return false;
      if (!decoder.Read8(&flags)) return false;
      *msg = FlagsUpdate(id, flags);
      break;
    }
    case kEntryDelete: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received ENTRY_DELETE in protocol < 3.0");
        return false;
      }
      unsigned int id;
      if (!decoder.Read16(&id)) return false;
      *msg = EntryDelete(id);
      break;
    }
    case kClearEntries: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received CLEAR_ENTRIES in protocol < 3.0");
        return false;
      }
      unsigned long magic;
      if (!decoder.Read32(&magic)) return false;
      if (magic != kClearAllMagic) {
        decoder.set_error(
            "received incorrect CLEAR_ENTRIES magic value, ignoring");
        return true;
      }
      *msg = ClearEntries();
      break;
    }
    case kExecuteRpc: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received EXECUTE_RPC in protocol < 3.0");
        return false;
      }
      unsigned int id, uid;
      if (!decoder.Read16(&id)) return false;
      if (!decoder.Read16(&uid)) return false;
      unsigned long size;
      if (!decoder.ReadUleb128(&size)) return false;
      const char* params;
      if (!decoder.Read(&params, size)) return false;
      *msg = ExecuteRpc(id, uid, llvm::StringRef(params, size));
      break;
    }
    case kRpcResponse: {
      if (decoder.proto_rev() < 0x0300u) {
        decoder.set_error("received RPC_RESPONSE in protocol < 3.0");
        return false;
      }
      unsigned int id, uid;
      if (!decoder.Read16(&id)) return false;
      if (!decoder.Read16(&uid)) return false;
      unsigned long size;
      if (!decoder.ReadUleb128(&size)) return false;
      const char* results;
      if (!decoder.Read(&results, size)) return false;
      *msg = RpcResponse(id, uid, llvm::StringRef(results, size));
      break;
    }
    default:
      decoder.set_error("unrecognized message type");
      return false;
  }
  return true;
}

Message Message::ClientHello(llvm::StringRef self_id) {
  Message msg(kClientHello);
  msg.m_str = self_id;
  return msg;
}

Message Message::ServerHello(unsigned int flags, llvm::StringRef self_id) {
  Message msg(kServerHello);
  msg.m_str = self_id;
  msg.m_flags = flags;
  return msg;
}

Message Message::EntryAssign(llvm::StringRef name, unsigned int id,
                             unsigned int seq_num, std::shared_ptr<Value> value,
                             unsigned int flags) {
  Message msg(kEntryAssign);
  msg.m_str = name;
  msg.m_value = value;
  msg.m_id = id;
  msg.m_flags = flags;
  msg.m_seq_num_uid = seq_num;
  return msg;
}

Message Message::EntryUpdate(unsigned int id, unsigned int seq_num,
                             std::shared_ptr<Value> value) {
  Message msg(kEntryUpdate);
  msg.m_value = value;
  msg.m_id = id;
  msg.m_seq_num_uid = seq_num;
  return msg;
}

Message Message::FlagsUpdate(unsigned int id, unsigned int flags) {
  Message msg(kFlagsUpdate);
  msg.m_id = id;
  msg.m_flags = flags;
  return msg;
}

Message Message::EntryDelete(unsigned int id) {
  Message msg(kEntryDelete);
  msg.m_id = id;
  return msg;
}

Message Message::ExecuteRpc(unsigned int id, unsigned int uid,
                            llvm::ArrayRef<NT_Value> params) {
  WireEncoder enc(0x0300);
  for (auto& param : params) enc.WriteValue(param);
  return ExecuteRpc(id, uid, enc.ToStringRef());
}

Message Message::ExecuteRpc(unsigned int id, unsigned int uid,
                            llvm::StringRef params) {
  Message msg(kExecuteRpc);
  msg.m_str = params;
  msg.m_id = id;
  msg.m_seq_num_uid = uid;
  return msg;
}

Message Message::RpcResponse(unsigned int id, unsigned int uid,
                             llvm::ArrayRef<NT_Value> results) {
  WireEncoder enc(0x0300);
  for (auto& result : results) enc.WriteValue(result);
  return RpcResponse(id, uid, enc.ToStringRef());
}

Message Message::RpcResponse(unsigned int id, unsigned int uid,
                             llvm::StringRef results) {
  Message msg(kRpcResponse);
  msg.m_str = results;
  msg.m_id = id;
  msg.m_seq_num_uid = uid;
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
      if (encoder.proto_rev() >= 0x0300u) encoder.Write8(m_value->type());
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
