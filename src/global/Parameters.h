//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

// Forward decalaration
class CommandLineOptions;

/// Class to contain all parameters used in this project
/// All members here are static, so that they can be easilly accessd by any function/object in the project
/// One should not try to create an instance of this class
class Parameters
{
public:
    static void readCommandLineOptions(const CommandLineOptions &opt);

public:
    static IndexType              verbose;

private:
    // Disable creating instances of this class
    Parameters() {}
};

PROJECT_NAMESPACE_END

#endif // __PARAMETERS_H__
