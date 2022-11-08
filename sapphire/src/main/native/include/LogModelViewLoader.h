
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

namespace sapphire{


// Loads all Model Views from a log file
class LogModelViewLoader : glass::WindowManager {
 public:
  using CreateModelFunc = std::function<std::unique_ptr<glass::Model>(DataLogModel& model, const char* path)>;
  using CreateViewFunc = std::function<std::unique_ptr<glass::View>(glass::Window*, glass::Model* model, const char* path)>;
  using glass::WindowManager::GlobalInit;

  void Register(std::string_view typeName, CreateModelFunc createModel,
                CreateViewFunc createView);

  void AddDataLog(DataLogModel& model);

 private:
  // std::vector<DataLogModel&> model;
  glass::Storage& m_typeCache;

  struct Builder{
    CreateModelFunc createModel;
    CreateViewFunc createView;
  };

  wpi::StringMap<Builder> m_typeMap;

};

}