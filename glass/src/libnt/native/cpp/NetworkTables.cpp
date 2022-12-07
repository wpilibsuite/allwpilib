// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NetworkTables.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <imgui.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableValue.h>
#include <ntcore_c.h>
#include <ntcore_cpp.h>
#include <ntcore_cpp_types.h>
#include <wpi/MessagePack.h>
#include <wpi/SmallString.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/mpack.h>
#include <wpi/raw_ostream.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"

using namespace glass;
using namespace mpack;

namespace {
enum ShowCategory {
  ShowPersistent,
  ShowRetained,
  ShowTransitory,
  ShowAll,
};
}  // namespace

static bool IsVisible(ShowCategory category, bool persistent, bool retained) {
  switch (category) {
    case ShowPersistent:
      return persistent;
    case ShowRetained:
      return retained && !persistent;
    case ShowTransitory:
      return !retained && !persistent;
    case ShowAll:
      return true;
    default:
      return false;
  }
}

static std::string BooleanArrayToString(std::span<const int> in) {
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

static std::string IntegerArrayToString(std::span<const int64_t> in) {
  return fmt::format("[{:d}]", fmt::join(in, ","));
}

template <typename T>
static std::string FloatArrayToString(std::span<const T> in) {
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>);
  return fmt::format("[{:.6f}]", fmt::join(in, ","));
}

