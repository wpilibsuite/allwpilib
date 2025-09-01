
#pragma once

#include <hal/SimDevice.h>

class SimValueCB {
public:

    using FnType = std::function<void(const char *, HAL_SimValueHandle, HAL_SimValueDirection, HAL_Value)>;

    SimValueCB(FnType fn, std::function<void(int32_t)> cancel) :
        m_fn(fn),
        m_cancel(cancel)
    {}

    void SetUID(int32_t uid) {
        m_uid = uid;
    }

    ~SimValueCB() {
        Cancel();
    }

    void Cancel() {
        if (m_valid) {
            m_cancel(m_uid);
            m_valid = false;
        }
    }

    FnType m_fn;

private:
    bool m_valid = true;
    int32_t m_uid;
    std::function<void(int32_t)> m_cancel;
};