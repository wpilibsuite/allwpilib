/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkTablesGui.h"

#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <vector>

#include <imgui.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <wpi/Format.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "HALSimGui.h"

using namespace halsimgui;

static void BooleanArrayToString(wpi::SmallVectorImpl<char>& out,
                                 wpi::ArrayRef<int> in) {
  out.clear();
  wpi::raw_svector_ostream os{out};
  os << '[';
  bool first = true;
  for (auto v : in) {
    if (!first) os << ',';
    first = false;
    if (v)
      os << "true";
    else
      os << "false";
  }
  os << ']';
}

static std::shared_ptr<nt::Value> StringToBooleanArray(wpi::StringRef in) {
  in = in.trim();
  if (in.empty())
    return nt::NetworkTableValue::MakeBooleanArray(
        std::initializer_list<bool>{});
  if (in.front() == '[') in = in.drop_front();
  if (in.back() == ']') in = in.drop_back();
  in = in.trim();

  wpi::SmallVector<wpi::StringRef, 16> inSplit;
  wpi::SmallVector<int, 16> out;

  in.split(inSplit, ',', -1, false);
  for (auto val : inSplit) {
    val = val.trim();
    if (val.equals_lower("true")) {
      out.emplace_back(1);
    } else if (val.equals_lower("false")) {
      out.emplace_back(0);
    } else {
      wpi::errs() << "GUI: NetworkTables: Could not understand value '" << val
                  << "'\n";
      return nullptr;
    }
  }

  return nt::NetworkTableValue::MakeBooleanArray(out);
}

static void DoubleArrayToString(wpi::SmallVectorImpl<char>& out,
                                wpi::ArrayRef<double> in) {
  out.clear();
  wpi::raw_svector_ostream os{out};
  os << '[';
  bool first = true;
  for (auto v : in) {
    if (!first) os << ',';
    first = false;
    os << wpi::format("%.6f", v);
  }
  os << ']';
}

static std::shared_ptr<nt::Value> StringToDoubleArray(wpi::StringRef in) {
  in = in.trim();
  if (in.empty())
    return nt::NetworkTableValue::MakeBooleanArray(
        std::initializer_list<bool>{});
  if (in.front() == '[') in = in.drop_front();
  if (in.back() == ']') in = in.drop_back();
  in = in.trim();

  wpi::SmallVector<wpi::StringRef, 16> inSplit;
  wpi::SmallVector<double, 16> out;

  in.split(inSplit, ',', -1, false);
  for (auto val : inSplit) {
    val = val.trim();
    wpi::SmallString<32> valStr = val;
    double d;
    if (std::sscanf(valStr.c_str(), "%lf", &d) == 1) {
      out.emplace_back(d);
    } else {
      wpi::errs() << "GUI: NetworkTables: Could not understand value '" << val
                  << "'\n";
      return nullptr;
    }
  }

  return nt::NetworkTableValue::MakeDoubleArray(out);
}

static void StringArrayToString(wpi::SmallVectorImpl<char>& out,
                                wpi::ArrayRef<std::string> in) {
  out.clear();
  wpi::raw_svector_ostream os{out};
  os << '[';
  bool first = true;
  for (auto&& v : in) {
    if (!first) os << ',';
    first = false;
    os << '"';
    os.write_escaped(v);
    os << '"';
  }
  os << ']';
}

static int fromxdigit(char ch) {
  if (ch >= 'a' && ch <= 'f')
    return (ch - 'a' + 10);
  else if (ch >= 'A' && ch <= 'F')
    return (ch - 'A' + 10);
  else
    return ch - '0';
}

static wpi::StringRef UnescapeString(wpi::StringRef source,
                                     wpi::SmallVectorImpl<char>& buf) {
  assert(source.size() >= 2 && source.front() == '"' && source.back() == '"');
  buf.clear();
  buf.reserve(source.size() - 2);
  for (auto s = source.begin() + 1, end = source.end() - 1; s != end; ++s) {
    if (*s != '\\') {
      buf.push_back(*s);
      continue;
    }
    switch (*++s) {
      case 't':
        buf.push_back('\t');
        break;
      case 'n':
        buf.push_back('\n');
        break;
      case 'x': {
        if (!isxdigit(*(s + 1))) {
          buf.push_back('x');  // treat it like a unknown escape
          break;
        }
        int ch = fromxdigit(*++s);
        if (std::isxdigit(*(s + 1))) {
          ch <<= 4;
          ch |= fromxdigit(*++s);
        }
        buf.push_back(static_cast<char>(ch));
        break;
      }
      default:
        buf.push_back(*s);
        break;
    }
  }
  return wpi::StringRef{buf.data(), buf.size()};
}

static std::shared_ptr<nt::Value> StringToStringArray(wpi::StringRef in) {
  in = in.trim();
  if (in.empty())
    return nt::NetworkTableValue::MakeStringArray(
        std::initializer_list<std::string>{});
  if (in.front() == '[') in = in.drop_front();
  if (in.back() == ']') in = in.drop_back();
  in = in.trim();

  wpi::SmallVector<wpi::StringRef, 16> inSplit;
  std::vector<std::string> out;
  wpi::SmallString<32> buf;

  in.split(inSplit, ',', -1, false);
  for (auto val : inSplit) {
    val = val.trim();
    if (val.empty()) continue;
    if (val.front() != '"' || val.back() != '"') {
      wpi::errs() << "GUI: NetworkTables: Could not understand value '" << val
                  << "'\n";
      return nullptr;
    }
    out.emplace_back(UnescapeString(val, buf));
  }

  return nt::NetworkTableValue::MakeStringArray(std::move(out));
}

static constexpr size_t kTextBufferSize = 4096;

