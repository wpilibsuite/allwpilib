// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableListener.h>
#include <ntcore_cpp.h>
#include <wpi/DenseMap.h>
#include <wpi/json.h>
#include <wpi/struct/DynamicStruct.h>

#ifndef NO_PROTOBUF
#include <wpi/protobuf/ProtobufMessageDatabase.h>
#endif

#include "glass/Model.h"
#include "glass/View.h"

namespace glass {

class DataSource;

class NetworkTablesModel : public Model {
 public:
  struct EntryValueTreeNode;

  struct ValueSource {
    void UpdateFromValue(NetworkTablesModel& model, std::string_view name,
                         std::string_view typeStr);

    /** The latest value. */
    nt::Value value;

    /** String representation of the value (for arrays / complex values). */
    std::string valueStr;

    /** Data type */
    std::string typeStr;

    /** Data source (for numeric values). */
    std::unique_ptr<DataSource> source;

    /** Children of this node, sorted by name/index */
    std::vector<EntryValueTreeNode> valueChildren;

    /** Whether or not the children represent a map */
    bool valueChildrenMap = false;

   private:
    void UpdateDiscreteSource(std::string_view name, double value, int64_t time,
                              bool digital = false);

    template <typename T, typename MakeValue>
    void UpdateDiscreteArray(std::string_view name, std::span<const T> arr,
                             int64_t time, MakeValue makeValue,
                             bool digital = false);
  };

  struct EntryValueTreeNode : public ValueSource {
    /** Short name (e.g. of just this node) */
    std::string name;

    /** Full path */
    std::string path;
  };

  struct Entry : public ValueSource {
    Entry() = default;
    Entry(const Entry&) = delete;
    Entry& operator=(const Entry&) = delete;
    ~Entry();

    void UpdateFromValue(NetworkTablesModel& model) {
      ValueSource::UpdateFromValue(model, info.name, info.type_str);
    }

    void UpdateTopic(nt::Event&& event) {
      if (std::holds_alternative<nt::TopicInfo>(event.data)) {
        UpdateInfo(std::get<nt::TopicInfo>(std::move(event.data)));
      }
    }
    void UpdateInfo(nt::TopicInfo&& info_);

    /** Topic information. */
    nt::TopicInfo info;

    /** JSON representation of the topic properties. */
    wpi::json properties;

    /** Specific common property flags. */
    bool persistent{false};
    bool retained{false};

    /** Publisher (created when the value changes). */
    NT_Publisher publisher{0};

    std::vector<nt::meta::TopicPublisher> publishers;
    std::vector<nt::meta::TopicSubscriber> subscribers;
  };

  struct TreeNode {
    explicit TreeNode(std::string_view name) : name{name} {}

    /** Short name (e.g. of just this node) */
    std::string name;

    /**
     * Full path if entry is null (otherwise use entry->name),
     * excluding trailing /
     */
    std::string path;

    /** Null if no value at this node */
    Entry* entry = nullptr;

    /** Children of node, sorted by name */
    std::vector<TreeNode> children;
  };

  struct Client : public nt::meta::Client {
    Client() = default;
    /*implicit*/ Client(nt::meta::Client&& oth)  // NOLINT
        : nt::meta::Client{std::move(oth)} {}

    struct Subscriber : public nt::meta::ClientSubscriber {
      /*implicit*/ Subscriber(nt::meta::ClientSubscriber&& oth);  // NOLINT
      std::string topicsStr;
    };

    std::vector<nt::meta::ClientPublisher> publishers;
    std::vector<Subscriber> subscribers;

    void UpdatePublishers(std::span<const uint8_t> data);
    void UpdateSubscribers(std::span<const uint8_t> data);
  };

  NetworkTablesModel();
  explicit NetworkTablesModel(nt::NetworkTableInstance inst);

  void Update() override;
  bool Exists() override;

  nt::NetworkTableInstance GetInstance() { return m_inst; }
  const std::vector<Entry*>& GetEntries() const { return m_sortedEntries; }
  const std::vector<TreeNode>& GetTreeRoot() const { return m_root; }
  const std::vector<TreeNode>& GetPersistentTreeRoot() const {
    return m_persistentRoot;
  }
  const std::vector<TreeNode>& GetRetainedTreeRoot() const {
    return m_retainedRoot;
  }
  const std::vector<TreeNode>& GetTransitoryTreeRoot() const {
    return m_transitoryRoot;
  }
  const std::map<std::string, Client, std::less<>>& GetClients() const {
    return m_clients;
  }
  const Client& GetServer() const { return m_server; }
  Entry* GetEntry(std::string_view name);
  Entry* AddEntry(NT_Topic topic);

