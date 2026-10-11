// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RestApi.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <format>
#include <limits>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "Types_internal.hpp"
#include "server/MessagePackWriter.hpp"
#include "server/RestCodec.hpp"
#include "server/ServerStorage.hpp"
#include "wpi/net/HttpUtil.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Base64.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/raw_ostream.hpp"

using namespace wpi::nt;
using namespace wpi::nt::server;
using wpi::util::json;

namespace {
constexpr std::string_view TOPICS_PATH = "/nt/v1/topics";
constexpr std::string_view TOPIC_PREFIX = "/nt/v1/topics/";

RestResponse Error(int status, std::string_view statusText,
                   std::string_view message, std::string_view allow = {}) {
  return {status, statusText, json::object("error", message).to_string(),
          allow};
}

enum class Format { JSON, MSGPACK };

std::optional<Format> GetFormat(std::string_view contentType) {
  auto type = wpi::util::trim(contentType.substr(0, contentType.find(';')));
  if (wpi::util::equals_lower(type, "application/json")) {
    return Format::JSON;
  }
  if (wpi::util::equals_lower(type, "application/msgpack") ||
      wpi::util::equals_lower(type, "application/x-msgpack")) {
    return Format::MSGPACK;
  }
  return std::nullopt;
}

std::optional<Format> Negotiate(std::string_view accept,
                                bool jsonOnly = false) {
  if (accept.empty()) {
    return Format::JSON;
  }
  struct Preference {
    int specificity = -1;
    double quality = 0;
  };
  Preference jsonPref;
  Preference msgpackPref;
  wpi::util::split(accept, ',', -1, false, [&](std::string_view range) {
    auto media = wpi::util::trim(range.substr(0, range.find(';')));
    double quality = 1;
    auto paramPos = range.find(';');
    while (paramPos != range.npos) {
      range.remove_prefix(paramPos + 1);
      paramPos = range.find(';');
      auto param = wpi::util::trim(range.substr(0, paramPos));
      auto equals = param.find('=');
      if (wpi::util::equals_lower(wpi::util::trim(param.substr(0, equals)),
                                  "q")) {
        if (equals == param.npos) {
          quality = 0;
          break;
        }
        auto value = wpi::util::trim(param.substr(equals + 1));
        auto [end, error] =
            std::from_chars(value.data(), value.data() + value.size(), quality);
        if (error != std::errc{} || end != value.data() + value.size() ||
            !std::isfinite(quality) || quality < 0 || quality > 1) {
          quality = 0;
        }
      }
    }
    auto update = [&](Preference& pref, int specificity) {
      if (specificity > pref.specificity ||
          (specificity == pref.specificity && quality > pref.quality)) {
        pref = {specificity, quality};
      }
    };
    if (media == "*/*") {
      update(jsonPref, 0);
      update(msgpackPref, 0);
    } else if (wpi::util::equals_lower(media, "application/*")) {
      update(jsonPref, 1);
      update(msgpackPref, 1);
    } else if (auto format = GetFormat(media)) {
      update(*format == Format::JSON ? jsonPref : msgpackPref, 2);
    }
  });
  if (jsonOnly) {
    return jsonPref.quality > 0 ? std::optional{Format::JSON} : std::nullopt;
  }
  if (jsonPref.quality == 0 && msgpackPref.quality == 0) {
    return std::nullopt;
  }
  return msgpackPref.quality > jsonPref.quality ? Format::MSGPACK
                                                : Format::JSON;
}

RestResponse Respond(int status, std::string_view statusText, const json& value,
                     std::string_view allow, Format format) {
  if (format == Format::MSGPACK) {
    auto encoded = RestEncodeMessagePack(value);
    if (!encoded) {
      return Error(406, "Not Acceptable",
                   "Value exceeds MessagePack codec limits");
    }
    return {status, statusText, std::move(*encoded), allow,
            "application/msgpack"};
  }
  return {status, statusText, value.to_string(), allow};
}

std::expected<json, std::string_view> DecodeBody(std::string_view body,
                                                 Format format) {
  if (format == Format::MSGPACK) {
    return RestDecodeMessagePack(body);
  }
  auto parsed = json::parse(body);
  if (!parsed) {
    return std::unexpected("Invalid JSON");
  }
  return std::move(*parsed);
}

json EncodeNumber(double value) {
  return std::isfinite(value) ? json{value} : json{};
}

template <typename T>
json EncodeNumbers(std::span<const T> values) {
  auto result = json::array();
  for (auto value : values) {
    result.emplace_back(EncodeNumber(value));
  }
  return result;
}

json EncodeValue(const Value& value, Format format = Format::JSON) {
  switch (value.type()) {
    case NT_BOOLEAN:
      return value.GetBoolean();
    case NT_INTEGER:
      return value.GetInteger();
    case NT_FLOAT:
      return format == Format::JSON ? EncodeNumber(value.GetFloat())
                                    : json{value.GetFloat()};
    case NT_DOUBLE:
      return format == Format::JSON ? EncodeNumber(value.GetDouble())
                                    : json{value.GetDouble()};
    case NT_STRING:
      return value.GetString();
    case NT_RAW:
    case NT_RPC: {
      std::string encoded;
      wpi::util::Base64Encode(value.GetRaw(), &encoded);
      return encoded;
    }
    case NT_BOOLEAN_ARRAY: {
      auto result = json::array();
      for (auto elem : value.GetBooleanArray()) {
        result.emplace_back(elem != 0);
      }
      return result;
    }
    case NT_INTEGER_ARRAY:
      return value.GetIntegerArray();
    case NT_FLOAT_ARRAY:
      return format == Format::JSON ? EncodeNumbers(value.GetFloatArray())
                                    : json{value.GetFloatArray()};
    case NT_DOUBLE_ARRAY:
      return format == Format::JSON ? EncodeNumbers(value.GetDoubleArray())
                                    : json{value.GetDoubleArray()};
    case NT_STRING_ARRAY:
      return value.GetStringArray();
    default:
      return nullptr;
  }
}

json EncodeTopic(const ServerTopic& topic, Format format) {
  return json::object("name", topic.name, "type", topic.typeStr, "properties",
                      topic.properties, "value",
                      EncodeValue(topic.lastValue, format), "timestamp",
                      topic.lastValue ? topic.lastValue.time() : 0);
}

template <typename T, typename Check, typename Get, typename Make>
Value DecodeArray(const json& input, Check check, Get get, Make make) {
  if (!input.is_array()) {
    return {};
  }
  std::vector<T> values;
  values.reserve(input.get_array().size());
  for (auto&& elem : input.get_array()) {
    if (!check(elem)) {
      return {};
    }
    values.emplace_back(get(elem));
  }
  return make(values);
}

Value DecodeValue(std::string_view type, const json& input,
                  bool requireFinite = true) {
  auto time = Now();
  auto isBool = [](const json& j) { return j.is_bool(); };
  auto isInt = [](const json& j) { return j.is_int(); };
  auto isNumber = [&](const json& j) {
    return j.is_number() && (!requireFinite || std::isfinite(j.get_number()));
  };
  auto isFloat = [&](const json& j) {
    return isNumber(j) &&
           (!requireFinite ||
            std::abs(j.get_number()) <= std::numeric_limits<float>::max());
  };
  auto isString = [](const json& j) { return j.is_string(); };
  auto getBool = [](const json& j) { return j.get_bool(); };
  auto getInt = [](const json& j) { return j.get_int(); };
  auto getNumber = [](const json& j) { return j.get_number(); };
  auto getString = [](const json& j) { return j.get_string(); };
  switch (StringToType(type)) {
    case NT_BOOLEAN:
      return isBool(input) ? Value::MakeBoolean(input.get_bool(), time)
                           : Value{};
    case NT_INTEGER:
      return isInt(input) ? Value::MakeInteger(input.get_int(), time) : Value{};
    case NT_FLOAT:
      return isFloat(input) ? Value::MakeFloat(input.get_number(), time)
                            : Value{};
    case NT_DOUBLE:
      return isNumber(input) ? Value::MakeDouble(input.get_number(), time)
                             : Value{};
    case NT_STRING:
      return isString(input) ? Value::MakeString(input.get_string(), time)
                             : Value{};
    case NT_BOOLEAN_ARRAY:
      return DecodeArray<int>(input, isBool, getBool, [&](auto& v) {
        return Value::MakeBooleanArray(v, time);
      });
    case NT_INTEGER_ARRAY:
      return DecodeArray<int64_t>(input, isInt, getInt, [&](auto& v) {
        return Value::MakeIntegerArray(v, time);
      });
    case NT_FLOAT_ARRAY:
      return DecodeArray<float>(input, isFloat, getNumber, [&](auto& v) {
        return Value::MakeFloatArray(v, time);
      });
    case NT_DOUBLE_ARRAY:
      return DecodeArray<double>(input, isNumber, getNumber, [&](auto& v) {
        return Value::MakeDoubleArray(v, time);
      });
    case NT_STRING_ARRAY:
      return DecodeArray<std::string>(input, isString, getString, [&](auto& v) {
        return Value::MakeStringArray(v, time);
      });
    default: {
      if (!input.is_string()) {
        return {};
      }
      std::vector<uint8_t> bytes;
      wpi::util::Base64Decode(input.get_string(), &bytes);
      // Require canonical base64; the shared decoder accepts partial input.
      std::string encoded;
      wpi::util::Base64Encode(bytes, &encoded);
      if (encoded != input.get_string()) {
        return {};
      }
      return Value::MakeRaw(std::move(bytes), time);
    }
  }
}

RestResponse ReadValue(const ServerTopic& topic, std::string_view allow,
                       Format format) {
  if (topic.lastValue && topic.typeStr == "msgpack") {
    auto bytes = topic.lastValue.GetRaw();
    std::string_view data{reinterpret_cast<const char*>(bytes.data()),
                          bytes.size()};
    if (!RestValidateMessagePack(data)) {
      return Error(422, "Unprocessable Content",
                   "Stored value is not valid supported MessagePack");
    }
    if (format == Format::MSGPACK) {
      return {200, "OK", std::string{data}, allow, "application/msgpack"};
    }
    auto decoded = RestDecodeMessagePack(data);
    if (!decoded) {
      return Error(406, "Not Acceptable", decoded.error());
    }
    return Respond(200, "OK", *decoded, allow, format);
  }
  if (topic.lastValue && topic.typeStr == "json") {
    auto decoded = json::parse(topic.lastValue.GetString());
    if (!decoded) {
      return Error(422, "Unprocessable Content",
                   "Stored value is not valid JSON");
    }
    return Respond(200, "OK", *decoded, allow, format);
  }
  if (topic.lastValue && topic.lastValue.IsRaw() && format == Format::MSGPACK) {
    Writer writer;
    auto bytes = topic.lastValue.GetRaw();
    mpack::mpack_write_bin(&writer, reinterpret_cast<const char*>(bytes.data()),
                           bytes.size());
    mpack::mpack_writer_destroy(&writer);
    return {200, "OK",
            std::string{reinterpret_cast<const char*>(writer.bytes.data()),
                        writer.bytes.size()},
            allow, "application/msgpack"};
  }
  return Respond(200, "OK", EncodeValue(topic.lastValue, format), allow,
                 format);
}

Value DecodeDirectValue(std::string_view type, std::string_view body,
                        Format format) {
  if (type == "msgpack" && format == Format::MSGPACK) {
    if (!RestValidateMessagePack(body)) {
      return {};
    }
    return Value::MakeRaw(
        std::span{reinterpret_cast<const uint8_t*>(body.data()), body.size()},
        Now());
  }
  if (StringToType(type) == NT_RAW && type != "msgpack" &&
      format == Format::MSGPACK) {
    mpack::mpack_reader_t reader;
    mpack::mpack_reader_init_data(&reader, body.data(), body.size());
    auto count = mpack::mpack_expect_bin(&reader);
    auto data = mpack::mpack_read_bytes_inplace(&reader, count);
    mpack::mpack_done_bin(&reader);
    auto remaining = mpack::mpack_reader_remaining(&reader, nullptr);
    auto error = mpack::mpack_reader_destroy(&reader);
    if (error != mpack::mpack_ok || remaining != 0) {
      return {};
    }
    return Value::MakeRaw(
        std::span{reinterpret_cast<const uint8_t*>(data), count}, Now());
  }
  auto input = DecodeBody(body, format);
  if (!input) {
    return {};
  }
  if (type == "msgpack") {
    auto encoded = RestEncodeMessagePack(*input);
    if (!encoded) {
      return {};
    }
    return Value::MakeRaw(
        std::span{reinterpret_cast<const uint8_t*>(encoded->data()),
                  encoded->size()},
        Now());
  }
  if (type == "json") {
    return Value::MakeString(input->to_string(), Now());
  }
  return DecodeValue(type, *input);
}

bool ValidProperties(const json& props) {
  if (!props.is_object()) {
    return false;
  }
  for (auto key : {"persistent", "retained", "cached"}) {
    if (auto value = props.lookup(key);
        value && !value->is_null() && !value->is_bool()) {
      return false;
    }
  }
  return true;
}

RestResponse HandlePersistentRequest(ServerStorage& storage,
                                     std::string_view method,
                                     std::string_view body,
                                     std::string_view contentType,
                                     std::string_view accept) {
  constexpr std::string_view ALLOW = "GET, PUT, OPTIONS";
  if (method == "OPTIONS") {
    return {204, "No Content", {}, ALLOW};
  }
  if (method == "GET") {
    if (!Negotiate(accept, true)) {
      return Error(406, "Not Acceptable",
                   "Persistence files require application/json");
    }
    std::string data;
    wpi::util::raw_string_ostream os{data};
    storage.DumpPersistent(os);
    os.flush();
    return {200, "OK", std::move(data), ALLOW};
  }
  if (method != "PUT") {
    return Error(405, "Method Not Allowed", "Method not allowed", ALLOW);
  }
  if (GetFormat(contentType) != Format::JSON) {
    return Error(415, "Unsupported Media Type",
                 "Persistence uploads require application/json");
  }
  auto input = json::parse(body);
  if (!input || !input->is_array()) {
    return Error(400, "Bad Request", "Expected a persistent JSON array");
  }

  struct PendingEntry {
    const json* entry;
    Value value;
  };
  std::vector<PendingEntry> pending;
  wpi::util::StringMap<bool> names;
  // Validate the entire file before notifying subscribers or changing storage.
  for (auto&& entry : input->get_array()) {
    auto error = [&](int status, std::string_view statusText,
                     std::string_view message) {
      return Error(status, statusText,
                   std::format("entry {}: {}", pending.size(), message));
    };
    if (!entry.is_object()) {
      return error(400, "Bad Request", "Expected an object");
    }
    auto name = entry.lookup("name");
    auto type = entry.lookup("type");
    auto props = entry.lookup("properties");
    auto jvalue = entry.lookup("value");
    if (!name || !name->is_string() || !type || !type->is_string() ||
        type->get_string().empty() || !props || !ValidProperties(*props) ||
        !jvalue) {
      return error(400, "Bad Request",
                   "Expected name, type, value, and properties");
    }
    if (name->get_string().starts_with('$')) {
      return error(403, "Forbidden", "Meta topics are read-only");
    }
    if (!names.try_emplace(name->get_string(), true).second) {
      return error(400, "Bad Request", "Duplicate topic name");
    }
    auto persistent = props->lookup("persistent");
    if (!persistent || !persistent->is_bool() || !persistent->get_bool()) {
      return error(400, "Bad Request",
                   "Every entry must have persistent: true");
    }
    if (auto cached = props->lookup("cached");
        cached && cached->is_bool() && !cached->get_bool()) {
      return error(400, "Bad Request",
                   "Persistent entries must cache their values");
    }
    auto topic = storage.GetTopic(name->get_string());
    if (topic && topic->typeStr != type->get_string()) {
      return error(409, "Conflict", "Topic type cannot be changed");
    }
    // Match the startup loader's numeric conversions, including infinity and
    // rounded decimal representations of the maximum float value.
    auto value = DecodeValue(type->get_string(), *jvalue, false);
    if (!value) {
      return error(400, "Bad Request", "Value does not match the topic type");
    }
    pending.emplace_back(&entry, std::move(value));
  }

  for (auto&& item : pending) {
    auto& entry = *item.entry;
    auto& name = entry.at("name").get_string();
    auto& props = entry.at("properties");
    auto topic = storage.GetTopic(name);
    if (!topic) {
      topic = storage.CreateTopic(nullptr, name, entry.at("type").get_string(),
                                  props);
    } else {
      auto update = props;
      for (auto&& [key, value] : topic->properties.get_object()) {
        if (!props.lookup(key)) {
          update[key] = nullptr;
        }
      }
      storage.SetProperties(nullptr, topic, update);
    }
    // Imports must replace the cached value even if a publisher used a future
    // timestamp.
    item.value.SetTime(std::max(item.value.time(), topic->lastValue.time()));
    storage.SetValue(nullptr, topic, item.value);
  }
  return {204, "No Content", {}, ALLOW};
}

}  // namespace

