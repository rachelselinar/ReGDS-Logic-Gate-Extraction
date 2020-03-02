//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
#ifndef __NETLIST_H__
#define __NETLIST_H__

#include <vector>
#include <functional>
#include <numeric>
#include <fstream>
#include <unordered_set>
#include "global/global.h"
#include "db/Netlist.h"

PROJECT_NAMESPACE_BEGIN

/// Class for netlist objects


class Pin //Pins within module 
{
public: 
    explicit Pin() = default;
    explicit Pin(std::string const pName) : _pName(pName) {}
    explicit Pin(std::string const pName, PinType nType) : _pName(pName), _pType(nType) {}
    explicit Pin(std::string const pName, PinType nType, PinDir nDir) : _pName(pName), _pType(nType), _pDir(nDir) {}
    explicit Pin(std::string const pName, PinType nType, PinDir nDir, bool pwr) : _pName(pName), _pType(nType), _pDir(nDir), _power(pwr) {}

    //Getters
    std::string const &                 name() const                                  { return _pName; }
    std::string &                       name()                                        { return _pName; }
    PinType                             type() const                                  { return _pType; }
    PinType &                           type()                                        { return _pType; }
    IndexType                           id() const                                    { return _pID; }
    IndexType &                         id()                                          { return _pID; }
    PinDir                              dir() const                                   { return _pDir; }
    PinDir &                            dir()                                         { return _pDir; }
    bool                                isPower() const                               { return _power; }
    bool                                isTieOff() const                              { return _tieOff; }

    // Setters
    void                                setId(IndexType id)                           { _pID = id; }
    void                                setName(std::string const &name)              { _pName = name; }
    void                                setPinType(PinType type)                      { _pType = type; }
    void                                setPinDir(PinDir dir)                         { _pDir = dir; }
    void                                setPower()                                    { _power = true; _pDir = PinDir::INOUT; }
    void                                setNotPower()                                 { _power = false; }
    void                                setTieOff()                                   { _tieOff = true; }

    void clear()
    {
        _pType = PinType::INVALID;
        _pDir = PinDir::INVALID;
        _pName.clear();
    }

private:
    std::string _pName;
    PinType     _pType; //PORT, NET
    IndexType   _pID;
    PinDir      _pDir; //INPUT, OUTPUT, INOUT
    bool        _power;
    bool        _tieOff = false;
};

class Cell 
{
public: 
    explicit Cell() = default;
    explicit Cell(std::string const name, std::string const gname, GateType type) : _cName(name), _gName(gname), _gType(type) {}

    void                                addPin(IndexType pinID)                       { _pinIDs.push_back(pinID); }
    void                                addpName(std::string pName)                   { _pins.push_back(pName); }

    //Getters
    IndexType                           id() const                                    { return _id; }
    IndexType &                         id()                                          { return _id; }
    IndexType                           instId() const                                { return _instId; }
    IndexType &                         instId()                                      { return _instId; }
    GateType                            type() const                                  { return _gType; }
    GateType &                          type()                                        { return _gType; }
    std::string const &                 cName() const                                 { return _cName; }
    std::string &                       cName()                                       { return _cName; }
    std::string const &                 gName() const                                 { return _gName; }
    std::string &                       gName()                                       { return _gName; }

    const std::vector<IndexType> &      pinArray() const                              { return _pinIDs; }
    std::vector<IndexType> &            pinArray()                                    { return _pinIDs; }
    IndexType                           pinId(IndexType i) const                      { return _pinIDs.at(i); }
    IndexType &                         pinId(IndexType i)                            { return _pinIDs.at(i); }
    IndexType                           numPins() const                               { return _pinIDs.size(); }
    const std::vector<std::string> &    featureArray() const                          { return _iFeatures; }
    std::vector<std::string> &          featureArray()                                { return _iFeatures; }
    std::string const &                 feature(IndexType i) const                    { return _iFeatures.at(i); }
    std::string &                       feature(IndexType i)                          { return _iFeatures.at(i); }
    IndexType                           numfeatures() const                           { return _iFeatures.size(); }
    const std::vector<std::string> &    pNameArray() const                            { return _pins; }
    std::vector<std::string> &          pNameArray()                                  { return _pins; }
    std::string const &                 pName(IndexType i) const                      { return _pins.at(i); }
    std::string &                       pName(IndexType i)                            { return _pins.at(i); }
    IndexType                           numpNames() const                             { return _pins.size(); }
    bool                                includePname()                                { return _includePname; }
    bool                                isAnalog()                                    { return _isAnalog; }
    bool                                isInverter()                                  { return _isInverter; }
    float                               lValue()                                      { return _dlength; }
    float                               wValue()                                      { return _dwidth; }
    IndexType                           fins()                                        { return _dfins; }
    IndexType                           mFactor()                                     { return _mfactor; }
    IndexType                           sFactor()                                     { return _sfactor; }

