// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "wpi/glass/Model.hpp"
#include "wpi/glass/Provider.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/nt/Topic.hpp"
#include "wpi/util/DenseMap.hpp"
#include "wpi/util/StringMap.hpp"

namespace wpi::glass {

class Window;

namespace detail {
struct NTProviderFunctions {
  using Exists =
      std::function<bool(wpi::nt::NetworkTableInstance inst, const char* path)>;
  using CreateModel = std::function<std::unique_ptr<Model>(
      wpi::nt::NetworkTableInstance inst, const char* path)>;
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
  NetworkTablesProvider(Storage& storage, wpi::nt::NetworkTableInstance inst);

  /**
   * Get the NetworkTables instance being used for this provider.
   *
   * @return NetworkTables instance
   */
  wpi::nt::NetworkTableInstance GetInstance() const { return m_inst; }

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

  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::NetworkTableListenerPoller m_poller;
  NT_Listener m_listener{0};

  // cached mapping from table name to type string
  Storage& m_typeCache;

  struct Builder {
    CreateModelFunc createModel;
    CreateViewFunc createView;
  };

  // mapping from .type string to model/view creators
  wpi::util::StringMap<Builder> m_typeMap;

  struct SubListener {
    wpi::nt::StringSubscriber subscriber;
    NT_Listener listener;
  };

  // mapping from .type topic to subscriber/listener
  wpi::util::DenseMap<NT_Topic, SubListener> m_topicMap;

  struct Entry : public ModelEntry {
    Entry(wpi::nt::Topic typeTopic, std::string_view name,
          const Builder& builder)
        : ModelEntry{name, [](auto, const char*) { return true; },
                     builder.createModel},
          typeTopic{typeTopic} {}
    wpi::nt::Topic typeTopic;
  };

  void Show(ViewEntry* entry, Window* window) override;

  ViewEntry* GetOrCreateView(const Builder& builder, wpi::nt::Topic typeTopic,
                             std::string_view name);
};

/**
 * Add "standard" set of NetworkTables models/views.
 *
 * @param provider NetworkTables provider
 */
void AddStandardNetworkTablesViews(NetworkTablesProvider& provider);

}  // namespace wpi::glass