static char* GetTextBuffer(wpi::StringRef in) {
  static char textBuffer[kTextBufferSize];
  size_t len = (std::min)(in.size(), kTextBufferSize - 1);
  std::memcpy(textBuffer, in.data(), len);
  textBuffer[len] = '\0';
  return textBuffer;
}

static void DisplayNetworkTables() {
  static auto inst = nt::NetworkTableInstance::GetDefault();

  if (ImGui::CollapsingHeader("Connections")) {
    ImGui::Columns(4, "connections");
    ImGui::Text("Id");
    ImGui::NextColumn();
    ImGui::Text("Address");
    ImGui::NextColumn();
    ImGui::Text("Updated");
    ImGui::NextColumn();
    ImGui::Text("Proto");
    ImGui::NextColumn();
    ImGui::Separator();
    for (auto&& i : inst.GetConnections()) {
      ImGui::Text("%s", i.remote_id.c_str());
      ImGui::NextColumn();
      ImGui::Text("%s", i.remote_ip.c_str());
      ImGui::NextColumn();
      ImGui::Text("%llu",
                  static_cast<unsigned long long>(  // NOLINT(runtime/int)
                      i.last_update));
      ImGui::NextColumn();
      ImGui::Text("%d.%d", i.protocol_version >> 8, i.protocol_version & 0xff);
      ImGui::NextColumn();
    }
    ImGui::Columns();
  }

  if (ImGui::CollapsingHeader("Values", ImGuiTreeNodeFlags_DefaultOpen)) {
    static bool first = true;
    ImGui::Columns(4, "values");
    if (first) ImGui::SetColumnWidth(-1, 0.5f * ImGui::GetWindowWidth());
    ImGui::Text("Name");
    ImGui::NextColumn();
    ImGui::Text("Value");
    ImGui::NextColumn();
    if (first) ImGui::SetColumnWidth(-1, 12 * ImGui::GetFontSize());
    ImGui::Text("Flags");
    ImGui::NextColumn();
    ImGui::Text("Changed");
    ImGui::NextColumn();
    ImGui::Separator();
    first = false;

    auto info = inst.GetEntryInfo("", 0);
    std::sort(info.begin(), info.end(),
              [](const auto& a, const auto& b) { return a.name < b.name; });

    for (auto&& i : info) {
      ImGui::Text("%s", i.name.c_str());
      ImGui::NextColumn();

      if (auto val = nt::GetEntryValue(i.entry)) {
        ImGui::PushID(i.name.c_str());
        switch (val->type()) {
          case NT_BOOLEAN: {
            static const char* boolOptions[] = {"false", "true"};
            int v = val->GetBoolean() ? 1 : 0;
            if (ImGui::Combo("boolean", &v, boolOptions, 2))
              nt::SetEntryValue(i.entry, nt::NetworkTableValue::MakeBoolean(v));
            break;
          }
          case NT_DOUBLE: {
            double v = val->GetDouble();
            if (ImGui::InputDouble("double", &v, 0, 0, "%.6f",
                                   ImGuiInputTextFlags_EnterReturnsTrue))
              nt::SetEntryValue(i.entry, nt::NetworkTableValue::MakeDouble(v));
            break;
          }
          case NT_STRING: {
            char* v = GetTextBuffer(val->GetString());
            if (ImGui::InputText("string", v, kTextBufferSize,
                                 ImGuiInputTextFlags_EnterReturnsTrue))
              nt::SetEntryValue(i.entry, nt::NetworkTableValue::MakeString(v));
            break;
          }
          case NT_BOOLEAN_ARRAY: {
            wpi::SmallString<64> buf;
            BooleanArrayToString(buf, val->GetBooleanArray());
            char* v = GetTextBuffer(buf);
            if (ImGui::InputText("boolean[]", v, kTextBufferSize,
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
              if (auto outv = StringToBooleanArray(v))
                nt::SetEntryValue(i.entry, std::move(outv));
            }
            break;
          }
          case NT_DOUBLE_ARRAY: {
            wpi::SmallString<64> buf;
            DoubleArrayToString(buf, val->GetDoubleArray());
            char* v = GetTextBuffer(buf);
            if (ImGui::InputText("double[]", v, kTextBufferSize,
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
              if (auto outv = StringToDoubleArray(v))
                nt::SetEntryValue(i.entry, std::move(outv));
            }
            break;
          }
          case NT_STRING_ARRAY: {
            wpi::SmallString<64> buf;
            StringArrayToString(buf, val->GetStringArray());
            char* v = GetTextBuffer(buf);
            if (ImGui::InputText("string[]", v, kTextBufferSize,
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
              if (auto outv = StringToStringArray(v))
                nt::SetEntryValue(i.entry, std::move(outv));
            }
            break;
          }
          case NT_RAW:
            ImGui::LabelText("raw", "[...]");
            break;
          case NT_RPC:
            ImGui::LabelText("rpc", "[...]");
            break;
          default:
            ImGui::LabelText("other", "?");
            break;
        }
        ImGui::PopID();
      }
      ImGui::NextColumn();

      if ((i.flags & NT_PERSISTENT) != 0)
        ImGui::Text("Persistent");
      else if (i.flags != 0)
        ImGui::Text("%02x", i.flags);
      ImGui::NextColumn();

      ImGui::Text("%llu",
                  static_cast<unsigned long long>(  // NOLINT(runtime/int)
                      i.last_change));
      ImGui::NextColumn();
      ImGui::Separator();
    }
    ImGui::Columns();
  }
}

void NetworkTablesGui::Initialize() {
  HALSimGui::AddWindow("NetworkTables", DisplayNetworkTables);
  HALSimGui::SetDefaultWindowPos("NetworkTables", 250, 277);
  HALSimGui::SetDefaultWindowSize("NetworkTables", 750, 185);
}
