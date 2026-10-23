// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RestCodec.hpp"

#include <cmath>
#include <limits>
#include <string>
#include <utility>

#include "server/MessagePackWriter.hpp"

using namespace mpack;
using namespace wpi::nt::server;
using wpi::util::json;

namespace {
constexpr unsigned int MAX_DEPTH = 64;
constexpr size_t MAX_NODES = 128 * 1024;

bool Read(mpack_reader_t& reader, json* out, unsigned int depth,
          size_t& nodes) {
  if (depth == 0 || nodes == 0 || mpack_reader_error(&reader) != mpack_ok) {
    mpack_reader_flag_error(&reader, mpack_error_too_big);
    return false;
  }
  --nodes;
  auto tag = mpack_read_tag(&reader);
  if (mpack_reader_error(&reader) != mpack_ok) {
    return false;
  }
  switch (mpack_tag_type(&tag)) {
    case mpack_type_nil:
      if (out) {
        *out = nullptr;
      }
      break;
    case mpack_type_bool:
      if (out) {
        *out = mpack_tag_bool_value(&tag);
      }
      break;
    case mpack_type_int:
      if (out) {
        *out = mpack_tag_int_value(&tag);
      }
      break;
    case mpack_type_uint:
      if (out) {
        auto value = mpack_tag_uint_value(&tag);
        *out = value <= std::numeric_limits<int64_t>::max()
                   ? json{static_cast<int64_t>(value)}
                   : json{value};
      }
      break;
    case mpack_type_float:
    case mpack_type_double:
      if (out) {
        double value = mpack_tag_type(&tag) == mpack_type_float
                           ? mpack_tag_float_value(&tag)
                           : mpack_tag_double_value(&tag);
        if (!std::isfinite(value)) {
          return false;
        }
        *out = value;
      }
      break;
    case mpack_type_str: {
      auto length = mpack_tag_str_length(&tag);
      auto bytes = mpack_read_utf8_inplace(&reader, length);
      if (!bytes) {
        return false;
      }
      if (out) {
        *out = std::string_view{bytes, length};
      }
      mpack_done_str(&reader);
      break;
    }
    case mpack_type_bin:
      if (out) {
        return false;
      }
      mpack_skip_bytes(&reader, mpack_tag_bin_length(&tag));
      mpack_done_bin(&reader);
      break;
    case mpack_type_array: {
      auto count = mpack_tag_array_count(&tag);
      // Each element needs at least one byte. Do not allocate from a wire
      // count.
      if (count > nodes || count > mpack_reader_remaining(&reader, nullptr)) {
        return false;
      }
      if (out) {
        *out = json::array();
      }
      for (uint32_t i = 0; i < count; ++i) {
        json elem;
        if (!Read(reader, out ? &elem : nullptr, depth - 1, nodes)) {
          return false;
        }
        if (out) {
          out->emplace_back(std::move(elem));
        }
      }
      mpack_done_array(&reader);
      break;
    }
    case mpack_type_map: {
      auto count = mpack_tag_map_count(&tag);
      if (count > nodes / 2 ||
          count > mpack_reader_remaining(&reader, nullptr) / 2) {
        return false;
      }
      if (out) {
        *out = json::object();
      }
      for (uint32_t i = 0; i < count; ++i) {
        json key;
        json value;
        if (!Read(reader, out ? &key : nullptr, depth - 1, nodes) ||
            (out && (!key.is_string() || out->lookup(key.get_string()))) ||
            !Read(reader, out ? &value : nullptr, depth - 1, nodes)) {
          return false;
        }
        if (out) {
          (*out)[key.get_string()] = std::move(value);
        }
      }
      mpack_done_map(&reader);
      break;
    }
    default:
      return false;
  }
  return mpack_reader_error(&reader) == mpack_ok;
}

bool Decode(std::string_view data, json* out) {
  mpack_reader_t reader;
  mpack_reader_init_data(&reader, data.data(), data.size());
  size_t nodes = MAX_NODES;
  bool valid = Read(reader, out, MAX_DEPTH, nodes) &&
               mpack_reader_remaining(&reader, nullptr) == 0;
  // Abandon any partially read containers before destroying the reader.
  if (!valid) {
    mpack_reader_flag_error(&reader, mpack_error_invalid);
  }
  return mpack_reader_destroy(&reader) == mpack_ok && valid;
}

void Write(mpack_writer_t& writer, const json& value, unsigned int depth,
           size_t& nodes) {
  if (depth == 0 || nodes == 0) {
    mpack_writer_flag_error(&writer, mpack_error_too_big);
    return;
  }
  --nodes;
  switch (value.type()) {
    case json::Type::Null:
      mpack_write_nil(&writer);
      break;
    case json::Type::Bool:
      mpack_write_bool(&writer, value.get_bool());
      break;
    case json::Type::Int:
      mpack_write_i64(&writer, value.get_int());
      break;
    case json::Type::Uint:
      mpack_write_u64(&writer, value.get_uint());
      break;
    case json::Type::Float:
      mpack_write_float(&writer, value.get_float());
      break;
    case json::Type::Double:
      mpack_write_double(&writer, value.get_double());
      break;
    case json::Type::String:
      mpack_write_str(&writer, value.get_string());
      break;
    case json::Type::Array:
      mpack_start_array(&writer, value.get_array().size());
      for (auto&& elem : value.get_array()) {
        Write(writer, elem, depth - 1, nodes);
      }
      mpack_finish_array(&writer);
      break;
    case json::Type::Object:
      mpack_start_map(&writer, value.get_object().size());
      for (auto&& [key, elem] : value.get_object()) {
        if (nodes == 0 || depth <= 1) {
          mpack_writer_flag_error(&writer, mpack_error_too_big);
          break;
        }
        --nodes;  // Map keys are MessagePack nodes too.
        mpack_write_str(&writer, key);
        Write(writer, elem, depth - 1, nodes);
      }
      mpack_finish_map(&writer);
      break;
  }
}
}  // namespace

std::expected<json, std::string_view> wpi::nt::server::RestDecodeMessagePack(
    std::string_view data) {
  json value;
  if (!Decode(data, &value)) {
    return std::unexpected(
        "MessagePack must contain one JSON-compatible value within codec "
        "limits");
  }
  return value;
}

bool wpi::nt::server::RestValidateMessagePack(std::string_view data) {
  return Decode(data, nullptr);
}

std::optional<std::string> wpi::nt::server::RestEncodeMessagePack(
    const json& value) {
  Writer writer;
  size_t nodes = MAX_NODES;
  Write(writer, value, MAX_DEPTH, nodes);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return std::nullopt;
  }
  return std::string{reinterpret_cast<const char*>(writer.bytes.data()),
                     writer.bytes.size()};
}
