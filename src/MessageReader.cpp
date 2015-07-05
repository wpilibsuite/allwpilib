/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MessageReader.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "internal.h"
#include "leb128.h"

using namespace ntimpl;

void MessageHandler::anchor() {}

MessageReader::MessageReader(MessageHandler& handler, raw_istream& is,
                             unsigned int proto_rev)
    : WireDecoder(is, proto_rev), m_handler(handler) {}

MessageReader::~MessageReader() {}

bool MessageReader::Run() {
  unsigned int msg_type;
  if (!Read8(&msg_type)) return false;
  switch (msg_type) {
    case NT_MSG_KEEP_ALIVE:
      m_handler.GotKeepAlive();
      break;
    case NT_MSG_CLIENT_HELLO: {
      unsigned int proto_rev;
      if (!Read16(&proto_rev)) return false;
      StringValue self_id;
      // This intentionally uses the provided proto_rev instead of
      // m_proto_rev.
      if (proto_rev >= 0x0300u) {
        if (!ReadString(&self_id)) return false;
      }
      m_handler.GotClientHello(proto_rev, std::move(self_id));
      break;
    }
    case NT_MSG_PROTO_UNSUP: {
      unsigned int proto_rev;
      if (!Read16(&proto_rev)) return false;
      m_handler.GotProtoUnsup(proto_rev);
      break;
    }
    case NT_MSG_SERVER_HELLO_DONE:
      if (m_proto_rev < 0x0300u) {
        m_error = "received SERVER_HELLO_DONE in protocol < 3.0";
        return false;
      }
      m_handler.GotServerHelloDone();
      break;
    case NT_MSG_CLIENT_HELLO_DONE:
      if (m_proto_rev < 0x0300u) {
        m_error = "received CLIENT_HELLO_DONE in protocol < 3.0";
        return false;
      }
      m_handler.GotClientHelloDone();
      break;
    case NT_MSG_ENTRY_ASSIGN: {
      StringValue name;
      if (!ReadString(&name)) return false;
      NT_Type type;
      if (!ReadType(&type)) return false;
      unsigned int id, seq_num;
      if (!Read16(&id)) return false;
      if (!Read16(&seq_num)) return false;
      unsigned int flags = 0;
      if (m_proto_rev >= 0x0300u) {
        if (!Read8(&flags)) return false;
      }
      Value value;
      if (!ReadValue(type, &value)) return false;
      m_handler.GotEntryAssign(std::move(name), id, seq_num, std::move(value),
                               flags);
      break;
    }
    case NT_MSG_ENTRY_UPDATE: {
      unsigned int id, seq_num;
      Value value;
      if (!Read16(&id)) return false;
      if (!Read16(&seq_num)) return false;
      NT_Type type;
      if (m_proto_rev >= 0x0300u) {
        unsigned int itype;
        if (!Read8(&itype)) return false;
        type = static_cast<NT_Type>(itype);
      } else
        type = m_handler.GetEntryType(id);
      if (!ReadValue(type, &value)) return false;
      m_handler.GotEntryUpdate(id, seq_num, std::move(value));
      break;
    }
    case NT_MSG_FLAGS_UPDATE: {
      if (m_proto_rev < 0x0300u) {
        m_error = "received FLAGS_UPDATE in protocol < 3.0";
        return false;
      }
      unsigned int id, flags;
      if (!Read16(&id)) return false;
      if (!Read8(&flags)) return false;
      m_handler.GotFlagsUpdate(id, flags);
      break;
    }
    case NT_MSG_ENTRY_DELETE: {
      if (m_proto_rev < 0x0300u) {
        m_error = "received ENTRY_DELETE in protocol < 3.0";
        return false;
      }
      unsigned int id;
      if (!Read16(&id)) return false;
      m_handler.GotEntryDelete(id);
      break;
    }
    case NT_MSG_CLEAR_ENTRIES: {
      if (m_proto_rev < 0x0300u) {
        m_error = "received CLEAR_ENTRIES in protocol < 3.0";
        return false;
      }
      unsigned long magic;
      if (!Read32(&magic)) return false;
      if (magic != NT_CLEAR_ALL_MAGIC) {
        m_error = "received incorrect CLEAR_ENTRIES magic value, ignoring";
        return true;
      }
      m_handler.GotClearEntries();
      break;
    }
    case NT_MSG_EXECUTE_RPC: {
      if (m_proto_rev < 0x0300u) {
        m_error = "received EXECUTE_RPC in protocol < 3.0";
        return false;
      }
      unsigned int id, uid;
      if (!Read16(&id)) return false;
      if (!Read16(&uid)) return false;
      unsigned long size;
      if (!ReadUleb128(&size)) return false;
      const char* params;
      if (!Read(&params, size)) return false;
      m_handler.GotExecuteRpc(id, uid, llvm::StringRef(params, size));
      break;
    }
    case NT_MSG_RPC_RESPONSE: {
      if (m_proto_rev < 0x0300u) {
        m_error = "received RPC_RESPONSE in protocol < 3.0";
        return false;
      }
      unsigned int id, uid;
      if (!Read16(&id)) return false;
      if (!Read16(&uid)) return false;
      unsigned long size;
      if (!ReadUleb128(&size)) return false;
      const char* results;
      if (!Read(&results, size)) return false;
      m_handler.GotRpcResponse(id, uid, llvm::StringRef(results, size));
      break;
    }
    default:
      m_error = "unrecognized message type";
      return false;
  }
  return true;
}
