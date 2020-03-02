//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#include <cstring>
#include "cmdline/CommandLineOptions.h"

PROJECT_NAMESPACE_BEGIN

/// Construct and parse command line options
CommandLineOptions::CommandLineOptions(int argc, char** argv)
{
    // Define options 
    _parser.add <std::string> ("sp", 'i', "Provide SPICE netlis for Library or Layout netlist as per mode of operation", true, "");
    _parser.add <bool> ("lib", 'l', "Specify mode of operation (True = Library Build; False = Logic Gate Identification)", true, true);
    _parser.add <bool> ("dot", '\0', "Write out DOT file for graphical visualization (optional, default=false)", false, false);
    _parser.add <bool> ("sOut", '\0', "Write out SPICE file (optional, default=false)", false, false);
    _parser.add <std::string> ("log", '\0', "log file", false, "");

    // Parse and check command line
    // It returns only if command line arguments are valid.
    // If arguments are invalid, a parser output error msgs then exit program.
    // If help flag ('--help' or '-?') is specified, a parser output usage message then exit program.
    _parser.parse_check(argc, argv);

    populateAndCheckOptions();
}

/// Check command line arguments and update options for regular parsing
void CommandLineOptions::populateAndCheckOptions()
{
    // Populate options
    aux              = _parser.get<std::string>("sp");
    log              = _parser.get<std::string>("log");
    lib              = _parser.get<bool>("lib");
    dot              = _parser.get<bool>("dot");
    sOut             = _parser.get<bool>("sOut");

    // SPICE files must be specified
    if (aux.empty())
    {
        std::cerr << "-i <file>.sp must be specified." << std::endl;
        std::cerr << _parser.usage();
        exit(1);
    }
}

PROJECT_NAMESPACE_END
