#include "timestamp.h"

#include <cassert>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

// offset in microseconds
static unsigned long long
zerotime()
{
#ifdef _WIN32
    FILETIME ft;
    unsigned long long tmpres =0;
    // 100-nanosecond intervals since January 1, 1601 (UTC)
    // which means 0.1 us
    GetSystemTimeAsFileTime(&ft);
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
    // January 1st, 1970 - January 1st, 1601 UTC ~ 369 years
    // or 116444736000000000 us
    static const unsigned long long deltaepoch = 116444736000000000ull;
    tmpres -= deltaepoch;
    return tmpres;
#else
    timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);

    // in 100-ns intervals
    return static_cast<unsigned long long>(ts.tv_sec) * 10000000u +
        static_cast<unsigned long long>(ts.tv_nsec) / 100u;
#endif
}

static unsigned long long
timestamp()
{
#ifdef _WIN32
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    // there is an imprecision with the initial value,
    // but what matters is that timestamps are monotonic and consistent
    return static_cast<unsigned long long>(li.QuadPart);
#else
    timespec ts;

    // cannot fail, parameters are correct and we checked earlier we can
    // access the clock
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // in ns
    return static_cast<unsigned long long>(ts.tv_sec) * 1000000000u +
        static_cast<unsigned long long>(ts.tv_nsec);
#endif
}

static unsigned long long
update_frequency()
{
#ifdef _WIN32
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li) || !li.QuadPart)
    {
        // log something
        std::terminate();
    }
    return static_cast<unsigned long long>(li.QuadPart);
#else
    timespec ts;

    if (clock_getres(CLOCK_MONOTONIC, &ts) < 0)
    {
        // log error
        std::terminate();
    }

    assert(!ts.tv_sec);

    // this is the precision of the clock, we want the number of updates per
    // second, which is 1 / ts.tvnsec * 1,000,000,000
    static const unsigned long long billion = 1000000000;

    return billion / static_cast<unsigned long long>(ts.tv_nsec);
#endif
}

static const unsigned long long zerotime_val = zerotime();
static const unsigned long long offset_val = timestamp();
static const unsigned long long frequency_val = update_frequency();

unsigned long long
NT_Now()
{
    assert(offset_val > 0u);
    assert(frequency_val > 0u);
    unsigned long long delta = timestamp() - offset_val;
    // because the frequency is in update per seconds, we have to multiply the
    // delta by 10,000,000
    unsigned long long delta_in_us = delta * 10000000u / frequency_val;
    return delta_in_us + zerotime_val;
}
