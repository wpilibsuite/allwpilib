#include "Plot.h"
#include "DataLog.h"

#include <imgui.h>
#include <imgui_stdlib.h>


#include "fmt/format.h"

#include <wpi/timestamp.h>
using namespace sapphire;

double GetValue(EntryData& data, wpi::log::DataLogRecord record){
    if(data.m_type == "double"){
        double val;
        if (record.GetDouble(&val)) {
            return val;
        }
    } else if(data.m_type == "int64"){
        int64_t val;
        if (record.GetInteger(&val)) {
            return (double)val;
        }
    }
    return 0;
}

void EntryPlot::CreatePlot(PlotAxis& axis, int startts, int endts, float sampleRate){
    m_points.clear();

    auto begin = m_entry->GetIterator(startts);
    auto end = m_entry->GetIterator(endts);
    auto it = begin;
    double& min = axis.min;
    double& max = axis.max;
    double lastTimestamp = endts;
    double lastValue = 0;
    while(it != end){
        auto record = it->second;
        auto value = GetValue(*m_entry, record);
        if(axis.autofit){
            if(value <= min){ 
                min = value; 
                min -= abs(max-min)/10;
                max += abs(max-min)/10;
            }
            if(value >= max){ 
                max = value + (value/10);
                min -= abs(max-min)/10;
                max += abs(max-min)/10;
            }
        }
        double timestamp = it->first*1e-6;
        if(timestamp - lastTimestamp > sampleRate){ // if we have not changed for a significant amount of time
            m_points.emplace_back(ImPlotPoint{timestamp-sampleRate - m_offset, lastValue});
        }
        m_points.emplace_back(ImPlotPoint{timestamp - m_offset, value});
        ++it;
        lastValue = value;
        lastTimestamp = timestamp;
    }
}

void EntryPlot::EmitPlot(Plot& view){
    
    struct GetterData {
        double now;
        double zeroTime;
        std::vector<ImPlotPoint>& data;
    };
    GetterData getterData = {view.m_now, 0, m_points};
    auto getter = [](int idx, void* data) {
        auto d = static_cast<GetterData*>(data);
        
        // if (idx == d->size) {
        //     return ImPlotPoint{
        //         d->now - d->zeroTime,
        //         d->data[d->offset == 0 ? d->size - 1 : d->offset - 1].y};
        // }
        // ImPlotPoint* point;
        // if (d->offset + idx < d->size) {
        //     point = &d->data[d->offset + idx];
        // } else {
        //     point = &d->data[d->offset + idx - d->size];
        // }
        ImPlotPoint* point = &d->data[idx];
        return ImPlotPoint{point->x - d->zeroTime, point->y};
    };
    
    if (ImPlot::GetCurrentPlot()->YAxis(m_yAxis).Enabled) {
        ImPlot::SetAxis(ImAxis_Y1 + m_yAxis);
    } else {
        ImPlot::SetAxis(ImAxis_Y1);
    }
    ImPlot::PlotLineG(m_id.c_str(), getter, &getterData, getterData.data.size());
}

Plot::PlotAction Plot::EmitContextMenu(){
    Plot::PlotAction action = PlotNothing;
    if (ImGui::BeginMenu(m_name.c_str())) {
            
        ImGui::Checkbox("Display Legend", &m_settings.m_legend);
        ImGui::Checkbox("Autofit Height", &m_settings.m_autoheight);
        if(!m_settings.m_autoheight){
            ImGui::InputInt("Height", &m_height);
            if(m_height < 0){
                m_height = 0;
            }
        }
        if(ImGui::Button("Delete")){
            action = PlotDelete;
        }
        EmitSettings();
        ImGui::EndMenu();
    }
    return action;
}

EntryPlot::PlotAction EntryPlot::EmitSettings(){
    if(ImGui::CollapsingHeader(m_id.c_str())){
        ImGui::PushID(m_id.c_str());
        if(ImGui::Button("Delete")){
            ImGui::PopID();
            return ACTION_DELETE;
        }
        m_color.ColorEdit3(m_id.c_str());
        ImGui::PopID();
    }
    return ACTION_NOTHING;
}

