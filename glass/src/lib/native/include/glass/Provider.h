// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpigui.h>

#include "glass/Model.h"
#include "glass/WindowManager.h"

namespace glass {

class Storage;

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
   * @param storage Storage
   */
  explicit Provider(Storage& storage) : WindowManager{storage} {}

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
  void ShowDefault(std::string_view name);

  /**
   * Register a model and view combination.  Equivalent to calling both
   * RegisterModel() and RegisterView() with no ViewExistsFunc.
   *
   * @param name View/model name
   * @param exists Functor, returns true if model can be created
   * @param createModel Functor for creating model
   * @param createView Functor for creating view
   */
  void Register(std::string_view name, ExistsFunc exists,
                CreateModelFunc createModel, CreateViewFunc createView);

  /**
   * Register a model.
   *
   * @param name Model name
   * @param exists Functor, returns true if model can be created
   * @param createModel Functor for creating model
   */
  void RegisterModel(std::string_view name, ExistsFunc exists,
                     CreateModelFunc createModel);

  /**
   * Register a view.
   *
   * @param name View name
   * @param modelName Model name
   * @param exists Functor, returns true if view can be created
   * @param createView Functor for creating view
   */
  void RegisterView(std::string_view name, std::string_view modelName,
                    ViewExistsFunc exists, CreateViewFunc createView);

 protected:
  virtual void Update();

  struct ModelEntry {
    ModelEntry(std::string_view name, ExistsFunc exists,
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
    ViewEntry(std::string_view name, ModelEntry* modelEntry,
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
    bool showDefault = false;
    Window* window = nullptr;
  };

  // sorted by name
  using ModelEntries = std::vector<std::unique_ptr<ModelEntry>>;
  ModelEntries m_modelEntries;
  using ViewEntries = std::vector<std::unique_ptr<ViewEntry>>;
  ViewEntries m_viewEntries;

  typename ModelEntries::iterator FindModelEntry(std::string_view name);
  typename ViewEntries::iterator FindViewEntry(std::string_view name);

  virtual std::unique_ptr<ModelEntry> MakeModelEntry(
      std::string_view name, ExistsFunc exists, CreateModelFunc createModel) {
    return std::make_unique<ModelEntry>(name, std::move(exists),
                                        std::move(createModel));
  }

  virtual std::unique_ptr<ViewEntry> MakeViewEntry(std::string_view name,
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
