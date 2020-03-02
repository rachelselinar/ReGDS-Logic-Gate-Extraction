//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
#include "global/global.h"
#include "cmdline/CommandLineOptions.h"
#include "db/Database.h"

int main(int argc, char** argv)
{
    using namespace PROJECT_NAMESPACE;

    MsgPrinter::startTimer();

    // Parse command-line options
    CommandLineOptions opt(argc, argv);
    MsgPrinter::reportCommandLineOptions(opt);

    // OpenMP settings
    omp_set_nested(1);
    omp_set_dynamic(0);
    omp_set_num_threads(1);

    // Open log file
    if (! opt.log.empty())
    {
        MsgPrinter::openLogFile(opt.log);
    }

    std::string spiLib;
#include "library/lib_path.cpp"
    //Check mode of operation
    if (opt.lib)
    {
        Database libdb;
        //Obtain logic gate information from library
        libdb.libraryUpdate(opt.aux);
    } else
    {
        Database db;
        // Parse data into database
        db.parse(opt.aux);
        //Analyze netlist
        db.analyzeNetlist();
        //Port propagation
        db.portPropagation();

        //Hierarchical leaf subckts flattening + logic gate extraction
        db.hierExtraction();
        //Port direction update for non leaf subckts
        db.portDirUpdate();

        //Write out verilog
        db.outVerilog(opt.sOut, spiLib);
        //Write out dot file for graphviz for visualization
        if (opt.dot)
        {
            db.dotOut();
        }
    }

    // Close log file
    if (! opt.log.empty())
    {
        MsgPrinter::closeLogFile();
    }
}
