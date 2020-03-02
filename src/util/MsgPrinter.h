//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __MSGPRINTER_H__
#define __MSGPRINTER_H__

#include <cstdio> // FILE *, stderr
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include "global/namespace.h"

PROJECT_NAMESPACE_BEGIN

/// Enum type for message printing
enum class MsgType
{
    INF,
    WRN,
    ERR,
    DBG
};

/// Function converting enum type to std::string
std::string msgTypeToStr(MsgType msgType);

// Forward declaration
class CommandLineOptions;

/// Message printing class
class MsgPrinter
{
private:
    using clock = std::chrono::system_clock;
    using millisec = std::chrono::milliseconds;

public:
    static void startTimer() { _startTime = clock::now(); }  // Cache start time
    static void screenOn()   { _screenOutStream = stderr; }  // Turn on screen printing
    static void screenOff()  { _screenOutStream = nullptr; } // Turn off screen printing
    
    static void openLogFile(const std::string &file);
    static void closeLogFile();
    static void inf(const char *rawFormat, ...);
    static void wrn(const char *rawFormat, ...);
    static void err(const char *rawFormat, ...);
    static void dbg(const char *rawFormat, ...);
    static void reportCommandLineOptions(const CommandLineOptions &opt);

private:
    static void print(MsgType msgType, const char *rawFormat, va_list args);

private:
    static clock::time_point   _startTime;
    static FILE *              _screenOutStream;  // Out stream for screen printing
    static FILE *              _logOutStream;     // Out stream for log printing
    static std::string         _logFileName;      // Current log file name
};

PROJECT_NAMESPACE_END

#endif // __MSGPRINTER_H__
