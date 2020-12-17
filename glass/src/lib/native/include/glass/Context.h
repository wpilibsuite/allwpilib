/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <imgui.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

namespace glass {

struct Context;

Context* CreateContext();
void DestroyContext(Context* ctx = nullptr);
Context* GetCurrentContext();
void SetCurrentContext(Context* ctx);

/**
 * Resets zero time to current time.
 */
void ResetTime();

/**
 * Gets the zero time.
 */
uint64_t GetZeroTime();

/**
 * Storage provides both persistent and non-persistent key/value storage for
 * widgets.
 *
 * Keys are always strings.  The storage also provides non-persistent arbitrary
 * data storage (via std::shared_ptr<void>).
 *
 * Storage is automatically indexed internally by the ID stack.  Note it is
 * necessary to use the glass wrappers for PushID et al to preserve naming in
 * the save file (unnamed values are still stored, but this is non-ideal for
 * users trying to hand-edit the save file).
 */
class Storage {
 public:
  struct Value {
    Value() = default;
    explicit Value(const wpi::Twine& str) : stringVal{str.str()} {}

    enum Type { kNone, kInt, kInt64, kBool, kFloat, kDouble, kString };
    Type type = kNone;
    union {
      int intVal;
      int64_t int64Val;
      bool boolVal;
      float floatVal;
      double doubleVal;
    };
    std::string stringVal;
  };

  int GetInt(wpi::StringRef key, int defaultVal = 0) const;
  int64_t GetInt64(wpi::StringRef key, int64_t defaultVal = 0) const;
  bool GetBool(wpi::StringRef key, bool defaultVal = false) const;
  float GetFloat(wpi::StringRef key, float defaultVal = 0.0f) const;
  double GetDouble(wpi::StringRef key, double defaultVal = 0.0) const;
  std::string GetString(wpi::StringRef key,
                        const std::string& defaultVal = {}) const;

  void SetInt(wpi::StringRef key, int val);
  void SetInt64(wpi::StringRef key, int64_t val);
  void SetBool(wpi::StringRef key, bool val);
  void SetFloat(wpi::StringRef key, float val);
  void SetDouble(wpi::StringRef key, double val);
  void SetString(wpi::StringRef key, const wpi::Twine& val);

  int* GetIntRef(wpi::StringRef key, int defaultVal = 0);
  int64_t* GetInt64Ref(wpi::StringRef key, int64_t defaultVal = 0);
  bool* GetBoolRef(wpi::StringRef key, bool defaultVal = false);
  float* GetFloatRef(wpi::StringRef key, float defaultVal = 0.0f);
  double* GetDoubleRef(wpi::StringRef key, double defaultVal = 0.0);
  std::string* GetStringRef(wpi::StringRef key, wpi::StringRef defaultVal = {});

  Value& GetValue(wpi::StringRef key);

  void SetData(std::shared_ptr<void>&& data) { m_data = std::move(data); }

  template <typename T>
  T* GetData() const {
    return static_cast<T*>(m_data.get());
  }

  Storage() = default;
  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  std::vector<std::string>& GetKeys() { return m_keys; }
  const std::vector<std::string>& GetKeys() const { return m_keys; }
  std::vector<std::unique_ptr<Value>>& GetValues() { return m_values; }
  const std::vector<std::unique_ptr<Value>>& GetValues() const {
    return m_values;
  }

 private:
  mutable std::vector<std::string> m_keys;
  mutable std::vector<std::unique_ptr<Value>> m_values;
  std::shared_ptr<void> m_data;
};

Storage& GetStorage();
Storage& GetStorage(wpi::StringRef id);

bool Begin(const char* name, bool* p_open = nullptr,
           ImGuiWindowFlags flags = 0);

void End();

bool BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0),
                bool border = false, ImGuiWindowFlags flags = 0);

void EndChild();

/**
 * Saves open status to storage "open" key.
 * If returning 'true' the header is open. doesn't indent nor push on ID stack.
 * user doesn't have to call TreePop().
 */
bool CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0);

bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags = 0);

void TreePop();

// push string into the ID stack (will hash string).
void PushID(const char* str_id);

// push string into the ID stack (will hash string).
void PushID(const char* str_id_begin, const char* str_id_end);

// push string into the ID stack (will hash string).
inline void PushID(wpi::StringRef str) { PushID(str.begin(), str.end()); }

// push integer into the ID stack (will hash integer).
void PushID(int int_id);

// pop from the ID stack.
void PopID();

bool PopupEditName(const char* label, std::string* name);

}  // namespace glass
