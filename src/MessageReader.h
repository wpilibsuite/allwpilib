/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_MESSAGEREADER_H_
#define NT_MESSAGEREADER_H_

#include "Value.h"
#include "WireDecoder.h"

namespace ntimpl {

class MessageHandler {
  void anchor();

 public:
  // Needed for protocol rev 2.0 ENTRY_UPDATE messages.
  virtual NT_Type GetEntryType(unsigned int id) = 0;

  virtual ~MessageHandler() {}
  virtual void GotKeepAlive() = 0;
  virtual void GotClientHello(unsigned int proto_rev,
                              StringValue&& self_id) = 0;
  virtual void GotProtoUnsup(unsigned int proto_rev) = 0;
  virtual void GotServerHelloDone() = 0;
  virtual void GotServerHello(unsigned int flags, StringValue&& self_id) = 0;
  virtual void GotClientHelloDone() = 0;
  virtual void GotEntryAssign(StringValue&& name, unsigned int id,
                              unsigned int seq_num, Value&& value,
                              unsigned int flags) = 0;
  virtual void GotEntryUpdate(unsigned int id, unsigned int seq_num,
                              Value&& value) = 0;
  virtual void GotFlagsUpdate(unsigned int id, unsigned int flags) = 0;
  virtual void GotEntryDelete(unsigned int id) = 0;
  virtual void GotClearEntries() = 0;
  virtual void GotExecuteRpc(unsigned int id, unsigned int uid,
                             llvm::StringRef params) = 0;
  virtual void GotRpcResponse(unsigned int id, unsigned int uid,
                              llvm::StringRef results) = 0;

  MessageHandler() = default;
  MessageHandler(const MessageHandler&) = delete;
  MessageHandler& operator=(const MessageHandler&) = delete;
};

class MessageReader : private WireDecoder {
 public:
  explicit MessageReader(MessageHandler& handler, raw_istream& is,
                         unsigned int proto_rev);
  ~MessageReader();

  bool ReadMessage();

  using WireDecoder::set_proto_rev;
  using WireDecoder::proto_rev;
  using WireDecoder::Reset;
  using WireDecoder::error;

  MessageReader(const MessageReader&) = delete;
  MessageReader& operator=(const MessageReader&) = delete;

 private:
  MessageHandler& m_handler;
};

}  // namespace ntimpl

#endif  // NT_MESSAGEREADER_H_