static std::string StringArrayToString(std::span<const std::string> in) {
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
    : NetworkTablesModel{nt::NetworkTableInstance::GetDefault()} {}

NetworkTablesModel::NetworkTablesModel(nt::NetworkTableInstance inst)
    : m_inst{inst}, m_poller{inst} {
  m_poller.AddListener({{"", "$"}}, nt::EventFlags::kTopic |
                                        nt::EventFlags::kValueAll |
                                        nt::EventFlags::kImmediate);
}

NetworkTablesModel::Entry::~Entry() {
  if (publisher != 0) {
    nt::Unpublish(publisher);
  }
}

void NetworkTablesModel::Entry::UpdateInfo(nt::TopicInfo&& info_) {
  info = std::move(info_);
  properties = info.GetProperties();

  persistent = false;
  auto it = properties.find("persistent");
  if (it != properties.end()) {
    if (auto v = it->get_ptr<const bool*>()) {
      persistent = *v;
    }
  }

  retained = false;
  it = properties.find("retained");
  if (it != properties.end()) {
    if (auto v = it->get_ptr<const bool*>()) {
      retained = *v;
    }
  }
}

static void UpdateMsgpackValueSource(NetworkTablesModel::ValueSource* out,
                                     mpack_reader_t& r, std::string_view name,
                                     int64_t time) {
  mpack_tag_t tag = mpack_read_tag(&r);
  switch (mpack_tag_type(&tag)) {
    case mpack::mpack_type_bool:
      out->UpdateFromValue(
          nt::Value::MakeBoolean(mpack_tag_bool_value(&tag), time), name, "");
      break;
    case mpack::mpack_type_int:
      out->UpdateFromValue(
          nt::Value::MakeInteger(mpack_tag_int_value(&tag), time), name, "");
      break;
    case mpack::mpack_type_uint:
      out->UpdateFromValue(
          nt::Value::MakeInteger(mpack_tag_uint_value(&tag), time), name, "");
      break;
    case mpack::mpack_type_float:
      out->UpdateFromValue(
          nt::Value::MakeFloat(mpack_tag_float_value(&tag), time), name, "");
      break;
    case mpack::mpack_type_double:
      out->UpdateFromValue(
          nt::Value::MakeDouble(mpack_tag_double_value(&tag), time), name, "");
      break;
    case mpack::mpack_type_str: {
      std::string str;
      mpack_read_str(&r, &tag, &str);
      out->UpdateFromValue(nt::Value::MakeString(std::move(str), time), name,
                           "");
      break;
    }
    case mpack::mpack_type_bin:
      // just skip it
      mpack_skip_bytes(&r, mpack_tag_bin_length(&tag));
      mpack_done_bin(&r);
      break;
    case mpack::mpack_type_array: {
      if (out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = false;
      }
      out->valueChildren.resize(mpack_tag_array_count(&tag));
      unsigned int i = 0;
      for (auto&& child : out->valueChildren) {
        if (child.name.empty()) {
          child.name = fmt::format("[{}]", i);
          child.path = fmt::format("{}{}", name, child.name);
        }
        ++i;
        UpdateMsgpackValueSource(&child, r, child.path, time);  // recurse
      }
      mpack_done_array(&r);
      break;
    }
    case mpack::mpack_type_map: {
      if (!out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = true;
      }
      wpi::StringMap<size_t> elems;
      for (size_t i = 0, size = out->valueChildren.size(); i < size; ++i) {
        elems[out->valueChildren[i].name] = i;
      }
      bool added = false;
      uint32_t count = mpack_tag_map_count(&tag);
      for (uint32_t i = 0; i < count; ++i) {
        std::string key;
        if (mpack_expect_str(&r, &key) == mpack_ok) {
          auto it = elems.find(key);
          if (it != elems.end()) {
            auto& child = out->valueChildren[it->second];
            UpdateMsgpackValueSource(&child, r, child.path, time);
            elems.erase(it);
          } else {
            added = true;
            out->valueChildren.emplace_back();
            auto& child = out->valueChildren.back();
            child.name = std::move(key);
            child.path = fmt::format("{}/{}", name, child.name);
            UpdateMsgpackValueSource(&child, r, child.path, time);
          }
        }
      }
      // erase unmatched keys
      out->valueChildren.erase(
          std::remove_if(
              out->valueChildren.begin(), out->valueChildren.end(),
              [&](const auto& child) { return elems.count(child.name) > 0; }),
          out->valueChildren.end());
      if (added) {
        // sort by name
        std::sort(out->valueChildren.begin(), out->valueChildren.end(),
                  [](const auto& a, const auto& b) { return a.name < b.name; });
      }
      mpack_done_map(&r);
      break;
    }
    default:
      out->value = {};
      mpack_done_type(&r, mpack_tag_type(&tag));
      break;
  }
}

static void UpdateJsonValueSource(NetworkTablesModel::ValueSource* out,
                                  const wpi::json& j, std::string_view name,
                                  int64_t time) {
  switch (j.type()) {
    case wpi::json::value_t::object: {
      if (!out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = true;
      }
      wpi::StringMap<size_t> elems;
      for (size_t i = 0, size = out->valueChildren.size(); i < size; ++i) {
        elems[out->valueChildren[i].name] = i;
      }
      bool added = false;
      for (auto&& kv : j.items()) {
        auto it = elems.find(kv.key());
        if (it != elems.end()) {
          auto& child = out->valueChildren[it->second];
          UpdateJsonValueSource(&child, kv.value(), child.path, time);
          elems.erase(it);
        } else {
          added = true;
          out->valueChildren.emplace_back();
          auto& child = out->valueChildren.back();
          child.name = kv.key();
          child.path = fmt::format("{}/{}", name, child.name);
          UpdateJsonValueSource(&child, kv.value(), child.path, time);
        }
      }
      // erase unmatched keys
      out->valueChildren.erase(
          std::remove_if(
              out->valueChildren.begin(), out->valueChildren.end(),
              [&](const auto& child) { return elems.count(child.name) > 0; }),
          out->valueChildren.end());
      if (added) {
        // sort by name
        std::sort(out->valueChildren.begin(), out->valueChildren.end(),
                  [](const auto& a, const auto& b) { return a.name < b.name; });
      }
      break;
    }
    case wpi::json::value_t::array: {
      if (out->valueChildrenMap) {
        out->valueChildren.clear();
        out->valueChildrenMap = false;
      }
      out->valueChildren.resize(j.size());
      unsigned int i = 0;
      for (auto&& child : out->valueChildren) {
        if (child.name.empty()) {
          child.name = fmt::format("[{}]", i);
          child.path = fmt::format("{}{}", name, child.name);
        }
        ++i;
        UpdateJsonValueSource(&child, j[i], child.path, time);  // recurse
      }
      break;
    }
    case wpi::json::value_t::string:
      out->UpdateFromValue(
          nt::Value::MakeString(j.get_ref<const std::string&>(), time), name,
          "");
      break;
    case wpi::json::value_t::boolean:
      out->UpdateFromValue(nt::Value::MakeBoolean(j.get<bool>(), time), name,
                           "");
      break;
    case wpi::json::value_t::number_integer:
      out->UpdateFromValue(nt::Value::MakeInteger(j.get<int64_t>(), time), name,
                           "");
      break;
    case wpi::json::value_t::number_unsigned:
      out->UpdateFromValue(nt::Value::MakeInteger(j.get<uint64_t>(), time),
                           name, "");
      break;
    case wpi::json::value_t::number_float:
      out->UpdateFromValue(nt::Value::MakeDouble(j.get<double>(), time), name,
                           "");
      break;
    default:
      out->value = {};
      break;
  }
}

void NetworkTablesModel::ValueSource::UpdateFromValue(
    nt::Value&& v, std::string_view name, std::string_view typeStr) {
  value = v;
  switch (value.type()) {
    case NT_BOOLEAN:
      valueChildren.clear();
      if (!source) {
        source = std::make_unique<DataSource>(fmt::format("NT:{}", name));
      }
      source->SetValue(value.GetBoolean() ? 1 : 0, value.last_change());
      source->SetDigital(true);
      break;
    case NT_INTEGER:
      valueChildren.clear();
      if (!source) {
        source = std::make_unique<DataSource>(fmt::format("NT:{}", name));
      }
      source->SetValue(value.GetInteger(), value.last_change());
      source->SetDigital(false);
      break;
    case NT_FLOAT:
      valueChildren.clear();
      if (!source) {
        source = std::make_unique<DataSource>(fmt::format("NT:{}", name));
      }
      source->SetValue(value.GetFloat(), value.last_change());
      source->SetDigital(false);
      break;
    case NT_DOUBLE:
      valueChildren.clear();
      if (!source) {
        source = std::make_unique<DataSource>(fmt::format("NT:{}", name));
      }
      source->SetValue(value.GetDouble(), value.last_change());
      source->SetDigital(false);
      break;
    case NT_BOOLEAN_ARRAY:
      valueChildren.clear();
      valueStr = BooleanArrayToString(value.GetBooleanArray());
      break;
    case NT_INTEGER_ARRAY:
      valueChildren.clear();
      valueStr = IntegerArrayToString(value.GetIntegerArray());
      break;
    case NT_FLOAT_ARRAY:
      valueChildren.clear();
      valueStr = FloatArrayToString(value.GetFloatArray());
      break;
    case NT_DOUBLE_ARRAY:
      valueChildren.clear();
      valueStr = FloatArrayToString(value.GetDoubleArray());
      break;
    case NT_STRING_ARRAY:
      valueChildren.clear();
      valueStr = StringArrayToString(value.GetStringArray());
      break;
    case NT_STRING:
      if (typeStr == "json") {
        try {
          UpdateJsonValueSource(this, wpi::json::parse(value.GetString()), name,
                                value.last_change());
        } catch (wpi::json::exception&) {
          // ignore
        }
      } else {
        valueChildren.clear();
      }
      break;
    case NT_RAW:
      if (typeStr == "msgpack") {
        mpack_reader_t r;
        mpack_reader_init_data(&r, value.GetRaw());
        UpdateMsgpackValueSource(this, r, name, value.last_change());

        mpack_reader_destroy(&r);
      } else {
        valueChildren.clear();
      }
      break;
    default:
      valueChildren.clear();
      break;
  }
}

void NetworkTablesModel::Update() {
  bool updateTree = false;
  for (auto&& event : m_poller.ReadQueue()) {
    if (auto info = event.GetTopicInfo()) {
      auto& entry = m_entries[info->topic];
      if (event.flags & nt::EventFlags::kPublish) {
        if (!entry) {
          entry = std::make_unique<Entry>();
          m_sortedEntries.emplace_back(entry.get());
          updateTree = true;
        }
      }
      if (event.flags & nt::EventFlags::kUnpublish) {
        // meta topic handling
        if (wpi::starts_with(info->name, '$')) {
          // meta topic handling
          if (info->name == "$clients") {
            m_clients.clear();
          } else if (info->name == "$serverpub") {
            m_server.publishers.clear();
          } else if (info->name == "$serversub") {
            m_server.subscribers.clear();
          } else if (wpi::starts_with(info->name, "$clientpub$")) {
            auto it = m_clients.find(wpi::drop_front(info->name, 11));
            if (it != m_clients.end()) {
              it->second.publishers.clear();
            }
          } else if (wpi::starts_with(info->name, "$clientsub$")) {
            auto it = m_clients.find(wpi::drop_front(info->name, 11));
            if (it != m_clients.end()) {
              it->second.subscribers.clear();
            }
          }
        }
        auto it = std::find(m_sortedEntries.begin(), m_sortedEntries.end(),
                            entry.get());
        // will be removed completely below
        if (it != m_sortedEntries.end()) {
          *it = nullptr;
        }
        m_entries.erase(info->topic);
        updateTree = true;
        continue;
      }
      if (event.flags & nt::EventFlags::kProperties) {
        updateTree = true;
      }
      if (entry) {
        entry->UpdateTopic(std::move(event));
      }
    } else if (auto valueData = event.GetValueEventData()) {
      auto& entry = m_entries[valueData->topic];
      if (entry) {
        entry->UpdateFromValue(std::move(valueData->value), entry->info.name,
                               entry->info.type_str);
        if (wpi::starts_with(entry->info.name, '$') && entry->value.IsRaw() &&
            entry->info.type_str == "msgpack") {
          // meta topic handling
          if (entry->info.name == "$clients") {
            UpdateClients(entry->value.GetRaw());
          } else if (entry->info.name == "$serverpub") {
            m_server.UpdatePublishers(entry->value.GetRaw());
          } else if (entry->info.name == "$serversub") {
            m_server.UpdateSubscribers(entry->value.GetRaw());
          } else if (wpi::starts_with(entry->info.name, "$clientpub$")) {
            auto it = m_clients.find(wpi::drop_front(entry->info.name, 11));
            if (it != m_clients.end()) {
              it->second.UpdatePublishers(entry->value.GetRaw());
            }
          } else if (wpi::starts_with(entry->info.name, "$clientsub$")) {
            auto it = m_clients.find(wpi::drop_front(entry->info.name, 11));
            if (it != m_clients.end()) {
              it->second.UpdateSubscribers(entry->value.GetRaw());
            }
          }
        }
      }
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

  RebuildTree();
}

void NetworkTablesModel::RebuildTree() {
  // sort by name
  std::sort(
      m_sortedEntries.begin(), m_sortedEntries.end(),
      [](const auto& a, const auto& b) { return a->info.name < b->info.name; });

  RebuildTreeImpl(&m_root, ShowAll);
  RebuildTreeImpl(&m_persistentRoot, ShowPersistent);
  RebuildTreeImpl(&m_retainedRoot, ShowRetained);
  RebuildTreeImpl(&m_transitoryRoot, ShowTransitory);
}

void NetworkTablesModel::RebuildTreeImpl(std::vector<TreeNode>* tree,
                                         int category) {
  tree->clear();
  wpi::SmallVector<std::string_view, 16> parts;
  for (auto& entry : m_sortedEntries) {
    if (!IsVisible(static_cast<ShowCategory>(category), entry->persistent,
                   entry->retained)) {
      continue;
    }
    parts.clear();
    wpi::split(entry->info.name, parts, '/', -1, false);

    // ignore a raw "/" key
    if (parts.empty()) {
      continue;
    }

    // get to leaf
    auto nodes = tree;
    for (auto part : wpi::drop_back(std::span{parts.begin(), parts.end()})) {
      auto it =
          std::find_if(nodes->begin(), nodes->end(),
                       [&](const auto& node) { return node.name == part; });
      if (it == nodes->end()) {
        nodes->emplace_back(part);
        // path is from the beginning of the string to the end of the current
        // part; this works because part is a reference to the internals of
        // entry->info.name
        nodes->back().path.assign(
            entry->info.name.data(),
            part.data() + part.size() - entry->info.name.data());
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
  return true;
}

NetworkTablesModel::Entry* NetworkTablesModel::GetEntry(std::string_view name) {
  auto entryIt = std::lower_bound(
      m_sortedEntries.begin(), m_sortedEntries.end(), name,
      [](auto&& entry, auto&& name) { return entry->info.name < name; });
  if (entryIt == m_sortedEntries.end() || (*entryIt)->info.name != name) {
    return nullptr;
  }
  return *entryIt;
}

NetworkTablesModel::Entry* NetworkTablesModel::AddEntry(NT_Topic topic) {
  auto& entry = m_entries[topic];
  if (!entry) {
    entry = std::make_unique<Entry>();
    entry->info = nt::GetTopicInfo(topic);
    entry->properties = entry->info.GetProperties();
    m_sortedEntries.emplace_back(entry.get());
  }
  RebuildTree();
  return entry.get();
}

void NetworkTablesModel::Client::UpdatePublishers(
    std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numPub = mpack_expect_array_max(&r, 1000);
  std::vector<ClientPublisher> newPublishers;
  newPublishers.reserve(numPub);
  for (uint32_t i = 0; i < numPub; ++i) {
    ClientPublisher pub;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "uid") {
        pub.uid = mpack_expect_i64(&r);
      } else if (key == "topic") {
        mpack_expect_str(&r, &pub.topic);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    newPublishers.emplace_back(std::move(pub));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) == mpack_ok) {
    publishers = std::move(newPublishers);
  } else {
    fmt::print(stderr, "Failed to update publishers\n");
  }
}

static void DecodeSubscriberOptions(
    mpack_reader_t& r, NetworkTablesModel::SubscriberOptions* options) {
  *options = NetworkTablesModel::SubscriberOptions{};
  uint32_t numMapElem = mpack_expect_map(&r);
  for (uint32_t j = 0; j < numMapElem; ++j) {
    std::string key;
    mpack_expect_str(&r, &key);
    if (key == "topicsonly") {
      options->topicsOnly = mpack_expect_bool(&r);
    } else if (key == "all") {
      options->sendAll = mpack_expect_bool(&r);
    } else if (key == "periodic") {
      options->periodic = mpack_expect_float(&r);
    } else if (key == "prefix") {
      options->prefixMatch = mpack_expect_bool(&r);
    } else {
      // TODO: Save other options
      mpack_discard(&r);
    }
  }
  mpack_done_map(&r);
}

void NetworkTablesModel::Client::UpdateSubscribers(
    std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numSub = mpack_expect_array_max(&r, 1000);
  std::vector<ClientSubscriber> newSubscribers;
  newSubscribers.reserve(numSub);
  for (uint32_t i = 0; i < numSub; ++i) {
    ClientSubscriber sub;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "uid") {
        sub.uid = mpack_expect_i64(&r);
      } else if (key == "topics") {
        uint32_t numPrefix = mpack_expect_array_max(&r, 100);
        sub.topics.reserve(numPrefix);
        for (uint32_t k = 0; k < numPrefix; ++k) {
          std::string val;
          if (mpack_expect_str(&r, &val) == mpack_ok) {
            sub.topics.emplace_back(std::move(val));
          }
        }
        sub.topicsStr = StringArrayToString(sub.topics);
        mpack_done_array(&r);
      } else if (key == "options") {
        DecodeSubscriberOptions(r, &sub.options);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    newSubscribers.emplace_back(std::move(sub));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) == mpack_ok) {
    subscribers = std::move(newSubscribers);
  } else {
    fmt::print(stderr, "Failed to update subscribers\n");
  }
}

void NetworkTablesModel::UpdateClients(std::span<const uint8_t> data) {
  mpack_reader_t r;
  mpack_reader_init_data(&r, data);
  uint32_t numClients = mpack_expect_array_max(&r, 100);
  std::vector<Client> clientsArr;
  clientsArr.reserve(numClients);
  for (uint32_t i = 0; i < numClients; ++i) {
    Client client;
    uint32_t numMapElem = mpack_expect_map(&r);
    for (uint32_t j = 0; j < numMapElem; ++j) {
      std::string key;
      mpack_expect_str(&r, &key);
      if (key == "id") {
        mpack_expect_str(&r, &client.id);
      } else if (key == "conn") {
        mpack_expect_str(&r, &client.conn);
      } else if (key == "ver") {
        uint16_t val = mpack_expect_u16(&r);
        client.version = fmt::format("{}.{}", val >> 8, val & 0xff);
      } else {
        mpack_discard(&r);
      }
    }
    mpack_done_map(&r);
    clientsArr.emplace_back(std::move(client));
  }
  mpack_done_array(&r);
  if (mpack_reader_destroy(&r) != mpack_ok) {
    return;
  }

  // we need to create a new map so deletions are reflected
  std::map<std::string, Client, std::less<>> newClients;
  for (auto&& client : clientsArr) {
    auto& newClient = newClients[client.id];
    newClient = std::move(client);
    auto it = m_clients.find(newClient.id);
    if (it != m_clients.end()) {
      // transfer from existing
      newClient.publishers = std::move(it->second.publishers);
      newClient.subscribers = std::move(it->second.subscribers);
    } else {
      // initially populate
      if (Entry* entry = GetEntry(fmt::format("$clientpub${}", newClient.id))) {
        if (entry->value.IsRaw() && entry->info.type_str == "msgpack") {
          newClient.UpdatePublishers(entry->value.GetRaw());
        }
      }
      if (Entry* entry = GetEntry(fmt::format("$clientsub${}", newClient.id))) {
        if (entry->value.IsRaw() && entry->info.type_str == "msgpack") {
          newClient.UpdateSubscribers(entry->value.GetRaw());
        }
      }
    }
  }

  // replace map
  m_clients = std::move(newClients);
}

static bool StringToBooleanArray(std::string_view in, std::vector<int>* out) {
  in = wpi::trim(in);
  if (in.empty()) {
    return false;
  }
  if (in.front() == '[') {
    in.remove_prefix(1);
  }
  if (in.back() == ']') {
    in.remove_suffix(1);
  }
  in = wpi::trim(in);

  wpi::SmallVector<std::string_view, 16> inSplit;

  wpi::split(in, inSplit, ',', -1, false);
  for (auto val : inSplit) {
    val = wpi::trim(val);
    if (wpi::equals_lower(val, "true")) {
      out->emplace_back(1);
    } else if (wpi::equals_lower(val, "false")) {
      out->emplace_back(0);
    } else {
      fmt::print(stderr,
                 "GUI: NetworkTables: Could not understand value '{}'\n", val);
      return false;
    }
  }

  return true;
}

static bool StringToIntegerArray(std::string_view in,
                                 std::vector<int64_t>* out) {
  in = wpi::trim(in);
  if (in.empty()) {
    return false;
  }
  if (in.front() == '[') {
    in.remove_prefix(1);
  }
  if (in.back() == ']') {
    in.remove_suffix(1);
  }
  in = wpi::trim(in);

  wpi::SmallVector<std::string_view, 16> inSplit;

  wpi::split(in, inSplit, ',', -1, false);
  for (auto val : inSplit) {
    if (auto num = wpi::parse_integer<int64_t>(wpi::trim(val), 0)) {
      out->emplace_back(num.value());
    } else {
      fmt::print(stderr,
                 "GUI: NetworkTables: Could not understand value '{}'\n", val);
      return false;
    }
  }

  return true;
}

template <typename T>
static bool StringToFloatArray(std::string_view in, std::vector<T>* out) {
  static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>);
  in = wpi::trim(in);
  if (in.empty()) {
    return false;
  }
  if (in.front() == '[') {
    in.remove_prefix(1);
  }
  if (in.back() == ']') {
    in.remove_suffix(1);
  }
  in = wpi::trim(in);

  wpi::SmallVector<std::string_view, 16> inSplit;

  wpi::split(in, inSplit, ',', -1, false);
  for (auto val : inSplit) {
    if (auto num = wpi::parse_float<T>(wpi::trim(val))) {
      out->emplace_back(num.value());
    } else {
      fmt::print(stderr,
                 "GUI: NetworkTables: Could not understand value '{}'\n", val);
      return false;
    }
  }

  return true;
}

static bool StringToStringArray(std::string_view in,
                                std::vector<std::string>* out) {
  in = wpi::trim(in);
  if (in.empty()) {
    return false;
  }
  if (in.front() == '[') {
    in.remove_prefix(1);
  }
  if (in.back() == ']') {
    in.remove_suffix(1);
  }
  in = wpi::trim(in);

  wpi::SmallVector<std::string_view, 16> inSplit;
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
      return false;
    }
    val.remove_prefix(1);
    val.remove_suffix(1);
    out->emplace_back(wpi::UnescapeCString(val, buf).first);
  }

  return true;
}

static void EmitEntryValueReadonly(const NetworkTablesModel::ValueSource& entry,
                                   const char* typeStr,
                                   NetworkTablesFlags flags) {
  auto& val = entry.value;
  if (!val) {
    return;
  }

  switch (val.type()) {
    case NT_BOOLEAN:
      ImGui::LabelText(typeStr ? typeStr : "boolean", "%s",
                       val.GetBoolean() ? "true" : "false");
      break;
    case NT_INTEGER:
      ImGui::LabelText(typeStr ? typeStr : "int", "%" PRId64, val.GetInteger());
      break;
    case NT_FLOAT:
      ImGui::LabelText(typeStr ? typeStr : "double", "%.6f", val.GetFloat());
      break;
    case NT_DOUBLE: {
      unsigned char precision = (flags & NetworkTablesFlags_Precision) >>
                                kNetworkTablesFlags_PrecisionBitShift;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
      ImGui::LabelText(typeStr ? typeStr : "double",
                       fmt::format("%.{}f", precision).c_str(),
                       val.GetDouble());
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      break;
    }
    case NT_STRING: {
      // GetString() comes from a std::string, so it's null terminated
      ImGui::LabelText(typeStr ? typeStr : "string", "%s",
                       val.GetString().data());
      break;
    }
    case NT_BOOLEAN_ARRAY:
      ImGui::LabelText(typeStr ? typeStr : "boolean[]", "%s",
                       entry.valueStr.c_str());
      break;
    case NT_INTEGER_ARRAY:
      ImGui::LabelText(typeStr ? typeStr : "int[]", "%s",
                       entry.valueStr.c_str());
      break;
    case NT_FLOAT_ARRAY:
      ImGui::LabelText(typeStr ? typeStr : "float[]", "%s",
                       entry.valueStr.c_str());
      break;
    case NT_DOUBLE_ARRAY:
      ImGui::LabelText(typeStr ? typeStr : "double[]", "%s",
                       entry.valueStr.c_str());
      break;
    case NT_STRING_ARRAY:
      ImGui::LabelText(typeStr ? typeStr : "string[]", "%s",
                       entry.valueStr.c_str());
      break;
    case NT_RAW:
      ImGui::LabelText(typeStr ? typeStr : "raw", "[...]");
      break;
    default:
      ImGui::LabelText(typeStr ? typeStr : "other", "?");
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

static void EmitEntryValueEditable(NetworkTablesModel::Entry& entry,
                                   NetworkTablesFlags flags) {
  auto& val = entry.value;
  if (!val) {
    return;
  }

  const char* typeStr =
      entry.info.type_str.empty() ? nullptr : entry.info.type_str.c_str();
  ImGui::PushID(entry.info.name.c_str());
  switch (val.type()) {
    case NT_BOOLEAN: {
      static const char* boolOptions[] = {"false", "true"};
      int v = val.GetBoolean() ? 1 : 0;
      if (ImGui::Combo(typeStr ? typeStr : "boolean", &v, boolOptions, 2)) {
        if (entry.publisher == 0) {
          entry.publisher =
              nt::Publish(entry.info.topic, NT_BOOLEAN, "boolean");
        }
        nt::SetBoolean(entry.publisher, v);
      }
      break;
    }
    case NT_INTEGER: {
      int64_t v = val.GetInteger();
      if (ImGui::InputScalar(typeStr ? typeStr : "int", ImGuiDataType_S64, &v,
                             nullptr, nullptr, nullptr,
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (entry.publisher == 0) {
          entry.publisher = nt::Publish(entry.info.topic, NT_INTEGER, "int");
        }
        nt::SetInteger(entry.publisher, v);
      }
      break;
    }
    case NT_FLOAT: {
      float v = val.GetFloat();
      if (ImGui::InputFloat(typeStr ? typeStr : "float", &v, 0, 0, "%.6f",
                            ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (entry.publisher == 0) {
          entry.publisher = nt::Publish(entry.info.topic, NT_FLOAT, "float");
        }
        nt::SetFloat(entry.publisher, v);
      }
      break;
    }
    case NT_DOUBLE: {
      double v = val.GetDouble();
      unsigned char precision = (flags & NetworkTablesFlags_Precision) >>
                                kNetworkTablesFlags_PrecisionBitShift;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
      if (ImGui::InputDouble(typeStr ? typeStr : "double", &v, 0, 0,
                             fmt::format("%.{}f", precision).c_str(),
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (entry.publisher == 0) {
          entry.publisher = nt::Publish(entry.info.topic, NT_DOUBLE, "double");
        }
        nt::SetDouble(entry.publisher, v);
      }
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      break;
    }
    case NT_STRING: {
      char* v = GetTextBuffer(val.GetString());
      if (ImGui::InputText(typeStr ? typeStr : "string", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (entry.publisher == 0) {
          entry.publisher = nt::Publish(entry.info.topic, NT_STRING, "string");
        }
        nt::SetString(entry.publisher, v);
      }
      break;
    }
    case NT_BOOLEAN_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText(typeStr ? typeStr : "boolean[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::vector<int> outv;
        if (StringToBooleanArray(v, &outv)) {
          if (entry.publisher == 0) {
            entry.publisher =
                nt::Publish(entry.info.topic, NT_BOOLEAN_ARRAY, "boolean[]");
          }
          nt::SetBooleanArray(entry.publisher, outv);
        }
      }
      break;
    }
    case NT_INTEGER_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText(typeStr ? typeStr : "int[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::vector<int64_t> outv;
        if (StringToIntegerArray(v, &outv)) {
          if (entry.publisher == 0) {
            entry.publisher =
                nt::Publish(entry.info.topic, NT_INTEGER_ARRAY, "int[]");
          }
          nt::SetIntegerArray(entry.publisher, outv);
        }
      }
      break;
    }
    case NT_FLOAT_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText(typeStr ? typeStr : "float[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::vector<float> outv;
        if (StringToFloatArray(v, &outv)) {
          if (entry.publisher == 0) {
            entry.publisher =
                nt::Publish(entry.info.topic, NT_DOUBLE_ARRAY, "float[]");
          }
          nt::SetFloatArray(entry.publisher, outv);
        }
      }
      break;
    }
    case NT_DOUBLE_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText(typeStr ? typeStr : "double[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::vector<double> outv;
        if (StringToFloatArray(v, &outv)) {
          if (entry.publisher == 0) {
            entry.publisher =
                nt::Publish(entry.info.topic, NT_DOUBLE_ARRAY, "double[]");
          }
          nt::SetDoubleArray(entry.publisher, outv);
        }
      }
      break;
    }
    case NT_STRING_ARRAY: {
      char* v = GetTextBuffer(entry.valueStr);
      if (ImGui::InputText(typeStr ? typeStr : "string[]", v, kTextBufferSize,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::vector<std::string> outv;
        if (StringToStringArray(v, &outv)) {
          if (entry.publisher == 0) {
            entry.publisher =
                nt::Publish(entry.info.topic, NT_STRING_ARRAY, "string[]");
          }
          nt::SetStringArray(entry.publisher, outv);
        }
      }
      break;
    }
    case NT_RAW:
      ImGui::LabelText(typeStr ? typeStr : "raw",
                       val.GetRaw().empty() ? "[]" : "[...]");
      break;
    case NT_RPC:
      ImGui::LabelText(typeStr ? typeStr : "rpc", "[...]");
      break;
    default:
      ImGui::LabelText(typeStr ? typeStr : "other", "?");
      break;
  }
  ImGui::PopID();
}

static void CreateTopicMenuItem(NetworkTablesModel* model,
                                std::string_view path, NT_Type type,
                                const char* typeStr, bool enabled) {
  if (ImGui::MenuItem(typeStr, nullptr, false, enabled)) {
    auto entry =
        model->AddEntry(nt::GetTopic(model->GetInstance().GetHandle(), path));
    if (entry->publisher == 0) {
      entry->publisher = nt::Publish(entry->info.topic, type, typeStr);
    }
  }
}

static void EmitParentContextMenu(NetworkTablesModel* model,
                                  const std::string& path,
                                  NetworkTablesFlags flags) {
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
      auto entry = model->GetEntry(fullNewPath);
      bool exists = entry && entry->info.type != NT_Type::NT_UNASSIGNED;
      bool enabled = (flags & NetworkTablesFlags_CreateNoncanonicalKeys ||
                      nameBuffer[0] != '\0') &&
                     !exists;

      CreateTopicMenuItem(model, fullNewPath, NT_STRING, "string", enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_INTEGER, "int", enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_FLOAT, "float", enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_DOUBLE, "double", enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_BOOLEAN, "boolean", enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_STRING_ARRAY, "string[]",
                          enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_INTEGER_ARRAY, "int[]",
                          enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_FLOAT_ARRAY, "float[]",
                          enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_DOUBLE_ARRAY, "double[]",
                          enabled);
      CreateTopicMenuItem(model, fullNewPath, NT_BOOLEAN_ARRAY, "boolean[]",
                          enabled);

      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }
}

static void EmitValueName(DataSource* source, const char* name,
                          const char* path) {
  if (source) {
    ImGui::Selectable(name);
    source->EmitDrag();
  } else {
    ImGui::TextUnformatted(name);
  }
  if (ImGui::BeginPopupContextItem(path)) {
    ImGui::TextUnformatted(path);
    ImGui::EndPopup();
  }
}

static void EmitValueTree(
    const std::vector<NetworkTablesModel::EntryValueTreeNode>& children,
    NetworkTablesFlags flags) {
  for (auto&& child : children) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    EmitValueName(child.source.get(), child.name.c_str(), child.path.c_str());
    ImGui::TableNextColumn();
    if (!child.valueChildren.empty()) {
      char label[64];
      std::snprintf(label, sizeof(label),
                    child.valueChildrenMap ? "{...}##v_%s" : "[...]##v_%s",
                    child.name.c_str());
      if (TreeNodeEx(label, ImGuiTreeNodeFlags_SpanFullWidth)) {
        EmitValueTree(child.valueChildren, flags);
        TreePop();
      }
    } else {
      EmitEntryValueReadonly(child, nullptr, flags);
    }
  }
}

static void EmitEntry(NetworkTablesModel* model,
                      NetworkTablesModel::Entry& entry, const char* name,
                      NetworkTablesFlags flags, ShowCategory category) {
  if (!IsVisible(category, entry.persistent, entry.retained)) {
    return;
  }

  bool valueChildrenOpen = false;
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  EmitValueName(entry.source.get(), name, entry.info.name.c_str());

  ImGui::TableNextColumn();
  if (!entry.valueChildren.empty()) {
    auto pos = ImGui::GetCursorPos();
    char label[64];
    std::snprintf(label, sizeof(label),
                  entry.valueChildrenMap ? "{...}##v_%s" : "[...]##v_%s",
                  entry.info.name.c_str());
    valueChildrenOpen =
        TreeNodeEx(label, ImGuiTreeNodeFlags_SpanFullWidth |
                              ImGuiTreeNodeFlags_AllowItemOverlap);
    // make it look like a normal label w/type
    ImGui::SetCursorPos(pos);
    ImGui::LabelText(entry.info.type_str.c_str(), "%s", "");
  } else if (flags & NetworkTablesFlags_ReadOnly) {
    EmitEntryValueReadonly(
        entry,
        entry.info.type_str.empty() ? nullptr : entry.info.type_str.c_str(),
        flags);
  } else {
    EmitEntryValueEditable(entry, flags);
  }

  if (flags & NetworkTablesFlags_ShowProperties) {
    ImGui::TableNextColumn();
    ImGui::Text("%s", entry.info.properties.c_str());
    if (ImGui::BeginPopupContextItem(entry.info.name.c_str())) {
      if (ImGui::Checkbox("persistent", &entry.persistent)) {
        nt::SetTopicPersistent(entry.info.topic, entry.persistent);
      }
      if (ImGui::Checkbox("retained", &entry.retained)) {
        if (entry.retained) {
          nt::SetTopicProperty(entry.info.topic, "retained", true);
        } else {
          nt::DeleteTopicProperty(entry.info.topic, "retained");
        }
      }
      ImGui::EndPopup();
    }
  }

  if (flags & NetworkTablesFlags_ShowTimestamp) {
    ImGui::TableNextColumn();
    if (entry.value) {
      ImGui::Text("%f", (entry.value.last_change() * 1.0e-6) -
                            (GetZeroTime() * 1.0e-6));
    } else {
      ImGui::TextUnformatted("");
    }
  }

  if (flags & NetworkTablesFlags_ShowServerTimestamp) {
    ImGui::TableNextColumn();
    if (entry.value && entry.value.server_time() != 0) {
      if (entry.value.server_time() == 1) {
        ImGui::TextUnformatted("---");
      } else {
        ImGui::Text("%f", entry.value.server_time() * 1.0e-6);
      }
    } else {
      ImGui::TextUnformatted("");
    }
  }

  if (valueChildrenOpen) {
    EmitValueTree(entry.valueChildren, flags);
    TreePop();
  }
}

static void EmitTree(NetworkTablesModel* model,
                     const std::vector<NetworkTablesModel::TreeNode>& tree,
                     NetworkTablesFlags flags, ShowCategory category,
                     bool root) {
  for (auto&& node : tree) {
    if (root && (flags & NetworkTablesFlags_ShowSpecial) == 0 &&
        wpi::starts_with(node.name, '$')) {
      continue;
    }
    if (node.entry) {
      EmitEntry(model, *node.entry, node.name.c_str(), flags, category);
    }

    if (!node.children.empty()) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      bool open =
          TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
      EmitParentContextMenu(model, node.path, flags);
      if (open) {
        EmitTree(model, node.children, flags, category, false);
        TreePop();
      }
    }
  }
}

static void DisplayTable(NetworkTablesModel* model,
                         const std::vector<NetworkTablesModel::TreeNode>& tree,
                         NetworkTablesFlags flags, ShowCategory category) {
  if (tree.empty()) {
    return;
  }

  const bool showProperties = (flags & NetworkTablesFlags_ShowProperties);
  const bool showTimestamp = (flags & NetworkTablesFlags_ShowTimestamp);
  const bool showServerTimestamp =
      (flags & NetworkTablesFlags_ShowServerTimestamp);

  ImGui::BeginTable("values",
                    2 + (showProperties ? 1 : 0) + (showTimestamp ? 1 : 0) +
                        (showServerTimestamp ? 1 : 0),
                    ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_BordersInner);
  ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed,
                          0.35f * ImGui::GetWindowWidth());
  ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed,
                          12 * ImGui::GetFontSize());
  if (showProperties) {
    ImGui::TableSetupColumn("Properties", ImGuiTableColumnFlags_WidthFixed,
                            12 * ImGui::GetFontSize());
  }
  if (showTimestamp) {
    ImGui::TableSetupColumn("Time");
  }
  if (showServerTimestamp) {
    ImGui::TableSetupColumn("Server Time");
  }
  ImGui::TableHeadersRow();

  // EmitParentContextMenu(model, "/", flags);
  if (flags & NetworkTablesFlags_TreeView) {
    switch (category) {
      case ShowPersistent:
        PushID("persistent");
        break;
      case ShowRetained:
        PushID("retained");
        break;
      case ShowTransitory:
        PushID("transitory");
        break;
      default:
        break;
    }
    EmitTree(model, tree, flags, category, true);
    if (category != ShowAll) {
      PopID();
    }
  } else {
    for (auto entry : model->GetEntries()) {
      if ((flags & NetworkTablesFlags_ShowSpecial) != 0 ||
          !wpi::starts_with(entry->info.name, '$')) {
        EmitEntry(model, *entry, entry->info.name.c_str(), flags, category);
      }
    }
  }
  ImGui::EndTable();
}

