/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NetworkTables.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <vector>

#include <imgui.h>
#include <networktables/NetworkTableValue.h>
#include <ntcore_cpp.h>
#include <wpi/ArrayRef.h>
#include <wpi/Format.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "glass/Context.h"
#include "glass/DataSource.h"

using namespace glass;

static std::string BooleanArrayToString(wpi::ArrayRef<int> in) {
  std::string rv;
  wpi::raw_string_ostream os{rv};
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
  return rv;
}

static std::string DoubleArrayToString(wpi::ArrayRef<double> in) {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  os << '[';
  bool first = true;
  for (auto v : in) {
    if (!first) os << ',';
    first = false;
    os << wpi::format("%.6f", v);
  }
  os << ']';
  return rv;
}

static std::string StringArrayToString(wpi::ArrayRef<std::string> in) {
  std::string rv;
  wpi::raw_string_ostream os{rv};
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
      if (!source)
        source = std::make_unique<DataSource>(wpi::Twine{"NT:"} + name);
      source->SetValue(value->GetBoolean() ? 1 : 0);
      source->SetDigital(true);
      break;
    case NT_DOUBLE:
      if (!source)
        source = std::make_unique<DataSource>(wpi::Twine{"NT:"} + name);
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
      }
    }
    if (!entry) continue;
    if (event.flags & NT_NOTIFY_DELETE) {
      auto it = std::find(m_sortedEntries.begin(), m_sortedEntries.end(),
                          entry.get());
      // will be removed completely below
      if (it != m_sortedEntries.end()) *it = nullptr;
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
  if (!updateTree) return;

  // remove deleted entries
  m_sortedEntries.erase(
      std::remove(m_sortedEntries.begin(), m_sortedEntries.end(), nullptr),
      m_sortedEntries.end());

  // sort by name
  std::sort(m_sortedEntries.begin(), m_sortedEntries.end(),
            [](const auto& a, const auto& b) { return a->name < b->name; });

  // rebuild tree
  m_root.clear();
  wpi::SmallVector<wpi::StringRef, 16> parts;
  for (auto& entry : m_sortedEntries) {
    parts.clear();
    wpi::StringRef{entry->name}.split(parts, '/', -1, false);

    // get to leaf
    auto nodes = &m_root;
    for (auto part : wpi::ArrayRef(parts.begin(), parts.end()).drop_back()) {
      auto it =
          std::find_if(nodes->begin(), nodes->end(),
                       [&](const auto& node) { return node.name == part; });
      if (it == nodes->end()) {
        nodes->emplace_back(part);
        // path is from the beginning of the string to the end of the current
        // part; this works because part is a reference to the internals of
        // entry->name
        nodes->back().path.assign(entry->name.data(),
                                  part.end() - entry->name.data());
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

bool NetworkTablesModel::Exists() { return nt::IsConnected(m_inst); }

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

static void EmitEntryValueReadonly(NetworkTablesModel::Entry& entry) {
  auto& val = entry.value;
  if (!val) return;

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

static char* GetTextBuffer(wpi::StringRef in) {
  static char textBuffer[kTextBufferSize];
  size_t len = (std::min)(in.size(), kTextBufferSize - 1);
  std::memcpy(textBuffer, in.data(), len);
  textBuffer[len] = '\0';
  return textBuffer;
}

static void EmitEntryValueEditable(NetworkTablesModel::Entry& entry) {
  auto& val = entry.value;
  if (!val) return;

  ImGui::PushID(entry.name.c_str());
  switch (val->type()) {
    case NT_BOOLEAN: {
      static const char* boolOptions[] = {"false", "true"};
      int v = val->GetBoolean() ? 1 : 0;
      if (ImGui::Combo("boolean", &v, boolOptions, 2))
        nt::SetEntryValue(entry.entry, nt::NetworkTableValue::MakeBoolean(v));
      break;
    }
    case NT_DOUBLE: {
      double v = val->GetDouble();
      if (ImGui::InputDouble("double", &v, 0, 0, "%.6f",
                             ImGuiInputTextFlags_EnterReturnsTrue))
        nt::SetEntryValue(entry.entry, nt::NetworkTableValue::MakeDouble(v));
      break;
    }
    case NT_STRING: {
      char* v = GetTextBuffer(val->GetString());
      if (ImGui::InputText("string", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue))
        nt::SetEntryValue(entry.entry, nt::NetworkTableValue::MakeString(v));
      break;
    }
    case NT_BOOLEAN_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText("boolean[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (auto outv = StringToBooleanArray(v))
          nt::SetEntryValue(entry.entry, std::move(outv));
      }
      break;
    }
    case NT_DOUBLE_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText("double[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (auto outv = StringToDoubleArray(v))
          nt::SetEntryValue(entry.entry, std::move(outv));
      }
      break;
    }
    case NT_STRING_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText("string[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (auto outv = StringToStringArray(v))
          nt::SetEntryValue(entry.entry, std::move(outv));
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

static void EmitEntry(NetworkTablesModel::Entry& entry, const char* name,
                      NetworkTablesFlags flags) {
  if (entry.source) {
    ImGui::Selectable(name);
    entry.source->EmitDrag();
  } else {
    ImGui::Text("%s", name);
  }
  ImGui::NextColumn();

  if (flags & NetworkTablesFlags_ReadOnly)
    EmitEntryValueReadonly(entry);
  else
    EmitEntryValueEditable(entry);
  ImGui::NextColumn();

  if (flags & NetworkTablesFlags_ShowFlags) {
    if ((entry.flags & NT_PERSISTENT) != 0)
      ImGui::Text("Persistent");
    else if (entry.flags != 0)
      ImGui::Text("%02x", entry.flags);
    ImGui::NextColumn();
  }

  if (flags & NetworkTablesFlags_ShowTimestamp) {
    if (entry.value)
      ImGui::Text("%f", (entry.value->last_change() * 1.0e-6) -
                            (GetZeroTime() * 1.0e-6));
    else
      ImGui::TextUnformatted("");
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
      ImGui::NextColumn();
      ImGui::NextColumn();
      if (flags & NetworkTablesFlags_ShowFlags) ImGui::NextColumn();
      if (flags & NetworkTablesFlags_ShowTimestamp) ImGui::NextColumn();
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

    if (!CollapsingHeader("Values", ImGuiTreeNodeFlags_DefaultOpen)) return;
  }

  const bool showFlags = (flags & NetworkTablesFlags_ShowFlags);
  const bool showTimestamp = (flags & NetworkTablesFlags_ShowTimestamp);

  static bool first = true;
  ImGui::Columns(2 + (showFlags ? 1 : 0) + (showTimestamp ? 1 : 0), "values");
  if (first) ImGui::SetColumnWidth(-1, 0.5f * ImGui::GetWindowWidth());
  ImGui::Text("Name");
  ImGui::NextColumn();
  ImGui::Text("Value");
  ImGui::NextColumn();
  if (showFlags) {
    if (first) ImGui::SetColumnWidth(-1, 12 * ImGui::GetFontSize());
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

void NetworkTablesView::Display() {
  if (ImGui::BeginPopupContextItem()) {
    auto& storage = GetStorage();
    auto pTreeView = storage.GetBoolRef(
        "tree", m_defaultFlags & NetworkTablesFlags_TreeView);
    auto pShowConnections = storage.GetBoolRef(
        "connections", m_defaultFlags & NetworkTablesFlags_ShowConnections);
    auto pShowFlags = storage.GetBoolRef(
        "flags", m_defaultFlags & NetworkTablesFlags_ShowFlags);
    auto pShowTimestamp = storage.GetBoolRef(
        "timestamp", m_defaultFlags & NetworkTablesFlags_ShowTimestamp);

    ImGui::MenuItem("Tree View", "", pTreeView);
    ImGui::MenuItem("Show Connections", "", pShowConnections);
    ImGui::MenuItem("Show Flags", "", pShowFlags);
    ImGui::MenuItem("Show Timestamp", "", pShowTimestamp);

    m_flags &=
        ~(NetworkTablesFlags_TreeView | NetworkTablesFlags_ShowConnections |
          NetworkTablesFlags_ShowFlags | NetworkTablesFlags_ShowTimestamp);
    m_flags |= (*pTreeView ? NetworkTablesFlags_TreeView : 0) |
               (*pShowConnections ? NetworkTablesFlags_ShowConnections : 0) |
               (*pShowFlags ? NetworkTablesFlags_ShowFlags : 0) |
               (*pShowTimestamp ? NetworkTablesFlags_ShowTimestamp : 0);
    ImGui::EndPopup();
  }

  DisplayNetworkTables(m_model, m_flags);
}
