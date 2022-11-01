// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableListener.h>
#include <networktables/StringTopic.h>
#include <networktables/Topic.h>
#include <wpi/DenseMap.h>
#include <wpi/StringMap.h>

#include "glass/Model.h"
#include "glass/Provider.h"

namespace glass {

class Window;

namespace detail {
struct NTProviderFunctions {
  using Exists =
      std::function<bool(nt::NetworkTableInstance inst, const char* path)>;
  using CreateModel = std::function<std::unique_ptr<Model>(
      nt::NetworkTableInstance inst, const char* path)>;
  using ViewExists = std::function<bool(Model*, const char* path)>;
  using CreateView =
      std::function<std::unique_ptr<View>(Window*, Model*, const char* path)>;
};
}  // namespace detail

/**
 * A provider for NetworkTables (SmartDashboard style) models and views.
 */
class NetworkTablesProvider : private Provider<detail::NTProviderFunctions> {
 public:
  using Provider::CreateModelFunc;
  using Provider::CreateViewFunc;

  explicit NetworkTablesProvider(Storage& storage);
  NetworkTablesProvider(Storage& storage, nt::NetworkTableInstance inst);

  /**
   * Get the NetworkTables instance being used for this provider.
   *
   * @return NetworkTables instance
   */
  nt::NetworkTableInstance GetInstance() const { return m_inst; }

  /**
   * Perform global initialization.  This should be called prior to
   * wpi::gui::Initialize().
   */
  void GlobalInit() override { Provider::GlobalInit(); }

  /**
   * Displays menu contents as a tree of available NetworkTables views.
   */
  void DisplayMenu() override;

  /**
   * Registers a NetworkTables model and view.
   *
   * @param typeName SmartDashboard .type value to match
   * @param createModel functor to create model
   * @param createView functor to create view
   */
  void Register(std::string_view typeName, CreateModelFunc createModel,
                CreateViewFunc createView);

 private:
  void Update() override;

  nt::NetworkTableInstance m_inst;
  nt::NetworkTableListenerPoller m_poller;
  NT_Listener m_listener{0};

  // cached mapping from table name to type string
  Storage& m_typeCache;

  struct Builder {
    CreateModelFunc createModel;
    CreateViewFunc createView;
  };

  // mapping from .type string to model/view creators
  wpi::StringMap<Builder> m_typeMap;

  struct SubListener {
    nt::StringSubscriber subscriber;
    NT_Listener listener;
  };

  // mapping from .type topic to subscriber/listener
  wpi::DenseMap<NT_Topic, SubListener> m_topicMap;

  struct Entry : public ModelEntry {
    Entry(nt::Topic typeTopic, std::string_view name, const Builder& builder)
        : ModelEntry{name, [](auto, const char*) { return true; },
                     builder.createModel},
          typeTopic{typeTopic} {}
    nt::Topic typeTopic;
  };

  void Show(ViewEntry* entry, Window* window) override;

  ViewEntry* GetOrCreateView(const Builder& builder, nt::Topic typeTopic,
                             std::string_view name);
};

/**
 * Add "standard" set of NetworkTables models/views.
 *
 * @param provider NetworkTables provider
 */
void AddStandardNetworkTablesViews(NetworkTablesProvider& provider);

}  // namespace glass
