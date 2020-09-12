/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/StringRef.h>
#include <wpi/Twine.h>
#include <wpigui.h>

#include "glass/Model.h"
#include "glass/WindowManager.h"

namespace glass {

namespace detail {
struct ProviderFunctions {
  using Exists = std::function<bool()>;
  using CreateModel = std::function<std::unique_ptr<Model>()>;
  using ViewExists = std::function<bool(Model*)>;
  using CreateView = std::function<std::unique_ptr<View>(Window*, Model*)>;
};
}  // namespace detail

/**
 * Providers are registries of models and views.  They have ownership over
 * their created Models, Windows, and Views.
 *
 * GlobalInit() configures Update() to be called during EarlyExecute.
 * Calling Update() calls Update() on all created models (Provider
 * implementations must ensure this occurs).
 *
 * @tparam Functions defines functor interface types
 */
template <typename Functions = detail::ProviderFunctions>
class Provider : public WindowManager {
 public:
  using ExistsFunc = typename Functions::Exists;
  using CreateModelFunc = typename Functions::CreateModel;
  using ViewExistsFunc = typename Functions::ViewExists;
  using CreateViewFunc = typename Functions::CreateView;

  /**
   * Constructor.
   *
   * @param iniName Group name to use in ini file
   */
  explicit Provider(const wpi::Twine& iniName) : WindowManager{iniName} {}

  Provider(const Provider&) = delete;
  Provider& operator=(const Provider&) = delete;

  /**
   * Perform global initialization.  This should be called prior to
   * wpi::gui::Initialize().
   */
  void GlobalInit() override;

  /**
   * Show the specified view by default on first load.  Has no effect if
   * the user previously hid the window (e.g. in a saved prior execution).
   *
   * @param name View name
   */
  void ShowDefault(wpi::StringRef name);

  /**
   * Register a model and view combination.  Equivalent to calling both
   * RegisterModel() and RegisterView() with no ViewExistsFunc.
   *
   * @param name View/model name
   * @param exists Functor, returns true if model can be created
   * @param createModel Functor for creating model
   * @param createView Functor for creating view
   */
  void Register(wpi::StringRef name, ExistsFunc exists,
                CreateModelFunc createModel, CreateViewFunc createView);

  /**
   * Register a model.
   *
   * @param name Model name
   * @param exists Functor, returns true if model can be created
   * @param createModel Functor for creating model
   */
  void RegisterModel(wpi::StringRef name, ExistsFunc exists,
                     CreateModelFunc createModel);

  /**
   * Register a view.
   *
   * @param name View name
   * @param modelName Model name
   * @param exists Functor, returns true if view can be created
   * @param createView Functor for creating view
   */
  void RegisterView(wpi::StringRef name, wpi::StringRef modelName,
                    ViewExistsFunc exists, CreateViewFunc createView);

 protected:
  virtual void Update();

  struct ModelEntry {
    ModelEntry(wpi::StringRef name, ExistsFunc exists,
               CreateModelFunc createModel)
        : name{name},
          exists{std::move(exists)},
          createModel{std::move(createModel)} {}
    virtual ~ModelEntry() = default;

    std::string name;
    ExistsFunc exists;
    CreateModelFunc createModel;
    std::unique_ptr<Model> model;
  };

  struct ViewEntry {
    ViewEntry(wpi::StringRef name, ModelEntry* modelEntry,
              ViewExistsFunc exists, CreateViewFunc createView)
        : name{name},
          modelEntry{modelEntry},
          exists{std::move(exists)},
          createView{std::move(createView)} {}
    virtual ~ViewEntry() = default;

    std::string name;
    ModelEntry* modelEntry;
    ViewExistsFunc exists;
    CreateViewFunc createView;
    Window* window = nullptr;
  };

  // sorted by name
  using ModelEntries = std::vector<std::unique_ptr<ModelEntry>>;
  ModelEntries m_modelEntries;
  using ViewEntries = std::vector<std::unique_ptr<ViewEntry>>;
  ViewEntries m_viewEntries;

  typename ModelEntries::iterator FindModelEntry(wpi::StringRef name);
  typename ViewEntries::iterator FindViewEntry(wpi::StringRef name);

  virtual std::unique_ptr<ModelEntry> MakeModelEntry(
      wpi::StringRef name, ExistsFunc exists, CreateModelFunc createModel) {
    return std::make_unique<ModelEntry>(name, std::move(exists),
                                        std::move(createModel));
  }

  virtual std::unique_ptr<ViewEntry> MakeViewEntry(wpi::StringRef name,
                                                   ModelEntry* modelEntry,
                                                   ViewExistsFunc exists,
                                                   CreateViewFunc createView) {
    return std::make_unique<ViewEntry>(name, modelEntry, std::move(exists),
                                       std::move(createView));
  }

  virtual void Show(ViewEntry* entry, Window* window) = 0;
};

}  // namespace glass

#include "Provider.inc"
