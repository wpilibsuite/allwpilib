// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/Storage.hpp"

#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <imgui.h>

#include "wpi/util/StringExtras.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::glass;

template <typename To>
bool ConvertFromString(To* out, std::string_view str) {
  if constexpr (std::same_as<To, bool>) {
    if (str == "true") {
      *out = true;
    } else if (str == "false") {
      *out = false;
    } else if (auto val = wpi::util::parse_integer<int>(str, 10)) {
      *out = val.value() != 0;
    } else {
      return false;
    }
  } else if constexpr (std::floating_point<To>) {
    if (auto val = wpi::util::parse_float<To>(str)) {
      *out = val.value();
    } else {
      return false;
    }
  } else {
    if (auto val = wpi::util::parse_integer<To>(str, 10)) {
      *out = val.value();
    } else {
      return false;
    }
  }
  return true;
}

#define CONVERT(CapsName, LowerName, CType, EnumName)                       \
  static bool Convert##CapsName(Storage::Value* value) {                    \
    switch (value->type) {                                                  \
      case Storage::Value::BOOL:                                            \
        value->LowerName##Val = value->boolVal;                             \
        value->LowerName##Default = value->boolDefault;                     \
        break;                                                              \
      case Storage::Value::DOUBLE:                                          \
        value->LowerName##Val = value->doubleVal;                           \
        value->LowerName##Default = value->doubleDefault;                   \
        break;                                                              \
      case Storage::Value::FLOAT:                                           \
        value->LowerName##Val = value->floatVal;                            \
        value->LowerName##Default = value->floatDefault;                    \
        break;                                                              \
      case Storage::Value::INT:                                             \
        value->LowerName##Val = value->intVal;                              \
        value->LowerName##Default = value->intDefault;                      \
        break;                                                              \
      case Storage::Value::INT64:                                           \
        value->LowerName##Val = value->int64Val;                            \
        value->LowerName##Default = value->int64Default;                    \
        break;                                                              \
      case Storage::Value::STRING:                                          \
        if (!ConvertFromString(&value->LowerName##Val, value->stringVal)) { \
          return false;                                                     \
        }                                                                   \
        if (!ConvertFromString(&value->LowerName##Default,                  \
                               value->stringDefault)) {                     \
          return false;                                                     \
        }                                                                   \
        break;                                                              \
      default:                                                              \
        return false;                                                       \
    }                                                                       \
    value->type = Storage::Value::EnumName;                                 \
    return true;                                                            \
  }

CONVERT(Int, int, int, INT)
CONVERT(Int64, int64, int64_t, INT64)
CONVERT(Float, float, float, FLOAT)
CONVERT(Double, double, double, DOUBLE)
CONVERT(Bool, bool, bool, BOOL)

static inline bool ConvertString(Storage::Value* value) {
  return false;
}

// Arrays can only come from JSON, so we only have to worry about conversions
// between the various number types, not bool or string

template <typename From, typename To>
static void ConvertArray(std::vector<To>** outPtr, std::vector<From>** inPtr) {
  if (*inPtr) {
    if (*outPtr) {
      (*outPtr)->assign((*inPtr)->begin(), (*inPtr)->end());
    } else {
      std::vector<To>* tmp;
      tmp = new std::vector<To>{(*inPtr)->begin(), (*inPtr)->end()};
      delete *inPtr;
      *outPtr = tmp;
    }
  } else {
    *outPtr = nullptr;
  }
}

