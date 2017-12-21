/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "TestPrinters.h"

#include "Handle.h"
#include "Message.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_cpp.h"

namespace nt {

void PrintTo(const EntryNotification& event, std::ostream* os) {
  *os << "EntryNotification{listener=";
  PrintTo(Handle{event.listener}, os);
  *os << ", entry=";
  PrintTo(Handle{event.entry}, os);
  *os << ", name=\"" << event.name << "\", flags=" << event.flags << ", value=";
  PrintTo(event.value, os);
  *os << '}';
}

void PrintTo(const Handle& handle, std::ostream* os) {
  *os << "Handle{";
  switch (handle.GetType()) {
    case Handle::kConnectionListener:
      *os << "kConnectionListener";
      break;
    case Handle::kConnectionListenerPoller:
      *os << "kConnectionListenerPoller";
      break;
    case Handle::kEntry:
      *os << "kEntry";
      break;
    case Handle::kEntryListener:
      *os << "kEntryListener";
      break;
    case Handle::kEntryListenerPoller:
      *os << "kEntryListenerPoller";
      break;
    case Handle::kInstance:
      *os << "kInstance";
      break;
    case Handle::kLogger:
      *os << "kLogger";
      break;
    case Handle::kLoggerPoller:
      *os << "kLoggerPoller";
      break;
    case Handle::kRpcCall:
      *os << "kRpcCall";
      break;
    case Handle::kRpcCallPoller:
      *os << "kRpcCallPoller";
      break;
    default:
      *os << "UNKNOWN";
      break;
  }
  *os << ", " << handle.GetInst() << ", " << handle.GetIndex() << '}';
}

void PrintTo(const Message& msg, std::ostream* os) {
  *os << "Message{";
  switch (msg.type()) {
    case Message::kKeepAlive:
      *os << "kKeepAlive";
      break;
    case Message::kClientHello:
      *os << "kClientHello";
      break;
    case Message::kProtoUnsup:
      *os << "kProtoUnsup";
      break;
    case Message::kServerHelloDone:
      *os << "kServerHelloDone";
      break;
    case Message::kServerHello:
      *os << "kServerHello";
      break;
    case Message::kClientHelloDone:
      *os << "kClientHelloDone";
      break;
    case Message::kEntryAssign:
      *os << "kEntryAssign";
      break;
    case Message::kEntryUpdate:
      *os << "kEntryUpdate";
      break;
    case Message::kFlagsUpdate:
      *os << "kFlagsUpdate";
      break;
    case Message::kEntryDelete:
      *os << "kEntryDelete";
      break;
    case Message::kClearEntries:
      *os << "kClearEntries";
      break;
    case Message::kExecuteRpc:
      *os << "kExecuteRpc";
      break;
    case Message::kRpcResponse:
      *os << "kRpcResponse";
      break;
    default:
      *os << "UNKNOWN";
      break;
  }
  *os << ": str=\"" << msg.str() << "\", id=" << msg.id()
      << ", flags=" << msg.flags() << ", seq_num_uid=" << msg.seq_num_uid()
      << ", value=";
  PrintTo(msg.value(), os);
  *os << '}';
}

void PrintTo(const Value& value, std::ostream* os) {
  *os << "Value{";
  switch (value.type()) {
    case NT_UNASSIGNED:
      break;
    case NT_BOOLEAN:
      *os << (value.GetBoolean() ? "true" : "false");
      break;
    case NT_DOUBLE:
      *os << value.GetDouble();
      break;
    case NT_STRING:
      *os << '"' << value.GetString().str() << '"';
      break;
    case NT_RAW:
      *os << ::testing::PrintToString(value.GetRaw());
      break;
    case NT_BOOLEAN_ARRAY:
      *os << ::testing::PrintToString(value.GetBooleanArray());
      break;
    case NT_DOUBLE_ARRAY:
      *os << ::testing::PrintToString(value.GetDoubleArray());
      break;
    case NT_STRING_ARRAY:
      *os << ::testing::PrintToString(value.GetStringArray());
      break;
    case NT_RPC:
      *os << ::testing::PrintToString(value.GetRpc());
      break;
    default:
      *os << "UNKNOWN TYPE " << value.type();
      break;
  }
  *os << '}';
}

}  // namespace nt