static void DisplayClient(const NetworkTablesModel::Client& client) {
  if (CollapsingHeader("Publishers")) {
    ImGui::BeginTable("publishers", 2, ImGuiTableFlags_Resizable);
    ImGui::TableSetupColumn("UID", ImGuiTableColumnFlags_WidthFixed,
                            10 * ImGui::GetFontSize());
    ImGui::TableSetupColumn("Topic");
    ImGui::TableHeadersRow();
    for (auto&& pub : client.publishers) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%" PRId64, pub.uid);
      ImGui::TableNextColumn();
      ImGui::Text("%s", pub.topic.c_str());
    }
    ImGui::EndTable();
  }
  if (CollapsingHeader("Subscribers")) {
    ImGui::BeginTable(
        "subscribers", 6,
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);
    ImGui::TableSetupColumn("UID", ImGuiTableColumnFlags_WidthFixed,
                            10 * ImGui::GetFontSize());
    ImGui::TableSetupColumn("Topics", ImGuiTableColumnFlags_WidthStretch, 6.0f);
    ImGui::TableSetupColumn("Periodic", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableSetupColumn("Topics Only", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableSetupColumn("Send All", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableSetupColumn("Prefix Match", ImGuiTableColumnFlags_WidthStretch,
                            1.0f);
    ImGui::TableHeadersRow();
    for (auto&& sub : client.subscribers) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%" PRId64, sub.uid);
      ImGui::TableNextColumn();
      ImGui::Text("%s", sub.topicsStr.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%0.3f", sub.options.periodic);
      ImGui::TableNextColumn();
      ImGui::Text(sub.options.topicsOnly ? "Yes" : "No");
      ImGui::TableNextColumn();
      ImGui::Text(sub.options.sendAll ? "Yes" : "No");
      ImGui::TableNextColumn();
      ImGui::Text(sub.options.prefixMatch ? "Yes" : "No");
    }
    ImGui::EndTable();
  }
}

void glass::DisplayNetworkTablesInfo(NetworkTablesModel* model) {
  auto inst = model->GetInstance();

  if (CollapsingHeader("Connections")) {
    ImGui::BeginTable("connections", 4, ImGuiTableFlags_Resizable);
    ImGui::TableSetupColumn("Id");
    ImGui::TableSetupColumn("Address");
    ImGui::TableSetupColumn("Updated");
    ImGui::TableSetupColumn("Proto");
    ImGui::TableSetupScrollFreeze(1, 0);
    ImGui::TableHeadersRow();
    for (auto&& i : inst.GetConnections()) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%s", i.remote_id.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%s", i.remote_ip.c_str());
      ImGui::TableNextColumn();
      ImGui::Text("%llu",
                  static_cast<unsigned long long>(  // NOLINT(runtime/int)
                      i.last_update));
      ImGui::TableNextColumn();
      ImGui::Text("%d.%d", i.protocol_version >> 8, i.protocol_version & 0xff);
    }
    ImGui::EndTable();
  }

  auto netMode = inst.GetNetworkMode();
  if (netMode == NT_NET_MODE_SERVER || netMode == NT_NET_MODE_CLIENT4) {
    if (CollapsingHeader("Server")) {
      PushID("Server");
      ImGui::Indent();
      DisplayClient(model->GetServer());
      ImGui::Unindent();
      PopID();
    }
    if (CollapsingHeader("Clients")) {
      ImGui::Indent();
      for (auto&& client : model->GetClients()) {
        if (CollapsingHeader(client.second.id.c_str())) {
          PushID(client.second.id.c_str());
          ImGui::Indent();
          ImGui::Text("%s (version %s)", client.second.conn.c_str(),
                      client.second.version.c_str());
          DisplayClient(client.second);
          ImGui::Unindent();
          PopID();
        }
      }
      ImGui::Unindent();
    }
  }
}

