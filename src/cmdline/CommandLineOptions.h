//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __COMMANDLINEOPTIONS_H__
#define __COMMANDLINEOPTIONS_H__

#include <string>
#include "global/global.h"
#include "cmdline/cmdline.h"

PROJECT_NAMESPACE_BEGIN

/// Class to parse command line options
class CommandLineOptions
{
public:
    CommandLineOptions(int argc, char **argv);

private:
    void populateAndCheckOptions();

public:
    // Options
    std::string      aux;
    std::string      log;
    bool             lib;
    bool             dot;
    bool             sOut;

private:
    cmdline::parser  _parser;
};

PROJECT_NAMESPACE_END

#endif // __COMMANDLINEOPTIONS_H__
