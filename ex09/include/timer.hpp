#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <chrono>

namespace timer {
    class Precision {
        std::chrono::high_resolution_clock::time_point t_start;
        std::chrono::high_resolution_clock::time_point t_stop;
        bool stopped;
        public:
        Precision () {
            t_start = std::chrono::high_resolution_clock::now();
            t_stop = std::chrono::high_resolution_clock::now();
            stopped = false;
        }
        std::chrono::high_resolution_clock::time_point stop () {
            t_stop = std::chrono::high_resolution_clock::now();
            stopped = true;

            return t_stop;
        }
        double duration () {
            if (stopped) {
                return std::chrono::duration<double>(t_stop - t_start).count();
            } else {
                return std::chrono::duration<double>(
                    std::chrono::high_resolution_clock::now() - t_start
                ).count();
            }
        }
    };
}

#endif /* ifndef _TIMER_HPP_ */