RestResponse wpi::nt::server::HandleRestRequest(ServerStorage& storage,
                                                std::string_view method,
                                                std::string_view target,
                                                std::string_view body,
                                                std::string_view contentType,
                                                std::string_view accept) {
  // Split the request target directly so URL normalization cannot change topic
  // names containing dot segments. Decode topic names exactly once, preserving
  // +.
  auto queryPos = target.find('?');
  auto path = target.substr(0, queryPos);
  if (path == "/nt/v1/persistent.json" || path == "/nt/persistent.json") {
    return HandlePersistentRequest(storage, method, body, contentType, accept);
  }
  bool collection = path == TOPICS_PATH;
  if (!collection && !path.starts_with(TOPIC_PREFIX)) {
    return Error(404, "Not Found", "Resource not found");
  }
  auto encodedName =
      collection ? std::string_view{} : path.substr(TOPIC_PREFIX.size());
  ada::url_search_params query{
      queryPos == target.npos ? std::string_view{} : target.substr(queryPos)};
  std::string_view field;
  if (!collection && query.size() != 0) {
    if (query.size() != 1) {
      return Error(400, "Bad Request", "Expected one topic field selector");
    }
    auto [key, value] = *query.get_entries().next();
    if (!value.empty()) {
      return Error(400, "Bad Request",
                   "Topic field selector must have no value");
    }
    field = key;
    if (field != "type" && field != "value" && field != "properties" &&
        field != "name" && field != "timestamp") {
      return Error(400, "Bad Request", "Unknown topic field selector");
    }
  }
  std::string_view allow = "GET, OPTIONS";
  if (!collection && field.empty()) {
    allow = "GET, PUT, PATCH, DELETE, OPTIONS";
  } else if (field == "value") {
    allow = "GET, PUT, OPTIONS";
  } else if (field == "properties") {
    allow = "GET, PUT, PATCH, OPTIONS";
  }
  if (method == "OPTIONS") {
    return {204, "No Content", {}, allow};
  }
  bool writeValue = method == "PUT" && (field.empty() || field == "value" ||
                                        field == "properties");
  bool writeProperties =
      method == "PATCH" && (field.empty() || field == "properties");
  bool deleteTopic = method == "DELETE" && field.empty();
  if (method != "GET" &&
      (collection || !(writeValue || writeProperties || deleteTopic))) {
    return Error(405, "Method Not Allowed", "Method not allowed", allow);
  }
  auto outputFormat = Negotiate(accept);
  if (!outputFormat &&
      (method == "GET" || (method == "PUT" && field.empty()))) {
    return Error(406, "Not Acceptable",
                 "Accept must allow application/json or application/msgpack");
  }
  auto inputFormat = GetFormat(contentType);
  if ((method == "PUT" || method == "PATCH") && !inputFormat) {
    return Error(
        415, "Unsupported Media Type",
        "Content-Type must be application/json or application/msgpack");
  }
  if (collection) {
    auto prefix = query.get("prefix").value_or("");
    auto result = json::array();
    storage.ForEachTopic([&](ServerTopic* topic) {
      if (topic->name.starts_with(prefix) &&
          (!topic->special || prefix.starts_with('$'))) {
        result.emplace_back(EncodeTopic(*topic, *outputFormat));
      }
    });
    return Respond(200, "OK", result, allow, *outputFormat);
  }

  std::string name;
  for (size_t i = 0; i < encodedName.size(); ++i) {
    char c = encodedName[i];
    if (c == '%') {
      if (i + 2 >= encodedName.size()) {
        return Error(400, "Bad Request", "Invalid topic name encoding");
      }
      auto hi = wpi::util::hexDigitValue(encodedName[++i]);
      auto lo = wpi::util::hexDigitValue(encodedName[++i]);
      if (hi == -1U || lo == -1U) {
        return Error(400, "Bad Request", "Invalid topic name encoding");
      }
      c = (hi << 4) | lo;
    }
    name += c;
  }
  if (name.empty()) {
    return Error(400, "Bad Request", "Topic name must not be empty");
  }
  auto topic = storage.GetTopic(name);
  if (!field.empty() && !topic) {
    return Error(404, "Not Found", "Topic not found");
  }
  if (method == "GET") {
    if (!topic) {
      return Error(404, "Not Found", "Topic not found");
    }
    if (field == "value") {
      return ReadValue(*topic, allow, *outputFormat);
    }
    json value;
    if (field == "type") {
      value = topic->typeStr;
    } else if (field == "properties") {
      value = topic->properties;
    } else if (field == "name") {
      value = topic->name;
    } else if (field == "timestamp") {
      value = topic->lastValue ? topic->lastValue.time() : 0;
    } else {
      value = EncodeTopic(*topic, *outputFormat);
    }
    return Respond(200, "OK", value, allow, *outputFormat);
  }
  if (name.starts_with('$')) {
    return Error(403, "Forbidden", "Meta topics are read-only");
  }
  if (method == "DELETE") {
    if (!topic) {
      return Error(404, "Not Found", "Topic not found");
    }
    if (topic->publisherCount != 0) {
      return Error(409, "Conflict", "Topic has active publishers");
    }
    // Clearing persistence first also marks the persistent file dirty.
    storage.SetProperties(
        nullptr, topic,
        json::object("persistent", nullptr, "retained", nullptr));
    return {204, "No Content", {}, allow};
  }
  if (field == "value") {
    auto value = DecodeDirectValue(topic->typeStr, body, *inputFormat);
    if (!value) {
      return Error(400, "Bad Request",
                   "Invalid value or encoding for topic type");
    }
    storage.SetValue(nullptr, topic, value);
    return {204, "No Content", {}, allow};
  }
  auto input = DecodeBody(body, *inputFormat);
  if (!input || !input->is_object()) {
    return Error(400, "Bad Request", "Expected an encoded object");
  }
  if (field == "properties") {
    if (!ValidProperties(*input)) {
      return Error(400, "Bad Request",
                   "Expected properties object with boolean or null flags");
    }
    if (method == "PUT") {
      // Replacement removes keys omitted by the caller; apply it in one update.
      for (auto&& [key, value] : topic->properties.get_object()) {
        if (!input->lookup(key)) {
          (*input)[key] = nullptr;
        }
      }
    }
    storage.SetProperties(nullptr, topic, *input);
    return {204, "No Content", {}, allow};
  }
  auto props = input->lookup("properties");
  if (props && !ValidProperties(*props)) {
    return Error(400, "Bad Request",
                 "Expected properties object with boolean or null flags");
  }
  if (method == "PATCH") {
    if (!topic) {
      return Error(404, "Not Found", "Topic not found");
    }
    if (!props || input->get_object().size() != 1) {
      return Error(400, "Bad Request",
                   "PATCH requires only a properties object");
    }
    storage.SetProperties(nullptr, topic, *props);
    return {204, "No Content", {}, allow};
  }
  for (auto&& [key, value] : input->get_object()) {
    if (key != "type" && key != "value" && key != "properties") {
      return Error(400, "Bad Request", "Unknown topic field");
    }
  }
  auto type = input->lookup("type");
  if ((type && (!type->is_string() || type->get_string().empty())) ||
      (!topic && !type)) {
    return Error(400, "Bad Request",
                 "A new topic requires a nonempty type string");
  }
  if (topic && type && type->get_string() != topic->typeStr) {
    return Error(409, "Conflict", "Topic type cannot be changed");
  }
  std::string_view typeStr =
      topic ? std::string_view{topic->typeStr} : type->get_string();
  auto jvalue = input->lookup("value");
  auto value = jvalue ? DecodeValue(typeStr, *jvalue) : Value{};
  if (!value) {
    return Error(400, "Bad Request",
                 "Value is missing or does not match the topic type");
  }
  bool created = !topic;
  if (created) {
    // There is no connection-scoped publisher to keep REST topics alive.
    auto properties = json::object("retained", true);
    if (props) {
      for (auto&& [key, prop] : props->get_object()) {
        if (prop.is_null()) {
          properties.erase(key);
        } else {
          properties[key] = prop;
        }
      }
    }
    auto retained = properties.lookup("retained");
    auto persistent = properties.lookup("persistent");
    if (!(retained && retained->get_bool()) &&
        !(persistent && persistent->get_bool())) {
      return Error(400, "Bad Request",
                   "A new topic must be retained or persistent");
    }
    topic = storage.CreateTopic(nullptr, name, typeStr, properties);
  } else if (props) {
    storage.SetProperties(nullptr, topic, *props);
    // A property update can unpublish the topic.
    topic = storage.GetTopic(name);
  }
  if (!topic) {
    return {204, "No Content", {}, allow};
  }
  storage.SetValue(nullptr, topic, value);
  return Respond(created ? 201 : 200, created ? "Created" : "OK",
                 EncodeTopic(*topic, *outputFormat), allow, *outputFormat);
}
