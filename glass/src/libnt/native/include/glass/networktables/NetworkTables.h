// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/DenseMap.h>

#include "glass/Model.h"
#include "glass/View.h"

namespace glass {

class DataSource;

class NetworkTablesModel : public Model {
 public:
  struct Entry {
    explicit Entry(nt::EntryNotification&& event);

    void UpdateValue();

    /** Entry handle. */
    NT_Entry entry;

    /** Entry name. */
    std::string name;

    /** The value. */
    std::shared_ptr<nt::Value> value;

    /** Flags. */
    unsigned int flags = 0;

    /** String representation of the value (for arrays / complex values). */
    std::string valueStr;

    /** Data source (for numeric values). */
    std::unique_ptr<DataSource> source;
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

  NetworkTablesModel();
  explicit NetworkTablesModel(NT_Inst inst);
  ~NetworkTablesModel() override;

  void Update() override;
  bool Exists() override;

  NT_Inst GetInstance() { return m_inst; }
  const std::vector<Entry*>& GetEntries() { return m_sortedEntries; }
  const std::vector<TreeNode>& GetTreeRoot() { return m_root; }

 private:
  NT_Inst m_inst;
  NT_EntryListenerPoller m_poller;
  wpi::DenseMap<NT_Entry, std::unique_ptr<Entry>> m_entries;

  // sorted by name
  std::vector<Entry*> m_sortedEntries;

  std::vector<TreeNode> m_root;
};

using NetworkTablesFlags = int;

enum NetworkTablesFlags_ {
  NetworkTablesFlags_TreeView = 1 << 0,
  NetworkTablesFlags_ReadOnly = 1 << 1,
  NetworkTablesFlags_ShowConnections = 1 << 2,
  NetworkTablesFlags_ShowFlags = 1 << 3,
  NetworkTablesFlags_ShowTimestamp = 1 << 4,
  NetworkTablesFlags_CreateNoncanonicalKeys = 1 << 5,
  NetworkTablesFlags_Precision = 0xff << 6,
  NetworkTablesFlags_Default = (1 & ~NetworkTablesFlags_ReadOnly &
                                ~NetworkTablesFlags_CreateNoncanonicalKeys) |
                               (6 << 6),
};

void DisplayNetworkTables(
    NetworkTablesModel* model,
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
  bool* m_pShowConnections = nullptr;
  bool* m_pShowFlags = nullptr;
  bool* m_pShowTimestamp = nullptr;
  bool* m_pCreateNoncanonicalKeys = nullptr;
  unsigned char m_precision;
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

 private:
  NetworkTablesModel* m_model;
  NetworkTablesFlagsSettings m_flags;
};

}  // namespace glass
