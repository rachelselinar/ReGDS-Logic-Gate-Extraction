//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

// Add all global/common header files here, so that
// we only need to include this header file in other files
#include <omp.h>
#include <vector>
#include <algorithm>
#include "global/type.h"
#include "global/namespace.h"
#include "global/Parameters.h"
#include "util/Assert.h"
#include "util/MsgPrinter.h"
#include "util/AutoTimer.h"

PROJECT_NAMESPACE_BEGIN

// Function aliases
static const auto &INF = MsgPrinter::inf;
static const auto &WRN = MsgPrinter::wrn;
static const auto &ERR = MsgPrinter::err;
static const auto &DBG = MsgPrinter::dbg;

template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

PROJECT_NAMESPACE_END

#endif // __GLOBAL_H__