    // Setters
    void                                setId(IndexType id)                                     { _id = id; }
    void                                setInstId(IndexType instId)                             { _instId = instId; }
    void                                setType(GateType type)                                  { _gType = type; }
    void                                setName(std::string const &name)                        { _cName = name; }
    void                                setGName(std::string const &name)                       { _gName = name; }
    void                                setFeatures(std::vector<std::string> const &features)   { _iFeatures = features; }
    void                                setPname()                                              { _includePname = true; }
    void                                setNotPname()                                           { _includePname = false; }
    void                                setAnalog()                                             { _isAnalog = true; }
    void                                setNotAnalog()                                          { _isAnalog = false; }
    void                                setInverter()                                           { _isInverter = true; }
    void                                setNotInverter()                                        { _isInverter = false; }
    void                                clrPins()                                               { _pinIDs.clear(); }
    void                                setlValue(float val)                                    { _dlength = val; }
    void                                setwValue(float val)                                    { _dwidth = val; }
    void                                setfins(IndexType fins)                                 { _dfins = fins; }
    void                                setmFactor(IndexType val)                               { _mfactor = val; }
    void                                setsFactor(IndexType val)                               { _sfactor = val; }

    void clear()
    {
        _cName.clear();
        _gName.clear();
        _gType = GateType::INVALID;
        _pinIDs.clear();
        _iFeatures.clear();
        _pins.clear();
        _includePname = false;
        _isAnalog = false;
        _isInverter = false;
        _dlength = 0.0;
        _dwidth = 0.0;
        _dfins = 0;
        _mfactor = 0;
        _sfactor = 0;
    }

private:
    std::string _cName; //cell name
    std::string _gName; //gate name
    GateType _gType; //GATE, NOT_GATE, COMPONENT, INVALID
    IndexType _id;
    IndexType _instId; //Instance in case of NOT_GATE
    std::vector<std::string> _pins; //Pin name for cell
    std::vector<IndexType> _pinIDs;
    std::vector<std::string> _iFeatures; //Currently not using features - just storing them
    bool _includePname;
    bool _isAnalog;
    bool _isInverter;
    //Additional features for FETs to check if ANALOG type
    float _dlength; //Default unit is nanometers 'n'
    float _dwidth; //Default unit is nanometers 'n'
    IndexType _dfins; //No of fins
    IndexType _mfactor; //multiplier factor for parallel connection
    IndexType _sfactor; //multiplier factor for series connection
};

class Module 
{
public:
    explicit Module() = default;
    explicit Module(std::string const name) : _name(name) {}
    // Module &operator = (const Module &mod) {};

    std::vector<bool>      portList;
    std::unordered_map<std::string, IndexType> pinName2IdMap;

    bool isPort (IndexType portId)
    {
        if (std::find(_ports.begin(), _ports.end(), portId) == _ports.end())
        {
            return false;
        } else
        {
            return true;
        }
    }

    std::unordered_map<IndexType, IndexType> pinInverseMap0, pinInverseMap1;

    void removeWire(IndexType rWire)
    {
        _wires.erase(std::remove(_wires.begin(), _wires.end(), rWire), _wires.end());
    }

    void                               addPort(IndexType port)                      { _ports.emplace_back(port); }
    void                               addWire(IndexType wire)                      { _wires.emplace_back(wire); }
    void                               addInv(IndexType inv)                        { _invs.emplace_back(inv); }
    void                               addTG(IndexType tg)                          { _tgs.emplace_back(tg); }
    void                               addTbuf(IndexType tbuf)                      { _tbufs.emplace_back(tbuf); }

