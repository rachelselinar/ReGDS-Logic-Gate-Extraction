//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#include "util/MsgPrinter.h"
#include "util/Assert.h"
#include "cmdline/CommandLineOptions.h"

PROJECT_NAMESPACE_BEGIN

MsgPrinter::clock::time_point  MsgPrinter::_startTime        = MsgPrinter::clock::now();
FILE *                         MsgPrinter::_screenOutStream  = stderr;
FILE *                         MsgPrinter::_logOutStream     = nullptr;
std::string                    MsgPrinter::_logFileName      = "";

/// Converting enum type to std::string
std::string msgTypeToStr(MsgType msgType)
{
    switch (msgType)
    {
        case MsgType::INF:  return "INF"; break;
        case MsgType::WRN:  return "WRN"; break;
        case MsgType::ERR:  return "ERR"; break;
        case MsgType::DBG:  return "DBG"; break;
    }
    AssertMsg(false, "Unknown MsgType.");
}


/// Open a log file, all output will be stored in the log
void MsgPrinter::openLogFile(const std::string &logFileName)
{
    if (_logOutStream != nullptr)
    {
        fclose(_logOutStream);
        wrn("Current log file %s is forcibly closed", _logFileName.c_str());
    }

    _logFileName = logFileName;
    _logOutStream = fopen(logFileName.c_str() , "w");

    if (_logOutStream == nullptr)
    {
        err("Cannot open log file %s", logFileName.c_str());
    }
    else
    {
        inf("Open log file %s", logFileName.c_str());
    }
}

/// Close current log file
void MsgPrinter::closeLogFile()
{
    if (_logOutStream == nullptr)
    {
        wrn("No log file is opened. Call to %s is ignored", __func__);
    }
    else
    {
        inf("Close log file %s", _logFileName.c_str());
    }
}

/// Print information
void MsgPrinter::inf(const char *rawFormat, ...)
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::INF, rawFormat, args);
    va_end(args);
}

/// Print warnings
void MsgPrinter::wrn(const char *rawFormat, ...)
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::WRN, rawFormat, args);
    va_end(args);
}

/// Print errors
void MsgPrinter::err(const char *rawFormat, ...)
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::ERR, rawFormat, args);
    va_end(args);
    exit(1);
}

/// Print debugging information
void MsgPrinter::dbg(const char *rawFormat, ...)
{
    va_list args;
    va_start(args, rawFormat);
    print(MsgType::DBG, rawFormat, args);
    va_end(args);
}

/// Message printing kernel
void MsgPrinter::print(MsgType msgType, const char *rawFormat, va_list args)
{
    // Get the message type
    std::string type = "[" + msgTypeToStr(msgType);

    // Get current time
    clock::time_point now = clock::now();

    // Get local time
    char locTime[32];
    std::time_t now_time_t = clock::to_time_t(now);
    struct tm * timeinfo;
    timeinfo = std::localtime(&now_time_t);
    strftime(locTime, sizeof(locTime), " %F %T ", timeinfo);

    // Get elapsed time
    char elpTime[32];
    double elapsed = std::chrono::duration_cast<millisec>(now - _startTime).count() / 1000.0;
    sprintf(elpTime, "%7.2lf sec]  ", elapsed);

    // Combine all the strings together
    std::string format = type + std::string(locTime) + std::string(elpTime) + std::string(rawFormat) + "\n";

    // print to log
    if (_logOutStream)
    {
        va_list args_copy;
        va_copy(args_copy, args);
        vfprintf(_logOutStream, format.c_str(), args_copy);
        va_end(args_copy);
        fflush(_logOutStream);
    }

    // print to screen
    if (_screenOutStream)
    {
        vfprintf(_screenOutStream, format.c_str(), args);
        fflush(_screenOutStream);
    }
}

/// Report user-defined command line options
void MsgPrinter::reportCommandLineOptions(const CommandLineOptions &opt)
{
    inf("----- Command-Line Options -----");
    //inf("numThreads = %u", opt.numThreads);
    //inf("--------------------------------");
}

PROJECT_NAMESPACE_END
