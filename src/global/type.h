//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __TYPE_H__
#define __TYPE_H__

#include <cstdint>
#include <string>
#include "global/namespace.h"

PROJECT_NAMESPACE_BEGIN

// Built-in type aliases
using IndexType   = std::uint32_t;
using Byte        = std::uint8_t;

// Built-in type constants
constexpr IndexType INDEX_TYPE_MAX = 1000000000;  // 1e+9

//Enum for Subckt pin type
enum class PinType : Byte
{
    PORT,
    NET,
    INVALID
};

//Enum for Subckt pin dir type
enum class PinDir : Byte
{
    INPUT,
    OUTPUT,
    INOUT,
    INVALID
};

//Enum for Subckt Instance type
enum class InstType : Byte
{
    NFET,
    PFET,
    DIODE,
    RES,
    CAP,
    PNP,
    NPN,
    COMPONENT,
    INVALID
};

//Enum for logic gate type
enum class GateType : Byte
{
    GATE,  
    NOT_GATE, //for devices
    COMPONENT, //for instances
    INVALID
};


PROJECT_NAMESPACE_END

#endif // __TYPE_H__
