#include "LogViewLoader.h"
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include "LogField2DModel.h"

using namespace sapphire;



LogViewLoader::DataLogProvider::DataLogProvider(LogViewLoader* loader,
                                    std::string_view name,
                                    DataLogModel* log, 
                                    wpi::StringMap<Builder>& typeMap) 
                                    : loader{loader},
                                    name{name},
                                    log{log}, 
                                    m_typeMap{typeMap} {
    // Parse all entries to find models
    for(auto& [i, entry] : log->m_entries){
        if(!wpi::ends_with(entry->GetName(), "/.type") || !(entry->type == "string") ){
            continue;
        }
        wpi::log::DataLogRecord record;
        record = entry->GetNextRecord(0);
        if(record.GetEntry() == -1){
            continue;
        }
        std::string_view typeName;
        record.GetString(&typeName);
        auto builderIt = m_typeMap.find(typeName);
        if(builderIt == m_typeMap.end()){
            continue;
        }

        std::string path = std::string(wpi::rsplit(entry->GetName(), '/').first);
        
        m_models.emplace_back(
                std::make_unique<ModelEntry>(
                    path, 
                    builderIt->second.createModel));
        
        auto& view = m_views.emplace_back(
                std::make_unique<ViewEntry>(
                    path,  m_models.back().get(),
                    builderIt->second.createView));

        Show(view.get(), nullptr);
    }

};
void LogViewLoader::Register(std::string_view typeName, CreateModelFunc createModel,
                CreateViewFunc createView){
    m_typeMap[typeName] = Builder{std::move(createModel), std::move(createView)};
}

void LogViewLoader::AddDataLog(DataLogModel& log){
    m_logMap.try_emplace(log.filename, std::make_shared<LogViewLoader::DataLogProvider>(this, log.filename, &log, m_typeMap));
}

void LogViewLoader::DataLogProvider::Show(ViewEntry* entry, glass::Window* window){
    // if there's already a window, just show it
    if (entry->window) {
        entry->window->SetVisible(true);
        return;
    }

    // get or create model
    if (!entry->modelEntry->model) {
        entry->modelEntry->model =
            entry->modelEntry->createModel(*log, entry->name.c_str());
    }
    if (!entry->modelEntry->model) {
        return;
    }

    // the window might exist and we're just not associated to it yet
    if (!window) {
        window = loader->GetOrAddWindow(std::string{name} + ":" + entry->name, true, glass::Window::kHide);
    }
    if (!window) {
        return;
    }
    if (wpi::starts_with(entry->name, "/SmartDashboard/")) {
        window->SetDefaultName(
            fmt::format("{} (SmartDashboard)", wpi::drop_front(entry->name, 16)));
    }
    entry->window = window;

    // create view
    auto view = entry->createView(window, entry->modelEntry->model.get(),
                                entry->name.c_str());
    if (!view) {
        return;
    }
    window->SetView(std::move(view));

    entry->window->SetVisible(true);

}
void LogViewLoader::DataLogProvider::Update(){
    for(auto&& entry : m_models){
        entry->model->Update();
    }
}

void LogViewLoader::Update(){
    for(auto& log : m_logMap ){
        log.second->Update();
    }
}

void LogViewLoader::DisplayMenu() {
    if(ImGui::BeginMenu("Views")){
        for(auto& log : m_logMap){
            if(ImGui::BeginMenu(log.first().data())){
                for(auto& entry : log.second->m_views){
                    entry->window->DisplayMenuItem();
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenu();
    }
}
void sapphire::RegisterLogModels(LogViewLoader& provider){
    provider.Register(
            LogField2DModel::kType,
            [&provider](DataLogModel& log, const char* path){
                return std::make_unique<LogField2DModel>(log, path, provider.nowRef);
            },
            [](glass::Window* win, glass::Model* model, const char*) {
                win->SetDefaultSize(400, 200);
                win->SetDefaultSize(400, 200);
                win->SetPadding(0, 0);
                return std::make_unique<glass::Field2DView>(
                        static_cast<LogField2DModel*>(model));
            });
}

