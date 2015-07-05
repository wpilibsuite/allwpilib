/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MessageWriter.h"

#include "internal.h"

using namespace ntimpl;

MessageWriter::~MessageWriter() {}

void MessageWriter::WriteKeepAlive() { Write8(NT_MSG_KEEP_ALIVE); }

void MessageWriter::WriteClientHello(llvm::StringRef self_id) {
  Write8(NT_MSG_CLIENT_HELLO);
  Write16(m_proto_rev);
  if (m_proto_rev < 0x0300u) return;
  WriteString(self_id);
}

void MessageWriter::WriteProtoUnsup() {
  Write8(NT_MSG_PROTO_UNSUP);
  Write16(m_proto_rev);
}

void MessageWriter::WriteServerHelloDone() { Write8(NT_MSG_SERVER_HELLO_DONE); }

void MessageWriter::WriteServerHello(unsigned int flags,
                                     llvm::StringRef self_id) {
  if (m_proto_rev < 0x0300u) return;  // new message in version 3.0
  Write8(NT_MSG_SERVER_HELLO);
  Write8(flags);
  WriteString(self_id);
}

void MessageWriter::WriteClientHelloDone() {
  if (m_proto_rev < 0x0300u) return;  // new message in version 3.0
  Write8(NT_MSG_CLIENT_HELLO_DONE);
}

void MessageWriter::WriteEntryAssign(llvm::StringRef name, unsigned int id,
                                     unsigned int seq_num,
                                     const NT_Value& value,
                                     unsigned int flags) {
  Write8(NT_MSG_ENTRY_ASSIGN);
  WriteString(name);
  WriteType(value.type);
  Write16(id);
  Write16(seq_num);
  if (m_proto_rev >= 0x0300u) Write8(flags);
  WriteValue(value);
}

void MessageWriter::WriteEntryUpdate(unsigned int id, unsigned int seq_num,
                                     const NT_Value& value) {
  Write8(NT_MSG_ENTRY_UPDATE);
  Write16(id);
  Write16(seq_num);
  if (m_proto_rev >= 0x0300u) Write8(value.type);
  WriteValue(value);
}

void MessageWriter::WriteFlagsUpdate(unsigned int id, unsigned int flags) {
  if (m_proto_rev < 0x0300u) return;  // new message in version 3.0
  Write8(NT_MSG_FLAGS_UPDATE);
  Write16(id);
  Write8(flags);
}

void MessageWriter::WriteEntryDelete(unsigned int id) {
  if (m_proto_rev < 0x0300u) return;  // new message in version 3.0
  Write8(NT_MSG_ENTRY_DELETE);
  Write16(id);
}

void MessageWriter::WriteClearEntries() {
  if (m_proto_rev < 0x0300u) return;  // new message in version 3.0
  Write8(NT_MSG_CLEAR_ENTRIES);
  Write32(NT_CLEAR_ALL_MAGIC);
}

void MessageWriter::WriteExecuteRpc(unsigned int id, unsigned int uid,
                                    llvm::ArrayRef<NT_Value> params) {
  WriteRpc(NT_MSG_EXECUTE_RPC, id, uid, params);
}

void MessageWriter::WriteRpcResponse(unsigned int id, unsigned int uid,
                                     llvm::ArrayRef<NT_Value> results) {
  WriteRpc(NT_MSG_RPC_RESPONSE, id, uid, results);
}

void MessageWriter::WriteRpc(unsigned int msg_type, unsigned int id,
                             unsigned int uid,
                             llvm::ArrayRef<NT_Value> values) {
  if (m_proto_rev < 0x0300u) return;  // new message in version 3.0

  Write8(msg_type);
  Write16(id);
  Write16(uid);

  unsigned long len = 0;
  for (auto& value : values) len += GetValueSize(value);
  WriteUleb128(len);

  for (auto& value : values) WriteValue(value);
}