    void                               addPin(Pin pin)                              { _pinArray.emplace_back(pin); }
    void                               addCell(Cell const &nCell)                   { _cells.emplace_back(nCell); }
    void                               updPort(IndexType const i, IndexType const j) { _ports[i] = j; }

    //Getters
    std::string const &                name() const                                  { return _name; }
    std::string &                      name()                                        { return _name; }
    IndexType                          id() const                                    { return _nId; }
    IndexType &                        id()                                          { return _nId; }

    const std::vector<IndexType> &     ports() const                                 { return _ports; }
    std::vector<IndexType> &           ports()                                       { return _ports; }
    IndexType                          port(IndexType const i) const                 { return _ports.at(i); }
    IndexType &                        port(IndexType const i)                       { return _ports.at(i); }
    IndexType                          numPorts() const                              { return _ports.size(); }

    const std::vector<IndexType> &     invs() const                                  { return _invs; }
    std::vector<IndexType> &           invs()                                        { return _invs; }
    IndexType                          inv(IndexType const i) const                  { return _invs.at(i); }
    IndexType &                        inv(IndexType const i)                        { return _invs.at(i); }
    IndexType                          numInvs() const                               { return _invs.size(); }

    const std::vector<IndexType> &     tgs() const                                   { return _tgs; }
    std::vector<IndexType> &           tgs()                                         { return _tgs; }
    IndexType                          tg(IndexType const i) const                   { return _tgs.at(i); }
    IndexType &                        tg(IndexType const i)                         { return _tgs.at(i); }
    IndexType                          numTGs() const                                { return _tgs.size(); }

    const std::vector<IndexType> &     tbufs() const                                 { return _tbufs; }
    std::vector<IndexType> &           tbufs()                                       { return _tbufs; }
    IndexType                          tbuf(IndexType const i) const                 { return _tbufs.at(i); }
    IndexType &                        tbuf(IndexType const i)                       { return _tbufs.at(i); }
    IndexType                          numTbufs() const                              { return _tbufs.size(); }

    const std::vector<IndexType> &     wires() const                                 { return _wires; }
    std::vector<IndexType> &           wires()                                       { return _wires; }
    IndexType                          wire(IndexType const i) const                 { return _wires.at(i); }
    IndexType &                        wire(IndexType const i)                       { return _wires.at(i); }
    IndexType                          numWires() const                              { return _wires.size(); }

    const std::vector<Cell> &          cellArray() const                             { return _cells; }
    std::vector<Cell> &                cellArray()                                   { return _cells; }
    const Cell &                       cellArray(IndexType i) const                  { return _cells.at(i); }
    Cell &                             cellArray(IndexType i)                        { return _cells.at(i); }
    IndexType                          numCells() const                              { return _cells.size(); }

    const std::vector<Pin> &           pinArray() const                             { return _pinArray; }
    std::vector<Pin> &                 pinArray()                                   { return _pinArray; }
    const Pin &                        pinArray(IndexType i) const                  { return _pinArray.at(i); }
    Pin &                              pinArray(IndexType i)                        { return _pinArray.at(i); }
    IndexType                          numPins() const                              { return _pinArray.size(); }

    // Setters
    void                               setId(IndexType id)                                 { _nId = id; }
    void                               setName(std::string const &name)                    { _name = name; }

void clear()
    {
        portList.resize(1, false);
        pinName2IdMap.clear();
        _ports.clear();
        _wires.clear();
        _invs.clear();
        _tgs.clear();
        _tbufs.clear();
        _name.clear();
        _cells.clear();
        _pinArray.clear();
    }

private:
    IndexType _nId;
    std::vector<IndexType> _ports, _wires;
    std::vector<IndexType> _invs, _tgs, _tbufs;
    std::string _name;

    //for classification
    std::vector<Cell> _cells;
    std::vector<Pin> _pinArray;
};

PROJECT_NAMESPACE_END

#endif // __NETLIST_H__