void Plot::EmitSettings(){
    ImGui::PushID("Settings");
    if(!m_settings.m_autoheight){
    }
    std::vector<int> removed_idxs;
    for(size_t i = 0; i < m_entryPlots.size(); i++){
        auto& plot = m_entryPlots[i];
        auto action = plot->EmitSettings();
        if(action == EntryPlot::ACTION_DELETE){
            removed_idxs.emplace_back(i);
        }
    }
    for(size_t i = removed_idxs.size()-1; static_cast<int>(i) >= 0; i--){
        size_t idx = removed_idxs[i];
        if(idx < m_entryPlots.size()){
            m_entryPlots.erase(m_entryPlots.begin()+idx);
        }
    }

    ImGui::PopID();
}

void Plot::DragDropAccept(){
    if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntryData")){
        auto source = *static_cast<EntryData**>(payload->Data);
        auto it =
            std::find_if(m_entryPlots.begin(), m_entryPlots.end(), [=](const auto& elem) {
                return elem->GetId() == source->GetName();
            });
        if(it == m_entryPlots.end()){
            m_entryPlots.emplace_back(std::make_unique<EntryPlot>(source, source->GetName(), 0, m_entryPlots.size()) );
            NotifyChange();
        }
    }
}

void Plot::DragDropTarget(){
    if(ImPlot::BeginDragDropTargetPlot() ||
        ImPlot::BeginDragDropTargetLegend()){
        DragDropAccept();
        ImPlot::EndDragDropTarget();
    }
}

void Plot::EmitPlot(){
    // Calculate Height
    if (ImPlot::BeginPlot(m_name.c_str(), ImVec2(-1, m_height))) {
    

        // setup x axis
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoMenus);
        double now = m_now;
        ImPlot::SetupAxisLimits(
            ImAxis_X1, now - m_settings.m_viewTime, now,
            ImGuiCond_Always);


        // setup y axes
        for (size_t i = 0; i < m_axis.size(); ++i) {
            ImPlotAxisFlags flags = 0;
                //   (m_axis[i].lockMin ? ImPlotAxisFlags_LockMin : 0) |
                //   (m_axis[i].lockMax ? ImPlotAxisFlags_LockMax : 0) |
                //   (m_axis[i].autoFit ? ImPlotAxisFlags_AutoFit : 0) |
                //   (m_axis[i].invert ? ImPlotAxisFlags_Invert : 0) |
                //   (m_axis[i].opposite ? ImPlotAxisFlags_Opposite : 0) |
                //   (m_axis[i].gridLines ? 0 : ImPlotAxisFlags_NoGridLines) |
                //   (m_axis[i].tickMarks ? 0 : ImPlotAxisFlags_NoTickMarks) |
                //   (m_axis[i].tickLabels ? 0 : ImPlotAxisFlags_NoTickLabels);
            ImPlot::SetupAxis(
                ImAxis_Y1 + i,
                m_axis[i].label.empty() ? nullptr : m_axis[i].label.c_str(), flags);
            ImPlot::SetupAxisLimits(
                ImAxis_Y1 + i, m_axis[i].min, m_axis[i].max,
                m_axis[i].apply ?
                ImGuiCond_Always : ImGuiCond_Once
                );
            ImPlot::SetupAxisScale(ImAxis_Y1 + i, m_axis[i].logScale
                                                        ? ImPlotScale_Log10 :  ImPlotScale_Linear);
            m_axis[i].apply = false;
        }

        ImPlot::SetupFinish();

        for(auto& plot : m_entryPlots){
            if (plot->m_color.GetColorFloat()[3] == IMPLOT_AUTO) {
                plot->m_color.SetColor(ImPlot::GetColormapColor(plot->m_number));
            }
            ImPlot::SetNextLineStyle(plot->m_color.GetColor());
            plot->EmitPlot(*this);
        }

        DragDropTarget();

        ImPlot::EndPlot();
    }
}
void EntryPlot::Update(Plot& view){
    auto& axis = view.m_axis[m_yAxis];
    int end = (m_offset * 1e6) + (view.m_now * 1e6);
    int start = m_offset * 1e6;
    CreatePlot(axis, start, end, view.m_sampleRate);
    axis.apply = true;
}

void EntryPlot::CheckForChange(Plot& view){
    if(m_offset != m_entry->GetOffset()){
        m_offset = m_entry->GetOffset();
        Update(view);
    }
}

void Plot::Display() {
    ImGui::PushID(m_name.c_str());

    EmitPlot();
    if(m_nowRef != m_now){
        m_now = m_nowRef;
        NotifyChange();
    }
    for(auto& plot : m_entryPlots){
        plot->CheckForChange(*this);
    }

    ImGui::PopID();
}


