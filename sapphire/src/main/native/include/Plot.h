#pragma once

#include "glass/WindowManager.h"
#include "glass/support/ColorSetting.h"
#include "DataLog.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <implot.h>
#include <implot_internal.h>
#include <vector>

namespace sapphire{
class PlotView;

struct PlotSettings {
    bool m_settings = false;
    bool m_autoheight = true;
    bool m_legend = true;
    float m_viewTime = 0.1f; 
};

struct PlotAxis{
    std::string& label;
    double min;
    double max;
    bool autofit;
    bool logScale;
    bool apply;
};

class EntryPlot {
public:
    EntryPlot(EntryData* entry, std::string id, int yAxis = 0) : m_entry{entry}, 
                                                                id{id}, 
                                                                m_yAxis{yAxis} {};
    void EmitPlot(PlotView& view);
    void CreatePlot(PlotAxis& axis, int startts, int endts, float SampleRate);
    void Update(PlotView& view);
    std::string GetId() const { return id; }
    ImVec4 m_color{1.0,1.0,1.0,1.0};
    std::string id;
private:
    static constexpr int kMaxSize = 20000;
    int m_yAxis;
    // int m_offset = 0;
    // int m_size = 0;
    EntryData* m_entry;
    std::vector<ImPlotPoint> points;
};

class PlotView : public glass::View{
public:
    PlotView(float& now, std::string name, float sampleRate = 0.02f) : m_nowRef{now}, m_now{now}, m_name{name}, m_sampleRate{sampleRate} {
        m_axis.emplace_back(PlotAxis{m_name, 0, 0, true, false, true});
    };
    void Display() override;
    float m_now;
    float& m_nowRef;
    float m_sampleRate;
    int m_height = 0;
    std::string m_name;

    void EmitPlot();
    void EmitContextMenu();
    void EmitSettings();
    void DragDropTarget();
    void DragDropAccept();

    PlotSettings settings;

    std::vector<PlotAxis> m_axis;
    std::vector<std::unique_ptr<EntryPlot> > plots;

private:
    void NotifyChange();

};

}