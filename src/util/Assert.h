//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <string>
#include <cstdio>
#include "util/MsgPrinter.h"

/// Assert without message
#define Assert(cond) \
        do { \
            if (!(cond)) \
            { \
                char format[1024]; \
                sprintf(format, "Assertion failed at file %s line %d.\n%39sAssert: %s\n", __FILE__, __LINE__, "", #cond); \
                MsgPrinter::err(format); \
            } \
        } while(0)

/// Assert with message
#define AssertMsg(cond, msg, ...) \
        do { \
            if (!(cond)) \
            { \
                char prefix[1024]; \
                sprintf(prefix, "Assertion failed at file %s line %d.\n%39sAssert: %s\n", __FILE__, __LINE__, "", #cond); \
                std::string format = std::string(prefix) + std::string(39, ' ') + "message: " + std::string(msg); \
                MsgPrinter::err(format.c_str(), ##__VA_ARGS__); \
                exit(1); \
            } \
        } while (0)

#endif // __ASSERT_H__

