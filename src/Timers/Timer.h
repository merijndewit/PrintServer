#pragma once
#include "esp_timer.h"

namespace PrintServer
{
    class Timer
    {
    public:
        Timer()
        {
            start = esp_timer_get_time();
        }
        float get_time()
        {
            return (esp_timer_get_time() - start) / 1000.f;
        }

        void reset()
        {
            start = esp_timer_get_time();
        }

    private:
        uint64_t start, elapsed;
    };
}