#define CONVERT_ARRAY(CapsName, LowerName, EnumName)                 \
  static bool Convert##CapsName##Array(Storage::Value* value) {      \
    switch (value->type) {                                           \
      case Storage::Value::DOUBLE_ARRAY:                             \
        ConvertArray(&value->LowerName##Array, &value->doubleArray); \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->doubleArrayDefault);                    \
        break;                                                       \
      case Storage::Value::FLOAT_ARRAY:                              \
        ConvertArray(&value->LowerName##Array, &value->floatArray);  \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->floatArrayDefault);                     \
        break;                                                       \
      case Storage::Value::INT_ARRAY:                                \
        ConvertArray(&value->LowerName##Array, &value->intArray);    \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->intArrayDefault);                       \
        break;                                                       \
      case Storage::Value::INT64ARRAY:                               \
        ConvertArray(&value->LowerName##Array, &value->int64Array);  \
        ConvertArray(&value->LowerName##ArrayDefault,                \
                     &value->int64ArrayDefault);                     \
        break;                                                       \
      default:                                                       \
        return false;                                                \
    }                                                                \
    value->type = Storage::Value::EnumName;                          \
    return true;                                                     \
  }

CONVERT_ARRAY(Int, int, INT_ARRAY)
CONVERT_ARRAY(Int64, int64, INT64ARRAY)
CONVERT_ARRAY(Float, float, FLOAT_ARRAY)
CONVERT_ARRAY(Double, double, DOUBLE_ARRAY)

static inline bool ConvertBoolArray(Storage::Value* value) {
  return false;
}

static inline bool ConvertStringArray(Storage::Value* value) {
  return false;
}

void Storage::Value::Reset(Type newType) {
  switch (type) {
    case CHILD:
      delete child;
      break;
    case INT_ARRAY:
      delete intArray;
      delete intArrayDefault;
      break;
    case INT64ARRAY:
      delete int64Array;
      delete int64ArrayDefault;
      break;
    case BOOL_ARRAY:
      delete boolArray;
      delete boolArrayDefault;
      break;
    case FLOAT_ARRAY:
      delete floatArray;
      delete floatArrayDefault;
      break;
    case DOUBLE_ARRAY:
      delete doubleArray;
      delete doubleArrayDefault;
      break;
    case STRING_ARRAY:
      delete stringArray;
      delete stringArrayDefault;
      break;
    case CHILD_ARRAY:
      delete childArray;
      break;
    default:
      break;
  }
  type = newType;
}

Storage::Value* Storage::FindValue(std::string_view key) {
  auto it = m_values.find(key);
  if (it == m_values.end()) {
    return nullptr;
  }
  return it->second.get();
}

Storage::Value& Storage::GetValue(std::string_view key) {
  auto& val = m_values[key];
  if (!val) {
    val = std::make_unique<Value>();
  }
  return *val;
}

#define DEFUN(CapsName, LowerName, EnumName, EnumArrayName, CType, CParamType, \
              ArrCType)                                                        \
  CType Storage::Read##CapsName(std::string_view key, CParamType defaultVal)   \
      const {                                                                  \
    auto it = m_values.find(key);                                              \
    if (it == m_values.end()) {                                                \
      return CType{defaultVal};                                                \
    }                                                                          \
    Value& value = *it->second;                                                \
    if (value.type != Value::EnumName) {                                       \
      if (!Convert##CapsName(&value)) {                                        \
        value.Reset(Value::EnumName);                                          \
        value.LowerName##Val = defaultVal;                                     \
        value.LowerName##Default = defaultVal;                                 \
        value.hasDefault = true;                                               \
      }                                                                        \
    }                                                                          \
    return value.LowerName##Val;                                               \
  }                                                                            \
                                                                               \
  void Storage::Set##CapsName(std::string_view key, CParamType val) {          \
    auto& valuePtr = m_values[key];                                            \
    if (!valuePtr) {                                                           \
      valuePtr = std::make_unique<Value>(Value::EnumName);                     \
    } else {                                                                   \
      valuePtr->Reset(Value::EnumName);                                        \
    }                                                                          \
    valuePtr->LowerName##Val = val;                                            \
    valuePtr->LowerName##Default = {};                                         \
  }                                                                            \
                                                                               \
  CType& Storage::Get##CapsName(std::string_view key, CParamType defaultVal) { \
    auto& valuePtr = m_values[key];                                            \
    bool setValue = false;                                                     \
    if (!valuePtr) {                                                           \
      valuePtr = std::make_unique<Value>(Value::EnumName);                     \
      setValue = true;                                                         \
    } else if (valuePtr->type != Value::EnumName) {                            \
      if (!Convert##CapsName(valuePtr.get())) {                                \
        valuePtr->Reset(Value::EnumName);                                      \
        setValue = true;                                                       \
      }                                                                        \
    }                                                                          \
    if (setValue) {                                                            \
      valuePtr->LowerName##Val = defaultVal;                                   \
    }                                                                          \
    if (!valuePtr->hasDefault) {                                               \
      valuePtr->LowerName##Default = defaultVal;                               \
      valuePtr->hasDefault = true;                                             \
    }                                                                          \
    return valuePtr->LowerName##Val;                                           \
  }                                                                            \
                                                                               \
  std::vector<ArrCType>& Storage::Get##CapsName##Array(                        \
      std::string_view key, std::span<const ArrCType> defaultVal) {            \
    auto& valuePtr = m_values[key];                                            \
    bool setValue = false;                                                     \
    if (!valuePtr) {                                                           \
      valuePtr = std::make_unique<Value>(Value::EnumArrayName);                \
      setValue = true;                                                         \
    } else if (valuePtr->type != Value::EnumArrayName) {                       \
      if (!Convert##CapsName##Array(valuePtr.get())) {                         \
        valuePtr->Reset(Value::EnumArrayName);                                 \
        setValue = true;                                                       \
      }                                                                        \
    }                                                                          \
    if (setValue) {                                                            \
      valuePtr->LowerName##Array =                                             \
          new std::vector<ArrCType>{defaultVal.begin(), defaultVal.end()};     \
    }                                                                          \
    if (!valuePtr->hasDefault) {                                               \
      if (defaultVal.empty()) {                                                \
        valuePtr->LowerName##ArrayDefault = nullptr;                           \
      } else {                                                                 \
        valuePtr->LowerName##ArrayDefault =                                    \
            new std::vector<ArrCType>{defaultVal.begin(), defaultVal.end()};   \
      }                                                                        \
      valuePtr->hasDefault = true;                                             \
    }                                                                          \
    assert(valuePtr->LowerName##Array);                                        \
    return *valuePtr->LowerName##Array;                                        \
  }

DEFUN(Int, int, INT, INT_ARRAY, int, int, int)
DEFUN(Int64, int64, INT64, INT64ARRAY, int64_t, int64_t, int64_t)
DEFUN(Bool, bool, BOOL, BOOL_ARRAY, bool, bool, int)
DEFUN(Float, float, FLOAT, FLOAT_ARRAY, float, float, float)
DEFUN(Double, double, DOUBLE, DOUBLE_ARRAY, double, double, double)
DEFUN(String, string, STRING, STRING_ARRAY, std::string, std::string_view,
      std::string)

Storage& Storage::GetChild(std::string_view label_id) {
  auto [label, id] = wpi::util::split(label_id, "###");
  if (id.empty()) {
    id = label;
  }
  auto& childPtr = m_values[id];
  if (!childPtr) {
    childPtr = std::make_unique<Value>();
  }
  if (childPtr->type != Value::CHILD) {
    childPtr->Reset(Value::CHILD);
    childPtr->child = new Storage;
  }
  return *childPtr->child;
}

std::vector<std::unique_ptr<Storage>>& Storage::GetChildArray(
    std::string_view key) {
  auto& valuePtr = m_values[key];
  if (!valuePtr) {
    valuePtr = std::make_unique<Value>(Value::CHILD_ARRAY);
    valuePtr->childArray = new std::vector<std::unique_ptr<Storage>>();
  } else if (valuePtr->type != Value::CHILD_ARRAY) {
    valuePtr->Reset(Value::CHILD_ARRAY);
    valuePtr->childArray = new std::vector<std::unique_ptr<Storage>>();
  }

  return *valuePtr->childArray;
}

std::unique_ptr<Storage::Value> Storage::Erase(std::string_view key) {
  auto it = m_values.find(key);
  if (it != m_values.end()) {
    auto rv = std::move(it->second);
    m_values.erase(it);
    return rv;
  }
  return nullptr;
}

void Storage::EraseChildren() {
  std::erase_if(m_values,
                [](const auto& kv) { return kv.second->type == Value::CHILD; });
}

static bool JsonArrayToStorage(Storage::Value* valuePtr,
                               const wpi::util::json& jarr,
                               const char* filename) {
  auto& arr = jarr.get_array();
  if (arr.empty()) {
    ImGui::LogText("empty array in %s, ignoring", filename);
    return false;
  }

  // guess array type from first element
  switch (arr[0].type()) {
    case wpi::util::json::Type::Bool:
      if (valuePtr->type != Storage::Value::BOOL_ARRAY) {
        valuePtr->Reset(Storage::Value::BOOL_ARRAY);
        valuePtr->boolArray = new std::vector<int>();
        valuePtr->boolArrayDefault = nullptr;
      }
      break;
    case wpi::util::json::Type::Float:
    case wpi::util::json::Type::Double:
      if (valuePtr->type != Storage::Value::DOUBLE_ARRAY) {
        valuePtr->Reset(Storage::Value::DOUBLE_ARRAY);
        valuePtr->doubleArray = new std::vector<double>();
        valuePtr->doubleArrayDefault = nullptr;
      }
      break;
    case wpi::util::json::Type::Int:
      if (valuePtr->type != Storage::Value::INT64ARRAY) {
        valuePtr->Reset(Storage::Value::INT64ARRAY);
        valuePtr->int64Array = new std::vector<int64_t>();
        valuePtr->int64ArrayDefault = nullptr;
      }
      break;
    case wpi::util::json::Type::Uint:
      ImGui::LogText("too large of integer in %s, ignoring", filename);
      return false;
    case wpi::util::json::Type::String:
      if (valuePtr->type != Storage::Value::STRING_ARRAY) {
        valuePtr->Reset(Storage::Value::STRING_ARRAY);
        valuePtr->stringArray = new std::vector<std::string>();
        valuePtr->stringArrayDefault = nullptr;
      }
      break;
    case wpi::util::json::Type::Object:
      if (valuePtr->type != Storage::Value::CHILD_ARRAY) {
        valuePtr->Reset(Storage::Value::CHILD_ARRAY);
        valuePtr->childArray = new std::vector<std::unique_ptr<Storage>>();
      }
      break;
    case wpi::util::json::Type::Array:
      ImGui::LogText("nested array in %s, ignoring", filename);
      return false;
    default:
      ImGui::LogText("null value in %s, ignoring", filename);
      return false;
  }

  // loop over array to store elements
  for (auto jvalue : arr) {
    switch (jvalue.type()) {
      case wpi::util::json::Type::Bool:
        if (valuePtr->type == Storage::Value::BOOL_ARRAY) {
          valuePtr->boolArray->push_back(jvalue.get_bool());
        } else {
          goto error;
        }
        break;
      case wpi::util::json::Type::Float:
        if (valuePtr->type == Storage::Value::DOUBLE_ARRAY) {
          valuePtr->doubleArray->push_back(jvalue.get_float());
        } else {
          goto error;
        }
        break;
      case wpi::util::json::Type::Double:
        if (valuePtr->type == Storage::Value::DOUBLE_ARRAY) {
          valuePtr->doubleArray->push_back(jvalue.get_double());
        } else {
          goto error;
        }
        break;
      case wpi::util::json::Type::Int:
        if (valuePtr->type == Storage::Value::INT64ARRAY) {
          valuePtr->int64Array->push_back(jvalue.get_int());
        } else if (valuePtr->type == Storage::Value::DOUBLE_ARRAY) {
          valuePtr->doubleArray->push_back(jvalue.get_int());
        } else {
          goto error;
        }
        break;
      case wpi::util::json::Type::Uint:
        ImGui::LogText("too large of integer in %s, ignoring", filename);
        return false;
      case wpi::util::json::Type::String:
        if (valuePtr->type == Storage::Value::STRING_ARRAY) {
          valuePtr->stringArray->emplace_back(jvalue.get_string());
        } else {
          goto error;
        }
        break;
      case wpi::util::json::Type::Object:
        if (valuePtr->type == Storage::Value::CHILD_ARRAY) {
          valuePtr->childArray->emplace_back(std::make_unique<Storage>());
          valuePtr->childArray->back()->FromJson(jvalue, filename);
        } else {
          goto error;
        }
        break;
      case wpi::util::json::Type::Array:
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

bool Storage::FromJson(const wpi::util::json& json, const char* filename) {
  if (m_fromJson) {
    return m_fromJson(json, filename);
  }

  if (!json.is_object()) {
    ImGui::LogText("non-object in %s", filename);
    return false;
  }
  for (auto&& [key, jvalue] : json.get_object()) {
    auto& valuePtr = m_values[key];
    bool created = false;
    if (!valuePtr) {
      valuePtr = std::make_unique<Value>();
      created = true;
    }
    switch (jvalue.type()) {
      case wpi::util::json::Type::Bool:
        valuePtr->Reset(Value::BOOL);
        valuePtr->boolVal = jvalue.get_bool();
        break;
      case wpi::util::json::Type::Float:
        valuePtr->Reset(Value::DOUBLE);
        valuePtr->doubleVal = jvalue.get_float();
        break;
      case wpi::util::json::Type::Double:
        valuePtr->Reset(Value::DOUBLE);
        valuePtr->doubleVal = jvalue.get_double();
        break;
      case wpi::util::json::Type::Int:
        valuePtr->Reset(Value::INT64);
        valuePtr->int64Val = jvalue.get_int();
        break;
      case wpi::util::json::Type::String:
        valuePtr->Reset(Value::STRING);
        valuePtr->stringVal = jvalue.get_string();
        break;
      case wpi::util::json::Type::Object:
        if (valuePtr->type != Value::CHILD) {
          valuePtr->Reset(Value::CHILD);
          valuePtr->child = new Storage;
        }
        valuePtr->child->FromJson(jvalue, filename);  // recurse
        break;
      case wpi::util::json::Type::Array:
        if (!JsonArrayToStorage(valuePtr.get(), jvalue, filename)) {
          if (created) {
            m_values.erase(key);
          }
        }
        break;
      default:
        ImGui::LogText("null value in %s, ignoring", filename);
        if (created) {
          m_values.erase(key);
        }
        break;
    }
  }
  return true;
}

template <typename T>
static wpi::util::json StorageToJsonArray(const std::vector<T>& arr) {
  wpi::util::json jarr = wpi::util::json::array();
  for (auto&& v : arr) {
    jarr.emplace_back(v);
  }
  return jarr;
}

template <>
wpi::util::json StorageToJsonArray<std::unique_ptr<Storage>>(
    const std::vector<std::unique_ptr<Storage>>& arr) {
  wpi::util::json jarr = wpi::util::json::array();
  for (auto&& v : arr) {
    jarr.emplace_back(v->ToJson());
  }
  // remove any trailing empty items
  auto& jarrArr = jarr.get_array();
  while (!jarrArr.empty() && jarrArr.back().empty()) {
    jarrArr.pop_back();
  }
  return jarr;
}

wpi::util::json Storage::ToJson() const {
  if (m_toJson) {
    return m_toJson();
  }

  wpi::util::json j = wpi::util::json::object();
  for (auto&& kv : m_values) {
    wpi::util::json jelem;
    auto& value = *kv.second;
    switch (value.type) {
#define CASE(CapsName, LowerName, EnumName, EnumArrayName)               \
  case Value::EnumName:                                                  \
    if (value.hasDefault &&                                              \
        value.LowerName##Val == value.LowerName##Default) {              \
      continue;                                                          \
    }                                                                    \
    jelem = value.LowerName##Val;                                        \
    break;                                                               \
  case Value::EnumArrayName:                                             \
    if (value.hasDefault &&                                              \
        ((!value.LowerName##ArrayDefault &&                              \
          value.LowerName##Array->empty()) ||                            \
         (value.LowerName##ArrayDefault &&                               \
          *value.LowerName##Array == *value.LowerName##ArrayDefault))) { \
      continue;                                                          \
    }                                                                    \
    jelem = StorageToJsonArray(*value.LowerName##Array);                 \
    break;

      CASE(Int, int, INT, INT_ARRAY)
      CASE(Int64, int64, INT64, INT64ARRAY)
      CASE(Bool, bool, BOOL, BOOL_ARRAY)
      CASE(Float, float, FLOAT, FLOAT_ARRAY)
      CASE(Double, double, DOUBLE, DOUBLE_ARRAY)
      CASE(String, string, STRING, STRING_ARRAY)

      case Value::CHILD:
        jelem = value.child->ToJson();  // recurse
        if (jelem.empty()) {
          continue;
        }
        break;
      case Value::CHILD_ARRAY:
        jelem = StorageToJsonArray(*value.childArray);
        if (jelem.empty()) {
          continue;
        }
        break;
      default:
        continue;
    }
    j[kv.first] = std::move(jelem);
  }
  return j;
}

void Storage::Clear() {
  if (m_clear) {
    return m_clear();
  }

  ClearValues();
}

void Storage::ClearValues() {
  for (auto&& kv : m_values) {
    auto& value = *kv.second;
    switch (value.type) {
      case Value::INT:
        value.intVal = value.intDefault;
        break;
      case Value::INT64:
        value.int64Val = value.int64Default;
        break;
      case Value::BOOL:
        value.boolVal = value.boolDefault;
        break;
      case Value::FLOAT:
        value.floatVal = value.floatDefault;
        break;
      case Value::DOUBLE:
        value.doubleVal = value.doubleDefault;
        break;
      case Value::STRING:
        value.stringVal = value.stringDefault;
        break;
      case Value::INT_ARRAY:
        if (value.intArrayDefault) {
          *value.intArray = *value.intArrayDefault;
        } else {
          value.intArray->clear();
        }
        break;
      case Value::INT64ARRAY:
        if (value.int64ArrayDefault) {
          *value.int64Array = *value.int64ArrayDefault;
        } else {
          value.int64Array->clear();
        }
        break;
      case Value::BOOL_ARRAY:
        if (value.boolArrayDefault) {
          *value.boolArray = *value.boolArrayDefault;
        } else {
          value.boolArray->clear();
        }
        break;
      case Value::FLOAT_ARRAY:
        if (value.floatArrayDefault) {
          *value.floatArray = *value.floatArrayDefault;
        } else {
          value.floatArray->clear();
        }
        break;
      case Value::DOUBLE_ARRAY:
        if (value.doubleArrayDefault) {
          *value.doubleArray = *value.doubleArrayDefault;
        } else {
          value.doubleArray->clear();
        }
        break;
      case Value::STRING_ARRAY:
        if (value.stringArrayDefault) {
          *value.stringArray = *value.stringArrayDefault;
        } else {
          value.stringArray->clear();
        }
        break;
      case Value::CHILD:
        value.child->Clear();
        break;
      case Value::CHILD_ARRAY:
        for (auto&& child : *value.childArray) {
          child->Clear();
        }
        break;
      default:
        break;
    }
  }
}

void Storage::Apply() {
  if (m_apply) {
    return m_apply();
  }

  ApplyChildren();
}

void Storage::ApplyChildren() {
  for (auto&& kv : m_values) {
    auto& value = *kv.second;
    switch (value.type) {
      case Value::CHILD:
        value.child->Apply();
        break;
      case Value::CHILD_ARRAY:
        for (auto&& child : *value.childArray) {
          child->Apply();
        }
        break;
      default:
        break;
    }
  }
}
