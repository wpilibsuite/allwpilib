/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/StringMap.h>
#include <wpi/StringRef.h>
#include <wpi/Twine.h>

#include "glass/Model.h"
#include "glass/Provider.h"
#include "glass/networktables/NetworkTablesHelper.h"
#include "glass/support/IniSaverInfo.h"
#include "glass/support/IniSaverString.h"

namespace glass {

class Window;

namespace detail {
struct NTProviderFunctions {
  using Exists = std::function<bool(NT_Inst inst, const char* path)>;
  using CreateModel =
      std::function<std::unique_ptr<Model>(NT_Inst inst, const char* path)>;
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

  explicit NetworkTablesProvider(const wpi::Twine& iniName);
  NetworkTablesProvider(const wpi::Twine& iniName, NT_Inst inst);

  /**
   * Get the NetworkTables instance being used for this provider.
   *
   * @return NetworkTables instance
   */
  NT_Inst GetInstance() const { return m_nt.GetInstance(); }

  /**
   * Perform global initialization.  This should be called prior to
   * wpi::gui::Initialize().
   */
  void GlobalInit() override;

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
  void Register(wpi::StringRef typeName, CreateModelFunc createModel,
                CreateViewFunc createView);

  using WindowManager::AddWindow;

 private:
  void Update() override;

  NetworkTablesHelper m_nt;

  // cached mapping from table name to type string
  IniSaverString<NameInfo> m_typeCache;

  struct Builder {
    CreateModelFunc createModel;
    CreateViewFunc createView;
  };

  // mapping from .type string to model/view creators
  wpi::StringMap<Builder> m_typeMap;

  struct Entry : public ModelEntry {
    Entry(NT_Entry typeEntry, wpi::StringRef name, const Builder& builder)
        : ModelEntry{name, [](NT_Inst, const char*) { return true; },
                     builder.createModel},
          typeEntry{typeEntry} {}
    NT_Entry typeEntry;
  };

  void Show(ViewEntry* entry, Window* window) override;

  ViewEntry* GetOrCreateView(const Builder& builder, NT_Entry typeEntry,
                             wpi::StringRef name);
};

/**
 * Add "standard" set of NetworkTables models/views.
 *
 * @param provider NetworkTables provider
 */
void AddStandardNetworkTablesViews(NetworkTablesProvider& provider);

}  // namespace glass