  wpi::StructDescriptorDatabase& GetStructDatabase() { return m_structDb; }
#ifndef NO_PROTOBUF
  wpi::ProtobufMessageDatabase& GetProtobufDatabase() { return m_protoDb; }
#endif

 private:
  void RebuildTree();
  void RebuildTreeImpl(std::vector<TreeNode>* tree, int category);
  void UpdateClients(std::span<const uint8_t> data);

  nt::NetworkTableInstance m_inst;
  nt::NetworkTableListenerPoller m_poller;
  wpi::DenseMap<NT_Topic, std::unique_ptr<Entry>> m_entries;

  // sorted by name
  std::vector<Entry*> m_sortedEntries;

  std::vector<TreeNode> m_root;
  std::vector<TreeNode> m_persistentRoot;
  std::vector<TreeNode> m_retainedRoot;
  std::vector<TreeNode> m_transitoryRoot;

  std::map<std::string, Client, std::less<>> m_clients;
  Client m_server;

  wpi::StructDescriptorDatabase m_structDb;
#ifndef NO_PROTOBUF
  wpi::ProtobufMessageDatabase m_protoDb;
#endif
};

using NetworkTablesFlags = int;

static constexpr const int kNetworkTablesFlags_PrecisionBitShift = 9;

enum NetworkTablesFlags_ {
  NetworkTablesFlags_TreeView = 1 << 0,
  NetworkTablesFlags_CombinedView = 1 << 1,
  NetworkTablesFlags_ReadOnly = 1 << 2,
  NetworkTablesFlags_ShowSpecial = 1 << 3,
  NetworkTablesFlags_ShowProperties = 1 << 4,
  NetworkTablesFlags_ShowTimestamp = 1 << 5,
  NetworkTablesFlags_ShowServerTimestamp = 1 << 6,
  NetworkTablesFlags_CreateNoncanonicalKeys = 1 << 7,
  NetworkTablesFlags_Precision = 0xff << kNetworkTablesFlags_PrecisionBitShift,
  NetworkTablesFlags_Default = NetworkTablesFlags_TreeView |
                               (6 << kNetworkTablesFlags_PrecisionBitShift),
};

void DisplayNetworkTablesInfo(NetworkTablesModel* model);

void DisplayNetworkTables(
    NetworkTablesModel* model,
    NetworkTablesFlags flags = NetworkTablesFlags_Default);

void DisplayNetworkTablesAddMenu(
    NetworkTablesModel* model, std::string_view path = {},
    NetworkTablesFlags flags = NetworkTablesFlags_Default);

class NetworkTablesFlagsSettings {
 public:
  explicit NetworkTablesFlagsSettings(
      NetworkTablesFlags defaultFlags = NetworkTablesFlags_Default)
      : m_defaultFlags{defaultFlags}, m_flags{defaultFlags} {}

  void Update();
  void DisplayMenu();

  NetworkTablesFlags GetFlags() const { return m_flags; }

 private:
  bool* m_pTreeView = nullptr;
  bool* m_pCombinedView = nullptr;
  bool* m_pShowSpecial = nullptr;
  bool* m_pShowProperties = nullptr;
  bool* m_pShowTimestamp = nullptr;
  bool* m_pShowServerTimestamp = nullptr;
  bool* m_pCreateNoncanonicalKeys = nullptr;
  int* m_pPrecision = nullptr;
  NetworkTablesFlags m_defaultFlags;  // NOLINT
  NetworkTablesFlags m_flags;         // NOLINT
};

class NetworkTablesView : public View {
 public:
  explicit NetworkTablesView(
      NetworkTablesModel* model,
      NetworkTablesFlags defaultFlags = NetworkTablesFlags_Default)
      : m_model{model}, m_flags{defaultFlags} {}

  void Display() override;
  void Settings() override;
  bool HasSettings() override;

 private:
  NetworkTablesModel* m_model;
  NetworkTablesFlagsSettings m_flags;
};

}  // namespace glass
