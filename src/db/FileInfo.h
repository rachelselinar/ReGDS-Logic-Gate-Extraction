//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __FILEINFO_H__
#define __FILEINFO_H__

#include <string>
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// Class for information of input/output files and paths
class FileInfo
{
public:
    // Getters
    const std::string &  inputPath() const                   { return _inputPath; }
    const std::string &  auxFile() const                     { return _auxFile; }
    const std::string &  libFile() const                     { return _libFile; }

    // Setters
    void                 setAuxFile(const std::string &str)    { splitPathToDirAndBase(str, _inputPath, _auxFile); }
    void                 setLibFile(const std::string &str)    { _libFile = str; }

private:
    void                 splitPathToDirAndBase(const std::string &path, std::string &dir, std::string &base) const;

private:
    std::string  _inputPath;
    std::string  _auxFile;  // Base file name, same below
    std::string  _libFile;
};

/// Split a path into directory name and base name
inline void FileInfo::splitPathToDirAndBase(const std::string &path, std::string &dir, std::string &base) const
{
    auto pos = path.find_last_of('/');
    if (pos == std::string::npos)
    {
        dir = "./";
        base = path;
    }
    else
    {
        dir = path.substr(0, pos + 1);
        base = path.substr(pos + 1);
    }
}

PROJECT_NAMESPACE_END

#endif // __FILEINFO_H__