void glass::DisplayNetworkTables(NetworkTablesModel* model,
                                 NetworkTablesFlags flags) {
  if (flags & NetworkTablesFlags_CombinedView) {
    DisplayTable(model, model->GetTreeRoot(), flags, ShowAll);
  } else {
    if (CollapsingHeader("Persistent Values", ImGuiTreeNodeFlags_DefaultOpen)) {
      DisplayTable(model, model->GetPersistentTreeRoot(), flags,
                   ShowPersistent);
    }

    if (CollapsingHeader("Retained Values", ImGuiTreeNodeFlags_DefaultOpen)) {
      DisplayTable(model, model->GetRetainedTreeRoot(), flags, ShowRetained);
    }

    if (CollapsingHeader("Transitory Values", ImGuiTreeNodeFlags_DefaultOpen)) {
      DisplayTable(model, model->GetTransitoryTreeRoot(), flags,
                   ShowTransitory);
    }
  }
}

void NetworkTablesFlagsSettings::Update() {
  if (!m_pTreeView) {
    auto& storage = GetStorage();
    m_pTreeView =
        &storage.GetBool("tree", m_defaultFlags & NetworkTablesFlags_TreeView);
    m_pCombinedView = &storage.GetBool(
        "combined", m_defaultFlags & NetworkTablesFlags_CombinedView);
    m_pShowSpecial = &storage.GetBool(
        "special", m_defaultFlags & NetworkTablesFlags_ShowSpecial);
    m_pShowProperties = &storage.GetBool(
        "properties", m_defaultFlags & NetworkTablesFlags_ShowProperties);
    m_pShowTimestamp = &storage.GetBool(
        "timestamp", m_defaultFlags & NetworkTablesFlags_ShowTimestamp);
    m_pShowServerTimestamp = &storage.GetBool(
        "serverTimestamp",
        m_defaultFlags & NetworkTablesFlags_ShowServerTimestamp);
    m_pCreateNoncanonicalKeys = &storage.GetBool(
        "createNonCanonical",
        m_defaultFlags & NetworkTablesFlags_CreateNoncanonicalKeys);
    m_pPrecision = &storage.GetInt(
        "precision", (m_defaultFlags & NetworkTablesFlags_Precision) >>
                         kNetworkTablesFlags_PrecisionBitShift);
  }

  m_flags &= ~(
      NetworkTablesFlags_TreeView | NetworkTablesFlags_CombinedView |
      NetworkTablesFlags_ShowSpecial | NetworkTablesFlags_ShowProperties |
      NetworkTablesFlags_ShowTimestamp |
      NetworkTablesFlags_ShowServerTimestamp |
      NetworkTablesFlags_CreateNoncanonicalKeys | NetworkTablesFlags_Precision);
  m_flags |=
      (*m_pTreeView ? NetworkTablesFlags_TreeView : 0) |
      (*m_pCombinedView ? NetworkTablesFlags_CombinedView : 0) |
      (*m_pShowSpecial ? NetworkTablesFlags_ShowSpecial : 0) |
      (*m_pShowProperties ? NetworkTablesFlags_ShowProperties : 0) |
      (*m_pShowTimestamp ? NetworkTablesFlags_ShowTimestamp : 0) |
      (*m_pShowServerTimestamp ? NetworkTablesFlags_ShowServerTimestamp : 0) |
      (*m_pCreateNoncanonicalKeys ? NetworkTablesFlags_CreateNoncanonicalKeys
                                  : 0) |
      (*m_pPrecision << kNetworkTablesFlags_PrecisionBitShift);
}

void NetworkTablesFlagsSettings::DisplayMenu() {
  if (!m_pTreeView) {
    return;
  }
  ImGui::MenuItem("Tree View", "", m_pTreeView);
  ImGui::MenuItem("Combined View", "", m_pCombinedView);
  ImGui::MenuItem("Show Special", "", m_pShowSpecial);
  ImGui::MenuItem("Show Properties", "", m_pShowProperties);
  ImGui::MenuItem("Show Timestamp", "", m_pShowTimestamp);
  ImGui::MenuItem("Show Server Timestamp", "", m_pShowServerTimestamp);
  if (ImGui::BeginMenu("Decimal Precision")) {
    static const char* precisionOptions[] = {"1", "2", "3", "4", "5",
                                             "6", "7", "8", "9", "10"};
    for (int i = 1; i <= 10; i++) {
      if (ImGui::MenuItem(precisionOptions[i - 1], nullptr,
                          i == *m_pPrecision)) {
        *m_pPrecision = i;
      }
    }
    ImGui::EndMenu();
  }
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
