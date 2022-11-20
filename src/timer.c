#include <stdint.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static LARGE_INTEGER timer_frequency;
static LARGE_INTEGER timer_start;
#else
#include <time.h>
#include <unistd.h>
#endif

void init_timer_system(void)
{
    #ifdef _WIN32
    QueryPerformanceFrequency(&timer_frequency);
    QueryPerformanceCounter(&timer_start);
    #endif
}

uint64_t time_in_nanos(void)
{
    #ifdef _WIN32
    LARGE_INTEGER timer;
    QueryPerformanceCounter(&timer);
    timer.QuadPart -= timer_start.QuadPart;
    return (timer.QuadPart * 1000000000LL) / timer_frequency.QuadPart;
    #else
    // TODO: Linux
    return 0;
    #endif
}