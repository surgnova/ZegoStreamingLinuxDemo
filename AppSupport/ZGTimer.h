//
// ZGTimer.h
//
// Copyright 2018Äê Zego. All rights reserved.
//

#ifndef ZGTimer_h__
#define ZGTimer_h__

#include <stdint.h>

bool QueryPerformanceFrequency(int64_t *frequency);
bool QueryPerformanceCounter(int64_t *performance_count);

class ZGTimer
{
public:

    ZGTimer()
    {
        QueryPerformanceFrequency(&frequency_);
        QueryPerformanceCounter(&start_count_);
    }

    ~ZGTimer()
    {

    }

    double ElapsedMs()
    {
        int64_t CurrentCount;
        QueryPerformanceCounter(&CurrentCount);
        return double((CurrentCount - start_count_)*1000.0f) / (double)frequency_;
    }

private:
    int64_t frequency_;
    int64_t start_count_;
};

#endif // ZGTimer_h__


