// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NetworkTables.h"

#include <networktables/NetworkTableValue.h>

#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <imgui.h>
#include <ntcore_cpp.h>
#include <wpi/SmallString.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/raw_ostream.h>
#include <wpi/span.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

static std::string BooleanArrayToString(wpi::span<const int> in) {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  os << '[';
  bool first = true;
  for (auto v : in) {
    if (!first) {
      os << ',';
    }
    first = false;
    if (v) {
      os << "true";
    } else {
      os << "false";
    }
  }
  os << ']';
  return rv;
}

static std::string DoubleArrayToString(wpi::span<const double> in) {
  return fmt::format("[{:.6f}]", fmt::join(in, ","));
}

static std::string StringArrayToString(wpi::span<const std::string> in) {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  os << '[';
  bool first = true;
  for (auto&& v : in) {
    if (!first) {
      os << ',';
    }
    first = false;
    os << '"';
    os.write_escaped(v);
    os << '"';
  }
  os << ']';
  return rv;
}

NetworkTablesModel::NetworkTablesModel()
    : NetworkTablesModel{nt::GetDefaultInstance()} {}

NetworkTablesModel::NetworkTablesModel(NT_Inst inst)
    : m_inst{inst}, m_poller{nt::CreateEntryListenerPoller(inst)} {
  nt::AddPolledEntryListener(m_poller, "",
                             NT_NOTIFY_LOCAL | NT_NOTIFY_NEW |
                                 NT_NOTIFY_UPDATE | NT_NOTIFY_DELETE |
                                 NT_NOTIFY_FLAGS | NT_NOTIFY_IMMEDIATE);
}

NetworkTablesModel::~NetworkTablesModel() {
  nt::DestroyEntryListenerPoller(m_poller);
}

NetworkTablesModel::Entry::Entry(nt::EntryNotification&& event)
    : entry{event.entry},
      name{std::move(event.name)},
      value{std::move(event.value)},
      flags{nt::GetEntryFlags(event.entry)} {
  UpdateValue();
}

void NetworkTablesModel::Entry::UpdateValue() {
  switch (value->type()) {
    case NT_BOOLEAN:
      if (!source) {
        source = std::make_unique<DataSource>(fmt::format("NT:{}", name));
      }
      source->SetValue(value->GetBoolean() ? 1 : 0);
      source->SetDigital(true);
      break;
    case NT_DOUBLE:
      if (!source) {
        source = std::make_unique<DataSource>(fmt::format("NT:{}", name));
      }
      source->SetValue(value->GetDouble());
      source->SetDigital(false);
      break;
    case NT_BOOLEAN_ARRAY:
      valueStr = BooleanArrayToString(value->GetBooleanArray());
      break;
    case NT_DOUBLE_ARRAY:
      valueStr = DoubleArrayToString(value->GetDoubleArray());
      break;
    case NT_STRING_ARRAY:
      valueStr = StringArrayToString(value->GetStringArray());
      break;
    default:
      break;
  }
}

void NetworkTablesModel::Update() {
  bool timedOut = false;
  bool updateTree = false;
  for (auto&& event : nt::PollEntryListener(m_poller, 0, &timedOut)) {
    auto& entry = m_entries[event.entry];
    if (event.flags & NT_NOTIFY_NEW) {
      if (!entry) {
        entry = std::make_unique<Entry>(std::move(event));
        m_sortedEntries.emplace_back(entry.get());
        updateTree = true;
        continue;
      }
    }
    if (!entry) {
      continue;
    }
    if (event.flags & NT_NOTIFY_DELETE) {
      auto it = std::find(m_sortedEntries.begin(), m_sortedEntries.end(),
                          entry.get());
      // will be removed completely below
      if (it != m_sortedEntries.end()) {
        *it = nullptr;
      }
      m_entries.erase(event.entry);
      updateTree = true;
      continue;
    }
    if (event.flags & NT_NOTIFY_UPDATE) {
      entry->value = std::move(event.value);
      entry->UpdateValue();
    }
    if (event.flags & NT_NOTIFY_FLAGS) {
      entry->flags = nt::GetEntryFlags(event.entry);
    }
  }

  // shortcut common case (updates)
  if (!updateTree) {
    return;
  }

  // remove deleted entries
  m_sortedEntries.erase(
      std::remove(m_sortedEntries.begin(), m_sortedEntries.end(), nullptr),
      m_sortedEntries.end());

  // sort by name
  std::sort(m_sortedEntries.begin(), m_sortedEntries.end(),
            [](const auto& a, const auto& b) { return a->name < b->name; });

  // rebuild tree
  m_root.clear();
  wpi::SmallVector<std::string_view, 16> parts;
  for (auto& entry : m_sortedEntries) {
    parts.clear();
    wpi::split(entry->name, parts, '/', -1, false);

    // ignore a raw "/" key
    if (parts.empty()) {
      continue;
    }

    // get to leaf
    auto nodes = &m_root;
    for (auto part : wpi::drop_back(wpi::span{parts.begin(), parts.end()})) {
      auto it =
          std::find_if(nodes->begin(), nodes->end(),
                       [&](const auto& node) { return node.name == part; });
      if (it == nodes->end()) {
        nodes->emplace_back(part);
        // path is from the beginning of the string to the end of the current
        // part; this works because part is a reference to the internals of
        // entry->name
        nodes->back().path.assign(
            entry->name.data(), part.data() + part.size() - entry->name.data());
        it = nodes->end() - 1;
      }
      nodes = &it->children;
    }

    auto it = std::find_if(nodes->begin(), nodes->end(), [&](const auto& node) {
      return node.name == parts.back();
    });
    if (it == nodes->end()) {
      nodes->emplace_back(parts.back());
      // no need to set path, as it's identical to entry->name
      it = nodes->end() - 1;
    }
    it->entry = entry;
  }
}

