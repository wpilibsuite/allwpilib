//
// Created by bagatelle on 5/21/22.
//

#pragma once

#ifndef ALLWPILIB_TIMEMANAGER_H
#define ALLWPILIB_TIMEMANAGER_H

#include "glass/View.h"
#include "LogData.h"


namespace sapphire {

// Fills entreis with records from logData.

struct EntryView {
    explicit EntryView(EntryData data ) : data{data} {};
    void Display();
    private:
    int timestamp = 0;
    EntryData data;
    std::string displayString = "";
};

class EntryManager: public glass::View {
    public:
        void Display() override;
        std::string name;
        static void FromLogData(const LogData& logData);
};
}  // namespace sapphire

#endif  // ALLWPILIB_TIMEMANAGER_H
