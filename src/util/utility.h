//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

//////////////////////////////////////////////////
///                                            ///
///  This file defines some utility functions  ///
///  that are frequently used in the project   ///
///                                            ///
//////////////////////////////////////////////////


/// Apply a index mapping to a index array
/// That is idxArray[i] = idxMapping[idxArray[i]]
/// \inout  idxArray    the index array
/// \param  idxMapping  the index mapping
inline void applyIndexMapping(std::vector<IndexType> &idxArray, const std::vector<IndexType> &idxMapping)
{
    for (IndexType &idx : idxArray)
    {
        idx = idxMapping.at(idx);
    }
}

/// Sort a index vector (e.g. Node/Net/Pin ID array) from low to high
/// and remove INDEX_TYPE_MAX
inline void sortIndexVectorAndRemoveInvalid(std::vector<IndexType> &idxArray)
{
    std::sort(idxArray.begin(), idxArray.end());
    while (! idxArray.empty() && idxArray.back() == INDEX_TYPE_MAX)
    {
        idxArray.pop_back();
    }
}

/// safe absolute value of value difference
template <typename T>
inline T absdiff(T a, T b)
{
    return a < b ? b - a : a - b;
}

/// Clamp a number to a [lo, hi] range
template <typename T>
inline T clamp(T v, T lo, T hi)
{
    return std::max(lo, std::min(v, hi));
}

PROJECT_NAMESPACE_END

#endif // __UTILITY_H__