bool NetworkTablesModel::Exists() {
  return nt::IsConnected(m_inst);
}

static std::shared_ptr<nt::Value> StringToBooleanArray(std::string_view in) {
  in = wpi::trim(in);
  if (in.empty()) {
    return nt::NetworkTableValue::MakeBooleanArray(
        std::initializer_list<bool>{});
  }
  if (in.front() == '[') {
    in.remove_prefix(1);
  }
  if (in.back() == ']') {
    in.remove_suffix(1);
  }
  in = wpi::trim(in);

  wpi::SmallVector<std::string_view, 16> inSplit;
  wpi::SmallVector<int, 16> out;

  wpi::split(in, inSplit, ',', -1, false);
  for (auto val : inSplit) {
    val = wpi::trim(val);
    if (wpi::equals_lower(val, "true")) {
      out.emplace_back(1);
    } else if (wpi::equals_lower(val, "false")) {
      out.emplace_back(0);
    } else {
      fmt::print(stderr,
                 "GUI: NetworkTables: Could not understand value '{}'\n", val);
      return nullptr;
    }
  }

  return nt::NetworkTableValue::MakeBooleanArray(out);
}

static std::shared_ptr<nt::Value> StringToDoubleArray(std::string_view in) {
  in = wpi::trim(in);
  if (in.empty()) {
    return nt::NetworkTableValue::MakeBooleanArray(
        std::initializer_list<bool>{});
  }
  if (in.front() == '[') {
    in.remove_prefix(1);
  }
  if (in.back() == ']') {
    in.remove_suffix(1);
  }
  in = wpi::trim(in);

  wpi::SmallVector<std::string_view, 16> inSplit;
  wpi::SmallVector<double, 16> out;

  wpi::split(in, inSplit, ',', -1, false);
  for (auto val : inSplit) {
    if (auto num = wpi::parse_float<double>(wpi::trim(val))) {
      out.emplace_back(num.value());
    } else {
      fmt::print(stderr,
                 "GUI: NetworkTables: Could not understand value '{}'\n", val);
      return nullptr;
    }
  }

  return nt::NetworkTableValue::MakeDoubleArray(out);
}

static int fromxdigit(char ch) {
  if (ch >= 'a' && ch <= 'f') {
    return (ch - 'a' + 10);
  } else if (ch >= 'A' && ch <= 'F') {
    return (ch - 'A' + 10);
  } else {
    return ch - '0';
  }
}

static std::string_view UnescapeString(std::string_view source,
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
  return {buf.data(), buf.size()};
}

static std::shared_ptr<nt::Value> StringToStringArray(std::string_view in) {
  in = wpi::trim(in);
  if (in.empty()) {
    return nt::NetworkTableValue::MakeStringArray(
        std::initializer_list<std::string>{});
  }
  if (in.front() == '[') {
    in.remove_prefix(1);
  }
  if (in.back() == ']') {
    in.remove_suffix(1);
  }
  in = wpi::trim(in);

  wpi::SmallVector<std::string_view, 16> inSplit;
  std::vector<std::string> out;
  wpi::SmallString<32> buf;

  wpi::split(in, inSplit, ',', -1, false);
  for (auto val : inSplit) {
    val = wpi::trim(val);
    if (val.empty()) {
      continue;
    }
    if (val.front() != '"' || val.back() != '"') {
      fmt::print(stderr,
                 "GUI: NetworkTables: Could not understand value '{}'\n", val);
      return nullptr;
    }
    out.emplace_back(UnescapeString(val, buf));
  }

  return nt::NetworkTableValue::MakeStringArray(std::move(out));
}

