//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __AUTOTIMER_H__
#define __AUTOTIMER_H__

#include <chrono>
#include <string>
#include "global/namespace.h"

PROJECT_NAMESPACE_BEGIN

/// This class is to eliminate boost::timer dependency
struct AutoTimer
{
    AutoTimer(const std::string &str) : msg(str), start(std::chrono::high_resolution_clock::now()) {}
    ~AutoTimer()
    {
        printf("%s : %.4e sec\n", msg.c_str(), elapsedNanoSec() * 1.0e-9);
    }

    void print(const std::string &str) const
    {
        printf("%s : %s : %.4e sec\n", msg.c_str(), str.c_str(), elapsedNanoSec() * 1.0e-9);
    }

    double elapsedNanoSec() const
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::nanoseconds elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    std::string                                                  msg;
    std::chrono::time_point<std::chrono::high_resolution_clock>  start;
};

PROJECT_NAMESPACE_END

#endif // __AUTOTIMER_H__
