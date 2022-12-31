#pragma once

#include "glass/View.h"
#include "glass/WindowManager.h"
#include "DataLog.h"
#include "glass/Storage.h"
#include "glass/support/ColorSetting.h"
using glass::Storage;

namespace sapphire{

class TextLog : public glass::View {
public:
    TextLog(Storage& storage, float& timestamp) : m_name{storage.GetString("name", "Text Log")},
                                                  m_nowRef{timestamp}  {};
    void Display() override;
    struct EntryDataObject {
        EntryDataObject(EntryData& data, float startingTime) : m_data{data},
                                          isShowing{true},
                                          _color{1.0, 1.0, 1.0, 1.0},
                                          m_color{_color},
                                          m_lastTime{startingTime} {};
        void EmitContextMenu();
        EntryData& m_data;
        bool isShowing;
        std::vector<float> _color;
        glass::ColorSetting m_color;
        double m_lastTime;
    };
    std::string m_name;
    std::vector<EntryDataObject> data;
    std::string text = "";
    float& m_nowRef;
    void DragDropTarget();
    void DragDropAccept();
    void ReconstructLog();
};

class TextLogProvider : glass::WindowManager {

};

}