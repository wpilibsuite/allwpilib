#pragma once

#include <units/time.h>

namespace frc {
class PneumaticsBase {
public:
    virtual ~PneumaticsBase() = default;

    virtual void SetSolenoids(int mask, int values) = 0;

    virtual int GetSolenoids() const = 0;

    virtual int GetModuleNumber() const = 0;

    virtual int GetSolenoidDisabledList() const = 0;

    virtual void FireOneShot(int index) = 0;

    virtual void SetOneShotDuration(int index, units::second_t duration) = 0;

    virtual bool CheckSolenoidChannel(int channel) = 0;
};
}
