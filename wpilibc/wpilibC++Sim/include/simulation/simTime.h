#pragma once

#include <condition_variable>
#include <mutex>

namespace wpilib { namespace internal {
    extern double simTime;
    extern ::std::condition_variable time_wait;
    extern ::std::mutex time_wait_mutex;
    // transport::SubscriberPtr time_sub;
}}
