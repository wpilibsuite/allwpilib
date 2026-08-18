// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestPrinters.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

#include "Handle.hpp"
#include "PubSubOptions.hpp"
#include "net/Message.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/fmt/raw_ostream.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace {

template <typename T>
void WriteRangeElement(const T& elem, wpi::util::raw_ostream& os) {
  using Elem = std::decay_t<T>;
  if constexpr (std::is_convertible_v<Elem, std::string_view>) {
    os << elem;
  } else {
    wpi::util::print(os, "{}", elem);
  }
}

template <typename Range>
void WriteRange(const Range& range, wpi::util::raw_ostream& os) {
  os << '{';
  bool first = true;
  for (const auto& elem : range) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    WriteRangeElement(elem, os);
  }
  os << '}';
}

template <typename Range>
void WriteStringRange(const Range& range, wpi::util::raw_ostream& os) {
  os << '{';
  bool first = true;
  for (const auto& elem : range) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << '"' << elem << '"';
  }
  os << '}';
}

std::string JsonToString(const wpi::util::json& value) {
  std::string out;
  wpi::util::raw_string_ostream os{out};
  value.marshal(os);
  return os.str();
}

void WriteOptionalInt(std::optional<int> value, wpi::util::raw_ostream& os) {
  if (value) {
    wpi::util::print(os, "{}", *value);
  } else {
    os << "nullopt";
  }
}

void WritePubSubOptions(const wpi::nt::PubSubOptionsImpl& options,
                        wpi::util::raw_ostream& os) {
  wpi::util::print(os,
                   "PubSubOptions{{periodicMs={}, pollStorage={}, sendAll={}, "
                   "keepDuplicates={}, topicsOnly={}, prefixMatch={}, "
                   "disableSignal={}}}",
                   options.periodicMs, options.pollStorage, options.sendAll,
                   options.keepDuplicates, options.topicsOnly,
                   options.prefixMatch, options.disableSignal);
}

void WriteHandle(const wpi::nt::Handle& handle, wpi::util::raw_ostream& os) {
  os << "Handle{";
  switch (handle.GetType()) {
    case wpi::nt::Handle::LISTENER:
      os << "LISTENER";
      break;
    case wpi::nt::Handle::LISTENER_POLLER:
      os << "LISTENER_POLLER";
      break;
    case wpi::nt::Handle::ENTRY:
      os << "ENTRY";
      break;
    case wpi::nt::Handle::INSTANCE:
      os << "INSTANCE";
      break;
    case wpi::nt::Handle::TOPIC:
      os << "kTopic";
      break;
    case wpi::nt::Handle::SUBSCRIBER:
      os << "SUBSCRIBER";
      break;
    case wpi::nt::Handle::PUBLISHER:
      os << "PUBLISHER";
      break;
    default:
      os << "UNKNOWN";
      break;
  }
  wpi::util::print(os, ", {}, {}}}", handle.GetInst(), handle.GetIndex());
}

void WriteValue(const wpi::nt::Value& value, wpi::util::raw_ostream& os) {
  os << "Value{";
  switch (value.type()) {
    case NT_UNASSIGNED:
      break;
    case NT_BOOLEAN:
      os << "boolean, " << (value.GetBoolean() ? "true" : "false");
      break;
    case NT_DOUBLE:
      wpi::util::print(os, "double, {}", value.GetDouble());
      break;
    case NT_FLOAT:
      wpi::util::print(os, "float, {}", value.GetFloat());
      break;
    case NT_INTEGER:
      wpi::util::print(os, "int, {}", value.GetInteger());
      break;
    case NT_STRING:
      os << "string, \"" << value.GetString() << '"';
      break;
    case NT_RAW:
      os << "raw, ";
      WriteRange(value.GetRaw(), os);
      break;
    case NT_BOOLEAN_ARRAY:
      os << "boolean[], ";
      WriteRange(value.GetBooleanArray(), os);
      break;
    case NT_DOUBLE_ARRAY:
      os << "double[], ";
      WriteRange(value.GetDoubleArray(), os);
      break;
    case NT_FLOAT_ARRAY:
      os << "float[], ";
      WriteRange(value.GetFloatArray(), os);
      break;
    case NT_INTEGER_ARRAY:
      os << "int[], ";
      WriteRange(value.GetIntegerArray(), os);
      break;
    case NT_STRING_ARRAY:
      os << "string[], ";
      WriteRange(value.GetStringArray(), os);
      break;
    default:
      wpi::util::print(os, "UNKNOWN TYPE {}", static_cast<int>(value.type()));
      break;
  }
  os << '}';
}