static void EmitEntryValueReadonly(NetworkTablesModel::Entry& entry) {
  auto& val = entry.value;
  if (!val) {
    return;
  }

  switch (val->type()) {
    case NT_BOOLEAN:
      ImGui::LabelText("boolean", "%s", val->GetBoolean() ? "true" : "false");
      break;
    case NT_DOUBLE:
      ImGui::LabelText("double", "%.6f", val->GetDouble());
      break;
    case NT_STRING: {
      // GetString() comes from a std::string, so it's null terminated
      ImGui::LabelText("string", "%s", val->GetString().data());
      break;
    }
    case NT_BOOLEAN_ARRAY:
      ImGui::LabelText("boolean[]", "%s", entry.valueStr.c_str());
      break;
    case NT_DOUBLE_ARRAY:
      ImGui::LabelText("double[]", "%s", entry.valueStr.c_str());
      break;
    case NT_STRING_ARRAY:
      ImGui::LabelText("string[]", "%s", entry.valueStr.c_str());
      break;
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
}

static constexpr size_t kTextBufferSize = 4096;

static char* GetTextBuffer(std::string_view in) {
  static char textBuffer[kTextBufferSize];
  size_t len = (std::min)(in.size(), kTextBufferSize - 1);
  std::memcpy(textBuffer, in.data(), len);
  textBuffer[len] = '\0';
  return textBuffer;
}

static void EmitEntryValueEditable(NetworkTablesModel::Entry& entry) {
  auto& val = entry.value;
  if (!val) {
    return;
  }

  ImGui::PushID(entry.name.c_str());
  switch (val->type()) {
    case NT_BOOLEAN: {
      static const char* boolOptions[] = {"false", "true"};
      int v = val->GetBoolean() ? 1 : 0;
      if (ImGui::Combo("boolean", &v, boolOptions, 2)) {
        nt::SetEntryValue(entry.entry, nt::NetworkTableValue::MakeBoolean(v));
      }
      break;
    }
    case NT_DOUBLE: {
      double v = val->GetDouble();
      if (ImGui::InputDouble("double", &v, 0, 0, "%.6f",
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
        nt::SetEntryValue(entry.entry, nt::NetworkTableValue::MakeDouble(v));
      }
      break;
    }
    case NT_STRING: {
      char* v = GetTextBuffer(val->GetString());
      if (ImGui::InputText("string", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        nt::SetEntryValue(entry.entry, nt::NetworkTableValue::MakeString(v));
      }
      break;
    }
    case NT_BOOLEAN_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText("boolean[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (auto outv = StringToBooleanArray(v)) {
          nt::SetEntryValue(entry.entry, std::move(outv));
        }
      }
      break;
    }
    case NT_DOUBLE_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText("double[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (auto outv = StringToDoubleArray(v)) {
          nt::SetEntryValue(entry.entry, std::move(outv));
        }
      }
      break;
    }
    case NT_STRING_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText("string[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (auto outv = StringToStringArray(v)) {
          nt::SetEntryValue(entry.entry, std::move(outv));
        }
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

static void EmitParentContextMenu(const std::string& path,
                                  NetworkTablesFlags flags) {
  // Workaround https://github.com/ocornut/imgui/issues/331
  bool openWarningPopup = false;
  static char nameBuffer[kTextBufferSize];
  if (ImGui::BeginPopupContextItem(path.c_str())) {
    ImGui::Text("%s", path.c_str());
    ImGui::Separator();

    if (ImGui::BeginMenu("Add new...")) {
      if (ImGui::IsWindowAppearing()) {
        nameBuffer[0] = '\0';
      }

      ImGui::InputTextWithHint("New item name", "example", nameBuffer,
                               kTextBufferSize);
      std::string fullNewPath;
      if (path == "/") {
        fullNewPath = path + nameBuffer;
      } else {
        fullNewPath = fmt::format("{}/{}", path, nameBuffer);
      }

      ImGui::Text("Adding: %s", fullNewPath.c_str());
      ImGui::Separator();
      auto entry = nt::GetEntry(nt::GetDefaultInstance(), fullNewPath);
      bool enabled = (flags & NetworkTablesFlags_CreateNoncanonicalKeys ||
                      nameBuffer[0] != '\0') &&
                     nt::GetEntryType(entry) == NT_Type::NT_UNASSIGNED;
      if (ImGui::MenuItem("string", nullptr, false, enabled)) {
        if (!nt::SetEntryValue(entry, nt::Value::MakeString(""))) {
          openWarningPopup = true;
        }
      }
      if (ImGui::MenuItem("double", nullptr, false, enabled)) {
        if (!nt::SetEntryValue(entry, nt::Value::MakeDouble(0.0))) {
          openWarningPopup = true;
        }
      }
      if (ImGui::MenuItem("boolean", nullptr, false, enabled)) {
        if (!nt::SetEntryValue(entry, nt::Value::MakeBoolean(false))) {
          openWarningPopup = true;
        }
      }
      if (ImGui::MenuItem("string[]", nullptr, false, enabled)) {
        if (!nt::SetEntryValue(entry, nt::Value::MakeStringArray({""}))) {
          openWarningPopup = true;
        }
      }
      if (ImGui::MenuItem("double[]", nullptr, false, enabled)) {
        if (!nt::SetEntryValue(entry, nt::Value::MakeDoubleArray({0.0}))) {
          openWarningPopup = true;
        }
      }
      if (ImGui::MenuItem("boolean[]", nullptr, false, enabled)) {
        if (!nt::SetEntryValue(entry, nt::Value::MakeBooleanArray({false}))) {
          openWarningPopup = true;
        }
      }

      ImGui::EndMenu();
    }

    ImGui::Separator();
    if (ImGui::MenuItem("Remove All")) {
      for (auto&& entry : nt::GetEntries(nt::GetDefaultInstance(), path, 0)) {
        nt::DeleteEntry(entry);
      }
    }
    ImGui::EndPopup();
  }

  // Workaround https://github.com/ocornut/imgui/issues/331
  if (openWarningPopup) {
    ImGui::OpenPopup("Value exists");
  }
  if (ImGui::BeginPopupModal("Value exists", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("The provided name %s already exists in the tree!", nameBuffer);
    ImGui::Separator();

    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
  }
}

static void EmitEntry(NetworkTablesModel::Entry& entry, const char* name,
                      NetworkTablesFlags flags) {
  if (entry.source) {
    ImGui::Selectable(name);
    entry.source->EmitDrag();
  } else {
    ImGui::Text("%s", name);
  }
  if (ImGui::BeginPopupContextItem(entry.name.c_str())) {
    ImGui::Text("%s", entry.name.c_str());
    ImGui::Separator();
    if (ImGui::MenuItem("Remove")) {
      nt::DeleteEntry(entry.entry);
    }
    ImGui::EndPopup();
  }
  ImGui::NextColumn();

  if (flags & NetworkTablesFlags_ReadOnly) {
    EmitEntryValueReadonly(entry);
  } else {
    EmitEntryValueEditable(entry);
  }
  ImGui::NextColumn();

  if (flags & NetworkTablesFlags_ShowFlags) {
    if ((entry.flags & NT_PERSISTENT) != 0) {
      ImGui::Text("Persistent");
    } else if (entry.flags != 0) {
      ImGui::Text("%02x", entry.flags);
    }
    ImGui::NextColumn();
  }

  if (flags & NetworkTablesFlags_ShowTimestamp) {
    if (entry.value) {
      ImGui::Text("%f", (entry.value->last_change() * 1.0e-6) -
                            (GetZeroTime() * 1.0e-6));
    } else {
      ImGui::TextUnformatted("");
    }
    ImGui::NextColumn();
  }
  ImGui::Separator();
}

static void EmitTree(const std::vector<NetworkTablesModel::TreeNode>& tree,
                     NetworkTablesFlags flags) {
  for (auto&& node : tree) {
    if (node.entry) {
      EmitEntry(*node.entry, node.name.c_str(), flags);
    }

    if (!node.children.empty()) {
      bool open =
          TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
      EmitParentContextMenu(node.path, flags);
      ImGui::NextColumn();
      ImGui::NextColumn();
      if (flags & NetworkTablesFlags_ShowFlags) {
        ImGui::NextColumn();
      }
      if (flags & NetworkTablesFlags_ShowTimestamp) {
        ImGui::NextColumn();
      }
      ImGui::Separator();
      if (open) {
        EmitTree(node.children, flags);
        TreePop();
      }
    }
  }
}

void glass::DisplayNetworkTables(NetworkTablesModel* model,
                                 NetworkTablesFlags flags) {
  auto inst = model->GetInstance();

  if (flags & NetworkTablesFlags_ShowConnections) {
    if (CollapsingHeader("Connections")) {
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
      for (auto&& i : nt::GetConnections(inst)) {
        ImGui::Text("%s", i.remote_id.c_str());
        ImGui::NextColumn();
        ImGui::Text("%s", i.remote_ip.c_str());
        ImGui::NextColumn();
        ImGui::Text("%llu",
                    static_cast<unsigned long long>(  // NOLINT(runtime/int)
                        i.last_update));
        ImGui::NextColumn();
        ImGui::Text("%d.%d", i.protocol_version >> 8,
                    i.protocol_version & 0xff);
        ImGui::NextColumn();
      }
      ImGui::Columns();
    }

    if (!CollapsingHeader("Values", ImGuiTreeNodeFlags_DefaultOpen)) {
      return;
    }
  }

  const bool showFlags = (flags & NetworkTablesFlags_ShowFlags);
  const bool showTimestamp = (flags & NetworkTablesFlags_ShowTimestamp);

  static bool first = true;
  ImGui::Columns(2 + (showFlags ? 1 : 0) + (showTimestamp ? 1 : 0), "values");
  if (first) {
    ImGui::SetColumnWidth(-1, 0.5f * ImGui::GetWindowWidth());
  }
  ImGui::Text("Name");
  EmitParentContextMenu("/", flags);
  ImGui::NextColumn();
  ImGui::Text("Value");
  ImGui::NextColumn();
  if (showFlags) {
    if (first) {
      ImGui::SetColumnWidth(-1, 12 * ImGui::GetFontSize());
    }
    ImGui::Text("Flags");
    ImGui::NextColumn();
  }
  if (showTimestamp) {
    ImGui::Text("Changed");
    ImGui::NextColumn();
  }
  ImGui::Separator();
  first = false;

  if (flags & NetworkTablesFlags_TreeView) {
    EmitTree(model->GetTreeRoot(), flags);
  } else {
    for (auto entry : model->GetEntries()) {
      EmitEntry(*entry, entry->name.c_str(), flags);
    }
  }
  ImGui::Columns();
}

void NetworkTablesFlagsSettings::Update() {
  if (!m_pTreeView) {
    auto& storage = GetStorage();
    m_pTreeView = storage.GetBoolRef(
        "tree", m_defaultFlags & NetworkTablesFlags_TreeView);
    m_pShowConnections = storage.GetBoolRef(
        "connections", m_defaultFlags & NetworkTablesFlags_ShowConnections);
    m_pShowFlags = storage.GetBoolRef(
        "flags", m_defaultFlags & NetworkTablesFlags_ShowFlags);
    m_pShowTimestamp = storage.GetBoolRef(
        "timestamp", m_defaultFlags & NetworkTablesFlags_ShowTimestamp);
    m_pCreateNoncanonicalKeys = storage.GetBoolRef(
        "createNonCanonical",
        m_defaultFlags & NetworkTablesFlags_CreateNoncanonicalKeys);
  }

  m_flags &=
      ~(NetworkTablesFlags_TreeView | NetworkTablesFlags_ShowConnections |
        NetworkTablesFlags_ShowFlags | NetworkTablesFlags_ShowTimestamp |
        NetworkTablesFlags_CreateNoncanonicalKeys);
  m_flags |=
      (*m_pTreeView ? NetworkTablesFlags_TreeView : 0) |
      (*m_pShowConnections ? NetworkTablesFlags_ShowConnections : 0) |
      (*m_pShowFlags ? NetworkTablesFlags_ShowFlags : 0) |
      (*m_pShowTimestamp ? NetworkTablesFlags_ShowTimestamp : 0) |
      (*m_pCreateNoncanonicalKeys ? NetworkTablesFlags_CreateNoncanonicalKeys
                                  : 0);
}

void NetworkTablesFlagsSettings::DisplayMenu() {
  if (!m_pTreeView) {
    return;
  }
  ImGui::MenuItem("Tree View", "", m_pTreeView);
  ImGui::MenuItem("Show Connections", "", m_pShowConnections);
  ImGui::MenuItem("Show Flags", "", m_pShowFlags);
  ImGui::MenuItem("Show Timestamp", "", m_pShowTimestamp);
  ImGui::Separator();
  ImGui::MenuItem("Allow creation of non-canonical keys", "",
                  m_pCreateNoncanonicalKeys);
}

void NetworkTablesView::Display() {
  m_flags.Update();
  if (ImGui::BeginPopupContextItem()) {
    m_flags.DisplayMenu();
    ImGui::EndPopup();
  }
  DisplayNetworkTables(m_model, m_flags.GetFlags());
}
