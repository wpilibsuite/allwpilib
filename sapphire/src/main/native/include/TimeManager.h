#pragma once

#include <map>

#include "glass/View.h"

#include "DataLog.h"

namespace sapphire{

class TimeManager : public glass::View{
    public:
    TimeManager(DataLogView& logView) : m_logView{logView}, timestamp{logView.GetTimestamp()} {};
    void Display() override;
    DataLogView& m_logView;
    float& timestamp;
    bool limitOffset = true;
};

}