//
// Created by bagatelle on 5/21/22.
//

#pragma once

#ifndef ALLWPILIB_TIMEMANAGER_H
#define ALLWPILIB_TIMEMANAGER_H

#include "glass/View.h"

namespace sapphire {

// Fills entreis with records from logData.
struct EntryData;

struct EntryView {
    explicit EntryView() {};
    explicit EntryView(EntryData* data, float timestamp=0);
    void Display(bool update, float timestamp);
    private:
    EntryData* data;
    std::string displayString = "";
};

}  // namespace sapphire

#endif  // ALLWPILIB_TIMEMANAGER_H
