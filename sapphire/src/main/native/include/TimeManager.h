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
    bool isPlaying = false;
    struct TimeOption{
        TimeOption(std::string name, double speed) : m_name{name}, m_speed{speed} {};
        std::string m_name;
        double m_speed;
        bool selected = false;
    };
    TimeOption m_options[7] = {
        TimeOption{"1/8", 1.0/8.0},
        TimeOption{"1/4", 1.0/4.0},
        TimeOption{"1/2", 1.0/2.0},
        TimeOption{"1", 1.0},
        TimeOption{"2", 2.0},
        TimeOption{"4", 4.0},
        TimeOption{"8", 8.0}
    };
    TimeOption* selected = &m_options[3];
    double lastTime = 0;
};

}