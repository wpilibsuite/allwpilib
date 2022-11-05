#pragma once

#include "glass/WindowManager.h"
#include "glass/support/ColorSetting.h"
#include "DataLog.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <implot.h>
#include <implot_internal.h>
#include <vector>
#include "glass/WindowManager.h"
#include "glass/Storage.h"
using glass::Storage;

namespace sapphire{
class Plot;
class PlotProvider;

struct PlotSettings {
    bool m_settings = true;
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
    EntryPlot(EntryData* entry, std::string id, int yAxis = 0, int number = 0) : m_entry{entry}, 
                                                                id{id}, 
                                                                m_yAxis{yAxis},
                                                                number{number},
                                                                _color{1.0,1.0,1.0,-1.0},
                                                                m_color{_color} {};
    void EmitPlot(Plot& view);
    
    enum PlotAction{
        ACTION_NOTHING,
        ACTION_UP,
        ACTION_DOWN,
        ACTION_DELETE
    };

    EntryPlot::PlotAction EmitSettings();
    void CreatePlot(PlotAxis& axis, int startts, int endts, float SampleRate);
    void Update(Plot& view);
    void CheckForChange(Plot& view);
    std::string GetId() const { return id; }
    std::vector<float> _color;
    glass::ColorSetting m_color;
    int number;
    std::string id;
private:
    static constexpr int kMaxSize = 20000;
    int m_yAxis;
    float m_offset = 0;
    // int m_size = 0;
    EntryData* m_entry;
    std::vector<ImPlotPoint> points;
};

class Plot{
public:
    Plot(float& now, std::string name, float sampleRate = 0.02f) : m_nowRef{now}, m_now{now}, m_name{name}, m_sampleRate{sampleRate} {
        m_axis.emplace_back(PlotAxis{m_name, 0, 0, true, false, true});
    };
    void Display() ;
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
    void SetAutoHeight(int height) { if(settings.m_autoheight){ m_height = height; } }
    PlotSettings settings;

    std::vector<PlotAxis> m_axis;
    std::vector<std::unique_ptr<EntryPlot> > plots;

private:
    void NotifyChange();

};


class PlotProvider : private glass::WindowManager{
  public:
    explicit PlotProvider(Storage& storage, float& m_now);
    ~PlotProvider() override;
    using glass::WindowManager::GlobalInit;
    void DisplayMenu() override;
    float& m_now;
};

class PlotView : public glass::View{
    public:
    PlotView(PlotProvider *provider, float& now, std::string name) : provider{provider}, m_now{now}, m_name{name} {}
    void Display() override;
    void EmitContextMenu();
    std::string m_name;
    std::vector<std::unique_ptr<Plot> > plots;
    float& m_now;
    PlotProvider *provider;
};

}