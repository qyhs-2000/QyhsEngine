#include "timer.h"
#include <thread>
//record delta time
namespace QYHS
{
    inline double Timer::record_past_time()
    {
        std::chrono::high_resolution_clock::time_point now_time_stamp = std::chrono::high_resolution_clock::now();
        //cast span time to second
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now_time_stamp - current_time_stamp);
        current_time_stamp = now_time_stamp;
        return time_span.count();
    }

    namespace timer
    {
        void QuickSleep(float milliseconds)
        {
            const std::chrono::high_resolution_clock::time_point stage_time = std::chrono::high_resolution_clock::now();
            const double seconds = double(milliseconds) / 1000;
            const int sleep_millisec_accuracy = 1;
            const double sleep_sec_accuarcy = double(sleep_millisec_accuracy/ 1000);
            while (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - stage_time).count() < seconds)
            {
                if (seconds - (std::chrono::high_resolution_clock::now() - stage_time).count() > sleep_sec_accuarcy)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_millisec_accuracy));
                }
            }

        }
    }
}
