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
#include "glass/Storage.h"

namespace glass {

class Window;

/**
 * NetworkTables (SmartDashboard style) windows.
 */
class NetworkTablesProvider {
 public:
  using CreateModelFunc = std::function<std::unique_ptr<Model>(
      nt::NetworkTableInstance inst, const char* path)>;
  using SetupWindowFunc = std::function<void(Window*)>;
  using DisplayFunc = std::function<void(Model*)>;

  explicit NetworkTablesProvider(Storage& storage);
  NetworkTablesProvider(Storage& storage, nt::NetworkTableInstance inst);

  /**
   * Get the NetworkTables instance being used for this provider.
   *
   * @return NetworkTables instance
   */
  nt::NetworkTableInstance GetInstance() const { return m_inst; }

  /**
   * Displays all windows.
   */
  void Display();

  /**
   * Displays menu contents as a tree of available NetworkTables views.
   */
  void DisplayMenu();

  /**
   * Registers a NetworkTables model and display.
   *
   * @param typeName SmartDashboard .type value to match
   * @param createModel functor to create model
   * @param setupWindow functor to set up window
   * @param display functor to display window content
   */
  void Register(std::string_view typeName, CreateModelFunc createModel,
                SetupWindowFunc setupWindow, DisplayFunc display);

  void Update();

 private:
  Storage& m_windows;

  nt::NetworkTableInstance m_inst;
  nt::NetworkTableListenerPoller m_poller;
  NT_Listener m_listener{0};

  // cached mapping from table name to type string
  Storage& m_typeCache;

  struct Builder {
    CreateModelFunc createModel;
    SetupWindowFunc setupWindow;
    DisplayFunc display;
  };

  // mapping from .type string to builders
  wpi::StringMap<Builder> m_typeMap;

  struct SubListener {
    nt::StringSubscriber subscriber;
    NT_Listener listener;
  };

  // mapping from .type topic to subscriber/listener
  wpi::DenseMap<NT_Topic, SubListener> m_topicMap;

  struct Entry {
    explicit Entry(Storage& storage) : typeString{storage.GetString("type")} {}

    std::string& typeString;
    std::unique_ptr<Model> model;
    std::unique_ptr<Window> window;
  };
  // active windows
#if 0
  ViewEntry* GetOrCreateView(const Builder& builder, nt::Topic typeTopic,
                             std::string_view name);
#endif
};

/**
 * Add "standard" set of NetworkTables models/views.
 *
 * @param provider NetworkTables provider
 */
void AddStandardNetworkTablesViews(NetworkTablesProvider& provider);

}  // namespace glass
