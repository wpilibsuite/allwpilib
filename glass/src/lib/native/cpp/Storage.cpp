// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/Storage.h"

#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <imgui.h>
#include <wpi/StringExtras.h>
#include <wpi/json.h>

using namespace glass;

Storage& Storage::GetChild(std::string_view label_id) {
  auto [label, id] = wpi::split(label_id, "###");
  if (id.empty()) {
    id = label;
  }
  Value& value = GetValue(id);
  if (auto data = std::get_if<Child>(&value.data)) {
    return **data;
  }
  return *value.data.emplace<Child>(std::make_shared<Storage>());
}

void Storage::EraseChildren() {
  std::erase_if(m_values, [](const auto& kv) {
    return std::holds_alternative<Child>(kv.second.data);
  });
}

static bool JsonArrayToStorage(Storage::Value* valuePtr, const wpi::json& jarr,
                               const char* filename) {
  auto& arr = jarr.get_ref<const wpi::json::array_t&>();
  if (arr.empty()) {
    ImGui::LogText("empty array in %s, ignoring", filename);
    return false;
  }

  // guess array type from first element
  switch (arr[0].type()) {
    case wpi::json::value_t::boolean:
      ImGui::LogText("boolean array in %s, ignoring", filename);
      return false;
    case wpi::json::value_t::number_float:
      valuePtr->data.emplace<std::vector<double>>();
      valuePtr->dataDefault = {};
      break;
    case wpi::json::value_t::number_integer:
    case wpi::json::value_t::number_unsigned:
      valuePtr->data.emplace<std::vector<int>>();
      valuePtr->dataDefault = {};
      break;
    case wpi::json::value_t::string:
      valuePtr->data.emplace<std::vector<std::string>>();
      valuePtr->dataDefault = {};
      break;
    case wpi::json::value_t::object:
      valuePtr->data.emplace<std::vector<Storage::Child>>();
      valuePtr->dataDefault = {};
      break;
    case wpi::json::value_t::array:
      ImGui::LogText("nested array in %s, ignoring", filename);
      return false;
    default:
      ImGui::LogText("null value in %s, ignoring", filename);
      return false;
  }

  // loop over array to store elements
  for (auto jvalue : arr) {
    switch (jvalue.type()) {
      case wpi::json::value_t::boolean:
        ImGui::LogText("boolean array value in %s, ignoring", filename);
        return false;
      case wpi::json::value_t::number_float:
        if (auto data = std::get_if<std::vector<double>>(&valuePtr->data)) {
          data->emplace_back(jvalue.get<double>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_integer:
        if (auto data = std::get_if<std::vector<int>>(&valuePtr->data)) {
          data->emplace_back(jvalue.get<int64_t>());
        } else if (auto data =
                       std::get_if<std::vector<double>>(&valuePtr->data)) {
          data->emplace_back(jvalue.get<int64_t>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::number_unsigned:
        if (auto data = std::get_if<std::vector<int>>(&valuePtr->data)) {
          data->emplace_back(jvalue.get<uint64_t>());
        } else if (auto data =
                       std::get_if<std::vector<double>>(&valuePtr->data)) {
          data->emplace_back(jvalue.get<uint64_t>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::string:
        if (auto data =
                std::get_if<std::vector<std::string>>(&valuePtr->data)) {
          data->emplace_back(jvalue.get_ref<const std::string&>());
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::object:
        if (auto data =
                std::get_if<std::vector<Storage::Child>>(&valuePtr->data)) {
          data->emplace_back(std::make_shared<Storage>())
              ->FromJson(jvalue, filename);
        } else {
          goto error;
        }
        break;
      case wpi::json::value_t::array:
        ImGui::LogText("nested array in %s, ignoring", filename);
        return false;
      default:
        ImGui::LogText("null value in %s, ignoring", filename);
        return false;
    }
  }
  return true;

error:
  ImGui::LogText("array with variant types in %s, ignoring", filename);
  return false;
}

bool Storage::FromJson(const wpi::json& json, const char* filename) {
  if (m_fromJson) {
    return m_fromJson(json, filename);
  }

  if (!json.is_object()) {
    ImGui::LogText("non-object in %s", filename);
    return false;
  }
  for (auto&& jkv : json.items()) {
    auto [it, created] = m_values.try_emplace(jkv.key());
    auto& jvalue = jkv.value();
    switch (jvalue.type()) {
      case wpi::json::value_t::boolean:
        it->second.data = jvalue.get<bool>();
        break;
      case wpi::json::value_t::number_float:
        it->second.data = jvalue.get<double>();
        break;
      case wpi::json::value_t::number_integer:
        it->second.data = static_cast<int>(jvalue.get<int64_t>());
        break;
      case wpi::json::value_t::number_unsigned:
        it->second.data = static_cast<int>(jvalue.get<uint64_t>());
        break;
      case wpi::json::value_t::string:
        it->second.data = jvalue.get_ref<const std::string&>();
        break;
      case wpi::json::value_t::object:
        if (auto d = std::get_if<Child>(&it->second.data)) {
          (*d)->FromJson(jvalue, filename);  // recurse
        } else {
          it->second.data.emplace<Child>(std::make_shared<Storage>())
              ->FromJson(jvalue, filename);  // recurse
        }
        break;
      case wpi::json::value_t::array:
        if (!JsonArrayToStorage(&it->second, jvalue, filename)) {
          if (created) {
            m_values.erase(it);
          }
        }
        break;
      default:
        ImGui::LogText("null value in %s, ignoring", filename);
        if (created) {
          m_values.erase(it);
        }
        break;
    }
  }
  return true;
}

wpi::json Storage::ToJson() const {
  if (m_toJson) {
    return m_toJson();
  }

  wpi::json j = wpi::json::object();
  for (auto&& kv : m_values) {
    auto jelem = std::visit(
        [&](auto&& arg) -> wpi::json {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::same_as<T, Child>) {
            return arg->ToJson();  // recurse
          } else if constexpr (std::same_as<T, std::vector<Child>>) {
            wpi::json jarr = wpi::json::array();
            for (auto&& v : arg) {
              jarr.emplace_back(v->ToJson());
            }
            // remove any trailing empty items
            while (!jarr.empty() && jarr.back().empty()) {
              jarr.get_ref<wpi::json::array_t&>().pop_back();
            }
            return jarr;
          } else if constexpr (std::same_as<T, std::monostate>) {
            return {};
          } else if (auto d = std::get_if<T>(&kv.second.dataDefault);
                     d && *d == arg) {
            return {};
          } else {
            return arg;
          }
        },
        kv.second.data);
    if (!jelem.empty()) {
      j.emplace(kv.first, std::move(jelem));
    }
  }
  return j;
}

void Storage::ClearValues() {
  for (auto&& kv : m_values) {
    Value& value = kv.second;
    if (auto d = std::get_if<Child>(&value.data)) {
      (*d)->Clear();
    } else if (auto d = std::get_if<std::vector<Child>>(&value.data)) {
      for (auto&& child : *d) {
        child->Clear();
      }
    } else {
      value.data = value.dataDefault;
    }
  }
}

void Storage::ApplyChildren() {
  for (auto&& kv : m_values) {
    Value& value = kv.second;
    if (auto d = std::get_if<Child>(&value.data)) {
      (*d)->Apply();
    } else if (auto d = std::get_if<std::vector<Child>>(&value.data)) {
      for (auto&& child : *d) {
        child->Apply();
      }
    }
  }
}