void WriteClientMessage(const wpi::nt::net::ClientMessage& msg,
                        wpi::util::raw_ostream& os) {
  std::visit(
      [&](const auto& contents) {
        using T = std::decay_t<decltype(contents)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          os << "ClientMessage{}";
        } else if constexpr (std::is_same_v<T, wpi::nt::net::PublishMsg>) {
          wpi::util::print(os,
                           "ClientMessage{{PublishMsg{{pubuid={}, name=\"{}\", "
                           "typeStr=\"{}\", properties={}, options=",
                           contents.pubuid, contents.name, contents.typeStr,
                           JsonToString(contents.properties));
          WritePubSubOptions(contents.options, os);
          os << "}}";
        } else if constexpr (std::is_same_v<T, wpi::nt::net::UnpublishMsg>) {
          wpi::util::print(os, "ClientMessage{{UnpublishMsg{{pubuid={}}}}}",
                           contents.pubuid);
        } else if constexpr (std::is_same_v<T,
                                            wpi::nt::net::SetPropertiesMsg>) {
          wpi::util::print(os,
                           "ClientMessage{{SetPropertiesMsg{{name=\"{}\", "
                           "update={}}}}}",
                           contents.name, JsonToString(contents.update));
        } else if constexpr (std::is_same_v<T, wpi::nt::net::SubscribeMsg>) {
          wpi::util::print(os,
                           "ClientMessage{{SubscribeMsg{{subuid={}, "
                           "topicNames=",
                           contents.subuid);
          WriteStringRange(contents.topicNames, os);
          os << ", options=";
          WritePubSubOptions(contents.options, os);
          os << "}}";
        } else if constexpr (std::is_same_v<T, wpi::nt::net::UnsubscribeMsg>) {
          wpi::util::print(os, "ClientMessage{{UnsubscribeMsg{{subuid={}}}}}",
                           contents.subuid);
        } else if constexpr (std::is_same_v<T, wpi::nt::net::ClientValueMsg>) {
          wpi::util::print(os,
                           "ClientMessage{{ClientValueMsg{{pubuid={}, value=",
                           contents.pubuid);
          WriteValue(contents.value, os);
          os << "}}";
        }
      },
      msg.contents);
}

void WriteServerMessage(const wpi::nt::net::ServerMessage& msg,
                        wpi::util::raw_ostream& os) {
  std::visit(
      [&](const auto& contents) {
        using T = std::decay_t<decltype(contents)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          os << "ServerMessage{}";
        } else if constexpr (std::is_same_v<T, wpi::nt::net::AnnounceMsg>) {
          wpi::util::print(os,
                           "ServerMessage{{AnnounceMsg{{name=\"{}\", id={}, "
                           "typeStr=\"{}\", pubuid=",
                           contents.name, contents.id, contents.typeStr);
          WriteOptionalInt(contents.pubuid, os);
          wpi::util::print(os, ", properties={}}}}}",
                           JsonToString(contents.properties));
        } else if constexpr (std::is_same_v<T, wpi::nt::net::UnannounceMsg>) {
          wpi::util::print(
              os, "ServerMessage{{UnannounceMsg{{name=\"{}\", id={}}}}}",
              contents.name, contents.id);
        } else if constexpr (std::is_same_v<
                                 T, wpi::nt::net::PropertiesUpdateMsg>) {
          wpi::util::print(os,
                           "ServerMessage{{PropertiesUpdateMsg{{name=\"{}\", "
                           "update={}, ack={}}}}}",
                           contents.name, JsonToString(contents.update),
                           contents.ack);
        } else if constexpr (std::is_same_v<T, wpi::nt::net::ServerValueMsg>) {
          wpi::util::print(os,
                           "ServerMessage{{ServerValueMsg{{topic={}, value=",
                           contents.topic);
          WriteValue(contents.value, os);
          os << "}}";
        }
      },
      msg.contents);
}

template <typename T, typename Writer>
std::string ToString(const T& value, Writer writer) {
  std::string out;
  wpi::util::raw_string_ostream os{out};
  writer(value, os);
  return os.str();
}

}  // namespace

std::string Catch::StringMaker<wpi::nt::Event>::convert(
    const wpi::nt::Event& event) {
  return ToString(event, [](const auto& value, wpi::util::raw_ostream& os) {
    os << "Event{listener=";
    WriteHandle(wpi::nt::Handle{value.listener}, os);
    wpi::util::print(os, ", flags={}}}", value.flags);
  });
}

std::string Catch::StringMaker<wpi::nt::Handle>::convert(
    const wpi::nt::Handle& handle) {
  return ToString(handle, WriteHandle);
}

std::string Catch::StringMaker<wpi::nt::net::ClientMessage>::convert(
    const wpi::nt::net::ClientMessage& msg) {
  return ToString(msg, WriteClientMessage);
}

std::string Catch::StringMaker<wpi::nt::net::ServerMessage>::convert(
    const wpi::nt::net::ServerMessage& msg) {
  return ToString(msg, WriteServerMessage);
}

std::string Catch::StringMaker<wpi::nt::Value>::convert(
    const wpi::nt::Value& value) {
  return ToString(value, WriteValue);
}

std::string Catch::StringMaker<wpi::nt::PubSubOptionsImpl>::convert(
    const wpi::nt::PubSubOptionsImpl& options) {
  return ToString(options, WritePubSubOptions);
}
