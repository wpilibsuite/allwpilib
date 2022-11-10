
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "DataLog.h"
#include <wpi/StringMap.h>

#include "glass/Model.h"
#include "glass/View.h"
#include "glass/Window.h"
#include "glass/WindowManager.h"
#include "glass/Provider.h"

namespace sapphire{

namespace detail{
  struct Functions{
    using Exists = std::function<bool(glass::Model*, const char*)>;
    using CreateModel = std::function<std::unique_ptr<glass::Model>(DataLogModel& model, const char* path)>;
    using CreateView = std::function<std::unique_ptr<glass::View>(glass::Window*, glass::Model* model, const char* path)>;
    using ViewExists = std::function<std::unique_ptr<glass::View>(glass::Window*, glass::Model*)>;
  };
}

// Loads all Model Views from a log file
class LogModelViewLoader : public glass::WindowManager {
  using Provider = glass::Provider<detail::Functions>;
 public:
 using CreateModelFunc = detail::Functions::CreateModel;
 using CreateViewFunc = detail::Functions::CreateView;
 using glass::WindowManager::GlobalInit;

  LogModelViewLoader(glass::Storage& storage, float& nowRef) :glass::WindowManager{storage}, nowRef{nowRef} {
    wpi::gui::AddEarlyExecute([this] {Update(); });
  }; 

  void Register(std::string_view typeName, CreateModelFunc createModel,
                CreateViewFunc createView);

  void AddDataLog(DataLogModel& model);

  float& nowRef;

 private:
  void Update();

  struct Builder{
    CreateModelFunc createModel;
    CreateViewFunc createView;
  };

  struct ModelEntry {
    ModelEntry(std::string_view name,
               CreateModelFunc createModel)
        : name{name},
          createModel{std::move(createModel)} {}
    virtual ~ModelEntry() = default;

    std::string name;
    CreateModelFunc createModel;
    std::unique_ptr<glass::Model> model;
  };

  struct ViewEntry {
    ViewEntry(std::string_view name, ModelEntry* modelEntry, CreateViewFunc createView)
        : name{name},
          modelEntry{modelEntry},
          createView{std::move(createView)} {}
    virtual ~ViewEntry() = default;

    std::string name;
    ModelEntry* modelEntry;
    CreateViewFunc createView;
    bool showDefault = false;
    glass::Window* window = nullptr;
  };

  class DataLogProvider{
   public:
    DataLogProvider(LogModelViewLoader* loader, std::string_view name, std::shared_ptr<DataLogModel> log, wpi::StringMap<Builder>& typeMap);
    std::string_view name;
    std::shared_ptr<DataLogModel> log;
    wpi::StringMap<Builder>& m_typeMap;
    std::vector<std::unique_ptr<ModelEntry> > m_models;
    std::vector<std::unique_ptr<ViewEntry> > m_views;
    LogModelViewLoader* loader;
    void Show(ViewEntry* entry, glass::Window* window);
    void Update();
  };
  wpi::StringMap<std::shared_ptr<DataLogProvider> > m_logMap;

  wpi::StringMap<Builder> m_typeMap;
};

void RegisterLogModels(LogModelViewLoader& provider);

}