void Plot::NotifyChange(){
    // Update all EntryPlot's
    for(auto& plot : m_entryPlots){
        plot->Update(*this);
    }
    m_settings.m_viewTime = m_now;
}

PlotView::PlotView(PlotProvider *provider, float& now, Storage& storage) : m_provider{provider}, 
                    m_now{now}, 
                    m_name{storage.GetString("name")},
                    m_plotStorage{storage.GetChildArray("plots")},
                    m_storage{storage} {
    for(auto&& v : m_plotStorage){
        m_plots.emplace_back(std::make_unique<Plot>(now, v->GetString("name")));
    }
}

void PlotView::AddPlot(){
    m_plotStorage.emplace_back(std::make_unique<Storage>());
    auto& name = m_plotStorage.back()->GetString("name");
    if(name == ""){
        name = fmt::format("Plot {}", m_plots.size()+1);
    }
    m_plots.emplace_back(std::make_unique<Plot>(m_now, name));
}

void PlotView::EmitContextMenu(){
    if (ImGui::BeginPopupContextItem(m_name.c_str())) {
        if(ImGui::Button("Add Plot")){
            AddPlot();
        }
        std::vector<Plot::PlotAction> actions;
        for(auto& plot : m_plots){
            actions.push_back(plot->EmitContextMenu());
        }
        for(int i = std::min(actions.size()-1, m_plots.size()-1); i >= 0; i--){
            if(actions[i] == Plot::PlotDelete){
                m_plotStorage.erase(m_plotStorage.begin()+i);
                m_plots.erase(m_plots.begin()+i);
                for(int j = i; j < m_plots.size(); j++){
                    m_plots[j]->m_name = fmt::format("Plot {}", j+1);
                }
            }
        }
        ImGui::EndPopup();
    }        
}

void PlotView::Display(){
    EmitContextMenu();
    if(m_plots.size() == 0){
        if(ImGui::Button("Add Plot")){
            AddPlot();
        }
    }
    int availHeight = ImGui::GetContentRegionAvail().y;
    int numAuto = 0;
    for(auto& plot : m_plots){
        if(plot->m_settings.m_autoheight){
            numAuto += 1;
        } else {
            availHeight -= plot->m_height;
        }
        availHeight -= ImGui::GetStyle().ItemSpacing.y;
    }
    if(numAuto > 0){
        int avgHeight = availHeight/numAuto;
        for(auto& plot : m_plots){
            plot->SetAutoHeight(avgHeight);
        }
    }
    for(auto& plot : m_plots){
        plot->Display();
    }
}


PlotProvider::PlotProvider(Storage& storage, float& now) : glass::WindowManager(storage), m_now{now}{
    storage.SetCustomApply([this] {
        for (auto&& windowkv : m_storage.GetChildren()) {
            // get or create window
            auto win = GetOrAddWindow(windowkv.key(), true);
            if (!win) {
                continue;
            }

            // get or create view
            auto view = static_cast<PlotView*>(win->GetView());
            if (!view) {
                win->SetView(std::make_unique<PlotView>(this, m_now, windowkv.value()));
                view = static_cast<PlotView*>(win->GetView());
            }
        }
    });
    storage.SetCustomClear([this] {
        EraseWindows();
        m_storage.EraseChildren();
    });
}

void PlotProvider::DisplayMenu(){
  if(ImGui::BeginMenu("Plots")){
    for (size_t i = 0; i < m_windows.size(); ++i) {
        m_windows[i]->DisplayMenuItem();
        // provide method to destroy the plot window
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Selectable("Destroy Plot Window")) {
                RemoveWindow(i);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    if (ImGui::MenuItem("New Plot Window")) {
        // this is an inefficient algorithm, but the number of windows is small
        char id[32];
        size_t numWindows = m_windows.size();
        for (size_t i = 0; i <= numWindows; ++i) {
            std::snprintf(id, sizeof(id), "Plot <%d>", static_cast<int>(i));
            bool match = false;
            for (size_t j = i; j < numWindows; ++j) {
                if (m_windows[j]->GetId() == id) {
                match = true;
                break;
                }
            }
            if (!match) {
                break;
            }
        }
        if (auto win = AddWindow(
            id, std::make_unique<PlotView>(this, m_now, m_storage.GetChild(id)))) {
                m_storage.GetChild(id).GetString(id, id);
                win->SetDefaultSize(700, 400);
        }
    }
    ImGui::EndMenu();
  }
}


