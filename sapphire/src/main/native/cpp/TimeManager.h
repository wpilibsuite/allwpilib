//
// Created by bagatelle on 5/21/22.
//

#pragma once

#ifndef ALLWPILIB_TIMEMANAGER_H
#define ALLWPILIB_TIMEMANAGER_H

#include "glass/View.h"

namespace sapphire {
class TimeManager: public glass::View {
    public:
        void Display() override;
        int test = 0;
};
}  // namespace sapphire

#endif  // ALLWPILIB_TIMEMANAGER_H
