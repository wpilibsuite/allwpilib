// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestPrinters.h"

#include <wpi/json.h>

#include "Handle.h"
#include "PubSubOptions.h"
#include "net/Message.h"
#include "net3/Message3.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_cpp.h"

namespace wpi {
void PrintTo(const json& val, ::std::ostream* os) {
  *os << val.dump();
}
}  // namespace wpi

namespace nt {

void PrintTo(const Event& event, std::ostream* os) {
  *os << "Event{listener=";
  PrintTo(Handle{event.listener}, os);
  *os << ", flags=" << event.flags;
  // *os << ", name=\"" << event.name << "\", flags=" << event.flags
  // << "value=";
  // PrintTo(event.value, os);
  *os << '}';
}

void PrintTo(const Handle& handle, std::ostream* os) {
  *os << "Handle{";
  switch (handle.GetType()) {
    case Handle::kListener:
      *os << "kListener";
      break;
    case Handle::kListenerPoller:
      *os << "kListenerPoller";
      break;
    case Handle::kEntry:
      *os << "kEntry";
      break;
    case Handle::kInstance:
      *os << "kInstance";
      break;
    case Handle::kTopic:
      *os << "kTopic";
      break;
    case Handle::kSubscriber:
      *os << "kSubscriber";
      break;
    case Handle::kPublisher:
      *os << "kPublisher";
      break;
    default:
      *os << "UNKNOWN";
      break;
  }
  *os << ", " << handle.GetInst() << ", " << handle.GetIndex() << '}';
}

void PrintTo(const net3::Message3& msg, std::ostream* os) {
  *os << "Message{";
  switch (msg.type()) {
    case net3::Message3::kKeepAlive:
      *os << "kKeepAlive";
      break;
    case net3::Message3::kClientHello:
      *os << "kClientHello";
      break;
    case net3::Message3::kProtoUnsup:
      *os << "kProtoUnsup";
      break;
    case net3::Message3::kServerHelloDone:
      *os << "kServerHelloDone";
      break;
    case net3::Message3::kServerHello:
      *os << "kServerHello";
      break;
    case net3::Message3::kClientHelloDone:
      *os << "kClientHelloDone";
      break;
    case net3::Message3::kEntryAssign:
      *os << "kEntryAssign";
      break;
    case net3::Message3::kEntryUpdate:
      *os << "kEntryUpdate";
      break;
    case net3::Message3::kFlagsUpdate:
      *os << "kFlagsUpdate";
      break;
    case net3::Message3::kEntryDelete:
      *os << "kEntryDelete";
      break;
    case net3::Message3::kClearEntries:
      *os << "kClearEntries";
      break;
    case net3::Message3::kExecuteRpc:
      *os << "kExecuteRpc";
      break;
    case net3::Message3::kRpcResponse:
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
      *os << "boolean, " << (value.GetBoolean() ? "true" : "false");
      break;
    case NT_DOUBLE:
      *os << "double, " << value.GetDouble();
      break;
    case NT_FLOAT:
      *os << "float, " << value.GetFloat();
      break;
    case NT_INTEGER:
      *os << "int, " << value.GetInteger();
      break;
    case NT_STRING:
      *os << "string, \"" << value.GetString() << '"';
      break;
    case NT_RAW:
      *os << "raw, " << ::testing::PrintToString(value.GetRaw());
      break;
    case NT_BOOLEAN_ARRAY:
      *os << "boolean[], " << ::testing::PrintToString(value.GetBooleanArray());
      break;
    case NT_DOUBLE_ARRAY:
      *os << "double[], " << ::testing::PrintToString(value.GetDoubleArray());
      break;
    case NT_FLOAT_ARRAY:
      *os << "float[], " << ::testing::PrintToString(value.GetFloatArray());
      break;
    case NT_INTEGER_ARRAY:
      *os << "int[], " << ::testing::PrintToString(value.GetIntegerArray());
      break;
    case NT_STRING_ARRAY:
      *os << "string[], " << ::testing::PrintToString(value.GetStringArray());
      break;
    default:
      *os << "UNKNOWN TYPE " << value.type();
      break;
  }
  *os << '}';
}

void PrintTo(const PubSubOptions& options, std::ostream* os) {
  *os << "PubSubOptions{periodic=" << options.periodic
      << ", pollStorageSize=" << options.pollStorageSize
      << ", logging=" << options.sendAll
      << ", keepDuplicates=" << options.keepDuplicates << '}';
}

}  // namespace nt
