
#pragma once

class SimCB {
public:

    SimCB(std::function<void(void)> fn, std::function<void(int32_t)> cancel) : 
        m_fn(fn),
        m_cancel(cancel)
    {}

    void SetUID(int32_t uid) {
        m_uid = uid;
    }

    ~SimCB() {
        Cancel();
    }

    void Cancel() {
        if (m_valid) {
            m_cancel(m_uid);
            m_valid = false;
        }
    }

    std::function<void(void)> m_fn;

private:
    bool m_valid = true;
    int32_t m_uid;
    std::function<void(int32_t)> m_cancel;
};