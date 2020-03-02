//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
#ifndef __SUBCKT_H__
#define __SUBCKT_H__

#include <string>
#include <map>
#include <algorithm>
#include "global/global.h"
#include "db/Netlist.h"

PROJECT_NAMESPACE_BEGIN

/// Class for Subckt
/// This class defines struct to store all subcircuit information from input SPICE netlist 

class SubPin //Pins in subckt
{
public: 
    explicit SubPin() = default;
    explicit SubPin(std::string const pName) : _pName(pName) {}
    explicit SubPin(std::string const pName, PinType pType) : _pName(pName), _pType(pType) {}

    //Getters
    std::string const &                 name() const                                  { return _pName; }
    std::string &                       name()                                        { return _pName; }
    PinType                             type() const                                  { return _pType; }
    PinType &                           type()                                        { return _pType; }
    IndexType                           id() const                                    { return _pID; }
    IndexType &                         id()                                          { return _pID; }
    PinDir                              dir() const                                   { return _pDir; }
    PinDir &                            dir()                                         { return _pDir; }
    IndexType                           conns() const                                 { return _pConns; }
    IndexType &                         conns()                                       { return _pConns; }
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
    // Updating port connectivity information - HIERARCHICAL use only
    void                                setConns(IndexType id)                        { _pConns = id; }
    void                                rstConns()                                    { _pConns = 0; }

    bool isPort()
    {
        if (_pType == PinType::PORT)
        {
            return true;
        } else
        {
            return false;
        }
    }

    void clear()
    {
        _pType = PinType::INVALID;
        _pDir = PinDir::INVALID;
        _pName.clear();
        _pConns = 0;
    }

private:
    std::string _pName;
    PinType     _pType; //PORT, NET
    IndexType   _pID;
    PinDir      _pDir; //INPUT, OUTPUT, INOUT
    IndexType   _pConns;
    bool        _power;
    bool        _tieOff = false;
};

class SubNet //Nets within subckt to store connectivity information
{
public: 
    explicit SubNet() = default;
    explicit SubNet(std::string const nName) : _nName(nName) {}
    explicit SubNet(std::string const nName, PinType nType) : _nName(nName), _nType(nType) {}

    void        addInst(IndexType const &instID)  { _instIDs.emplace_back(instID); }

    void clrInsts()
    {
        _instIDs.clear();
    }

    //Getters
    std::string const &                 name() const                                  { return _nName; }
    std::string &                       name()                                        { return _nName; }
    PinType                             type() const                                  { return _nType; }
    PinType &                           type()                                        { return _nType; }
    IndexType                           id() const                                    { return _nID; }
    IndexType &                         id()                                          { return _nID; }
    const std::vector<IndexType> &      instArray() const                             { return _instIDs; }
    std::vector<IndexType> &            instArray()                                   { return _instIDs; }
    IndexType                           instId(IndexType i) const                     { return _instIDs.at(i); }
    IndexType &                         instId(IndexType i)                           { return _instIDs.at(i); }
    IndexType                           numInsts() const                              { return _instIDs.size(); }

    // Setters
    void                                setId(IndexType id)                           { _nID = id; }
    void                                setName(std::string const &name)              { _nName = name; }
    void                                setNetType(PinType type)                      { _nType = type; }

    void clear()
    {
        _nType = PinType::INVALID;
        _nName.clear();
        _instIDs.clear();
    }
private:
    std::string _nName;
    PinType     _nType; //PORT, NET
    IndexType   _nID;
    std::vector<IndexType> _instIDs; //SubPin connections
};

class Inst
{
public: 
    explicit Inst() = default;

    void                                addPin(IndexType const &pinID)                      { _pinIDs.emplace_back(pinID); }
    void                                addFeature(std::string const &feature)              { _iFeatures.emplace_back(feature); }

    void                                updPin(IndexType const pIndex, IndexType const &pinID) { _pinIDs[pIndex] = pinID; }

    //Getters
    std::string const &                 name() const                                  { return _iName; }
    std::string &                       name()                                        { return _iName; }
    IndexType                           id() const                                    { return _id; }
    IndexType &                         id()                                          { return _id; }
    InstType                            type() const                                  { return _iType; }
    InstType &                          type()                                        { return _iType; }
    std::string const &                 cName() const                                 { return _cName; }
    std::string &                       cName()                                       { return _cName; }
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
    bool                                isAnalog()                                    { return _analog; }
    float                               lValue()                                      { return _dlength; }
    float                               wValue()                                      { return _dwidth; }
    IndexType                           fins()                                        { return _dfins; }
    IndexType                           mFactor()                                     { return _mfactor; }
    IndexType                           sFactor()                                     { return _sfactor; }

    // Setters
    void                                setId(IndexType id)                                     { _id = id; }
    void                                setType(InstType type)                                  { _iType = type; }
    void                                setName(std::string const &name)                        { _iName = name; }
    void                                setCompName(std::string const &name)                    { _cName = name; }
    void                                setFeatures(std::vector<std::string> const &features)   { _iFeatures = features; }
    void                                setAnalog()                                             { _analog = true; }
    void                                setNotAnalog()                                          { _analog = false; }
    void                                clrPins()                                               { _pinIDs.clear(); }
    void                                setlValue(float val)                                    { _dlength = val; }
    void                                setwValue(float val)                                    { _dwidth= val; }
    void                                setfins(IndexType fins)                                 { _dfins = fins; }
    void                                setmFactor(IndexType val)                               { _mfactor = val; }
    void                                setsFactor(IndexType val)                               { _sfactor = val; }

    void clear()
    {
        _id = 0;
        _iName.clear();
        _iType = InstType::INVALID;
        _cName.clear();
        _pinIDs.clear();
        _iFeatures.clear();
        _analog = false;
        _dlength = 0.0;
        _dwidth = 0.0;
        _dfins = 0;
        _mfactor = 0;
        _sfactor = 0;
    }

private:
    std::string _iName; //Inst name eg) M0
    InstType _iType; //NFET, PFET, DIODE, RES, CAP, PNP, NPN, COMPONENT
    std::string _cName; //Component name eg) nfet
    IndexType _id;
    std::vector<IndexType> _pinIDs;
    std::vector<std::string> _iFeatures; //Currently not using features - just storing them
    bool _analog;
    //Additional features for FETs to check if ANALOG type
    float _dlength; //Default unit is nanometers 'n'
    float _dwidth; //Default unit is nanometers 'n'
    IndexType _dfins; //No of fins
    IndexType _mfactor; //multiplier factor for parallel connection
    IndexType _sfactor; //multiplier factor for series connection
};

class Subckt
{
public:
    explicit Subckt() = default;

    std::unordered_map<std::string, IndexType> pinName2IdMap;
    std::vector<bool>      portList;

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

    Byte getPortPwr (IndexType idx)
    {
        return (_pwrList[idx]);
    }

    Byte getPortType (IndexType portId)
    {
        std::vector<IndexType>::iterator it = std::find(_ports.begin(), _ports.end(), portId);
        Assert(it != _ports.end());
        IndexType idx = std::distance(_ports.begin(), it);
        return (_pwrList[idx]);
    }

    void setPortType (IndexType portId, Byte portType) //0-NotPower; 1-VDD; 2-VSS
    {
        std::vector<IndexType>::iterator it = std::find(_ports.begin(), _ports.end(), portId);
        Assert(it != _ports.end());
        IndexType idx = std::distance(_ports.begin(), it);
        _pwrList[idx] = portType;
    }

    void setPortPwr (IndexType idx, Byte portType) //0-NotPower; 1-VDD; 2-VSS
    {
        _pwrList[idx] = portType;
    }


    void                                addPort(IndexType port)                       { _ports.emplace_back(port); _pwrList.emplace_back(0); }
    void                                addAnalogInst(IndexType aInst)                { _analogInsts.emplace_back(aInst); }
    void                                addWire(IndexType wire)                       { _wires.emplace_back(wire); }

    void                                addInst(Inst const &nInst)                    { _Insts.emplace_back(nInst); }
    void                                addNet(SubNet const &nNet)                    { _SubNets.emplace_back(nNet); }
    void                                addPin(SubPin const &nPin)                    { _SubPins.emplace_back(nPin); }

    void                                updPort(IndexType const i, IndexType const j) { _ports[i] = j; }

    //Getters
    std::string const &                 name() const                                  { return _cellName; }
    std::string &                       name()                                        { return _cellName; }
    IndexType                           id() const                                    { return _sID; }
    IndexType &                         id()                                          { return _sID; }

    const std::vector<IndexType> &      ports() const                                 { return _ports; }
    std::vector<IndexType> &            ports()                                       { return _ports; }
    IndexType                           port(IndexType const i) const                 { return _ports.at(i); }
    IndexType &                         port(IndexType const i)                       { return _ports.at(i); }
    IndexType                           numPorts() const                              { return _ports.size(); }

    const std::vector<IndexType> &      analogInsts() const                           { return _analogInsts; }
    std::vector<IndexType> &            analogInsts()                                 { return _analogInsts; }
    IndexType                           analogInst(IndexType i) const                 { return _analogInsts.at(i); }
    IndexType &                         analogInst(IndexType i)                       { return _analogInsts.at(i); }
    IndexType                           numAnalogInsts() const                        { return _analogInsts.size(); }

    const std::vector<IndexType> &      wires() const                                 { return _wires; }
    std::vector<IndexType> &            wires()                                       { return _wires; }
    IndexType                           wire(IndexType i) const                       { return _wires.at(i); }
    IndexType &                         wire(IndexType i)                             { return _wires.at(i); }
    IndexType                           numWires() const                              { return _wires.size(); }

    const std::vector<Inst> &          instArray() const                              { return _Insts; }
    std::vector<Inst> &                instArray()                                    { return _Insts; }
    const Inst &                       instArray(IndexType i) const                   { return _Insts.at(i); }
    Inst &                             instArray(IndexType i)                         { return _Insts.at(i); }
    IndexType                          numInsts() const                               { return _Insts.size(); }

    const std::vector<SubNet> &        netArray() const                               { return _SubNets; }
    std::vector<SubNet> &              netArray()                                     { return _SubNets; }
    const SubNet &                     netArray(IndexType i) const                    { return _SubNets.at(i); }
    SubNet &                           netArray(IndexType i)                          { return _SubNets.at(i); }
    IndexType                          numNets() const                                { return _SubNets.size(); }

    const std::vector<SubPin> &        pinArray() const                               { return _SubPins; }
    std::vector<SubPin> &              pinArray()                                     { return _SubPins; }
    const SubPin &                     pinArray(IndexType i) const                    { return _SubPins.at(i); }
    SubPin &                           pinArray(IndexType i)                          { return _SubPins.at(i); }
    IndexType                          numPins() const                                { return _SubPins.size(); }

    const std::vector<Byte> &          pwrListArray() const                           { return _pwrList; }
    std::vector<Byte> &                pwrListArray()                                 { return _pwrList; }
    const Byte &                       pwrList(IndexType i) const                     { return _pwrList.at(i); }
    Byte &                             pwrList(IndexType i)                           { return _pwrList.at(i); }
    IndexType                          numPwrList() const                             { return _pwrList.size(); }

    IndexType                           tnfet() const                                 { return _dCount[0]; }
    IndexType &                         tnfet()                                       { return _dCount[0]; }
    IndexType                           tpfet() const                                 { return _dCount[1]; }
    IndexType &                         tpfet()                                       { return _dCount[1]; }
    IndexType                           tfet() const                                  { return (_dCount[0] + _dCount[1]); }
    IndexType                           tbjt() const                                  { return _dCount[2]; }
    IndexType &                         tbjt()                                        { return _dCount[2]; }
    IndexType                           tdio() const                                  { return _dCount[3]; }
    IndexType &                         tdio()                                        { return _dCount[3]; }
    IndexType                           tres() const                                  { return _dCount[4]; }
    IndexType &                         tres()                                        { return _dCount[4]; }
    IndexType                           tcap() const                                  { return _dCount[5]; }
    IndexType &                         tcap()                                        { return _dCount[5]; }

    IndexType                           nfet() const                                 { return _count[0]; }
    IndexType &                         nfet()                                       { return _count[0]; }
    IndexType                           pfet() const                                 { return _count[1]; }
    IndexType &                         pfet()                                       { return _count[1]; }
    IndexType                           fet() const                                  { return (_count[0] + _count[1]); }
    IndexType                           bjt() const                                  { return _count[2]; }
    IndexType &                         bjt()                                        { return _count[2]; }
    IndexType                           dio() const                                  { return _count[3]; }
    IndexType &                         dio()                                        { return _count[3]; }
    IndexType                           res() const                                  { return _count[4]; }
    IndexType &                         res()                                        { return _count[4]; }
    IndexType                           cap() const                                  { return _count[5]; }
    IndexType &                         cap()                                        { return _count[5]; }
    IndexType                           comp() const                                 { return _count[6]; }
    IndexType &                         comp()                                       { return _count[6]; }

    bool                                isLeaf()                                      { return _leaf; }
    bool                                isMapped()                                    { return _mapped; }
    bool                                isTraversed()                                 { return _traversed; }
    bool                                isAnalog()                                    { return _analog; }

    // Setters
    void                                setId(IndexType const id)                                   { _sID = id; }
    void                                setName(std::string const &name)                            { _cellName = name; }
    void                                setLeaf()                                                   { _leaf = true; }
    void                                setNotLeaf()                                                { _leaf = false; }
    void                                setMapped()                                                 { _mapped = true; }
    void                                setNotMapped()                                              { _mapped = false; }
    void                                setTraversed()                                              { _traversed = true; }
    void                                setNotTraversed()                                           { _traversed = false; }
    void                                setAnalog()                                                 { _analog = true; }
    void                                setNotAnalog()                                              { _analog = false; }
    void                                clrAnalogInsts()                                            { _analogInsts.clear(); }
    void                                updateAnalogInsts(std::vector<IndexType> const &in)         { _analogInsts = in; }
    void                                clrInsts()                                                  { _Insts.clear(); }
    void                                updateInsts(std::vector<Inst> const &inst)                  { _Insts = inst; }
    void                                updateInst(Inst const &inst, IndexType const instID)        { _Insts[instID] = inst; }
    void                                updatePort(IndexType const port, IndexType const location)  { _ports[location] = port; }

    void                                setTNFET(IndexType const cnt)                             { _dCount[0] = cnt; }
    void                                setTPFET(IndexType const cnt)                             { _dCount[1] = cnt; }
    void                                setTBJT(IndexType const cnt)                              { _dCount[2] = cnt; }
    void                                setTDIO(IndexType const cnt)                              { _dCount[3] = cnt; }
    void                                setTRES(IndexType const cnt)                              { _dCount[4] = cnt; }
    void                                setTCAP(IndexType const cnt)                              { _dCount[5] = cnt; }
    void                                rstTNFET()                                                { _dCount[0] = 0; }
    void                                rstTPFET()                                                { _dCount[1] = 0; }
    void                                rstTBJT()                                                 { _dCount[2] = 0; }
    void                                rstTDIO()                                                 { _dCount[3] = 0; }
    void                                rstTRES()                                                 { _dCount[4] = 0; }
    void                                rstTCAP()                                                 { _dCount[5] = 0; }

    void                                setNFET(IndexType const cnt)                              { _count[0] = cnt; }
    void                                setPFET(IndexType const cnt)                              { _count[1] = cnt; }
    void                                setBJT(IndexType const cnt)                               { _count[2] = cnt; }
    void                                setDIO(IndexType const cnt)                               { _count[3] = cnt; }
    void                                setRES(IndexType const cnt)                               { _count[4] = cnt; }
    void                                setCAP(IndexType const cnt)                               { _count[5] = cnt; }
    void                                setCOMP(IndexType const cnt)                              { _count[6] = cnt; }
    void                                incrNFET()                                                { _count[0]++; }
    void                                incrPFET()                                                { _count[1]++; }
    void                                incrBJT()                                                 { _count[2]++; }
    void                                incrDIO()                                                 { _count[3]++; }
    void                                incrRES()                                                 { _count[4]++; }
    void                                incrCAP()                                                 { _count[5]++; }
    void                                incrCOMP()                                                { _count[6]++; }
    void                                rstNFET()                                                 { _count[0] = 0; }
    void                                rstPFET()                                                 { _count[1] = 0; }
    void                                rstBJT()                                                  { _count[2] = 0; }
    void                                rstDIO()                                                  { _count[3] = 0; }
    void                                rstRES()                                                  { _count[4] = 0; }
    void                                rstCAP()                                                  { _count[5] = 0; }
    void                                rstCOMP()                                                 { _count[6] = 0; }

    void clear()
    {
        pinName2IdMap.clear();
        portList.resize(1, false);
        _ports.clear();
        _pwrList.clear();
        _cellName.clear();
        _analogInsts.clear();
        _wires.clear();
        _Insts.clear();
        _SubNets.clear();
        _SubPins.clear();
        _leaf = false;
        _mapped = false;
        _traversed = false;
        _analog = false;
        std::fill( std::begin(_dCount), std::end(_dCount), 0 );
        std::fill( std::begin(_count), std::end(_count), 0 );
    }

private:
    IndexType _sID;
    std::string _cellName;
    bool _leaf; //Has atleast one device
    bool _mapped; //All FETs mapped to logic gates
    bool _traversed;
    bool _analog; //True if SRAM or ANALOG cell
    std::vector<Byte>      _pwrList;
    std::vector<IndexType> _ports;

    //for classification
    std::vector<IndexType> _analogInsts;
    std::vector<IndexType> _wires;

    std::vector<Inst> _Insts;
    std::vector<SubPin> _SubPins;
    std::vector<SubNet> _SubNets;

    //device count
    std::array<IndexType, 6> _dCount; //NMOS, PMOS, BJT, Diode, Resistor, Capacitor
    std::array<IndexType, 7> _count; //local count - NMOS, PMOS, BJT, Diode, Resistor, Capacitor, Component
};

//Sorting vector of subckts based on size of instances for library
struct largerSize
{
    inline bool operator() (const Subckt& ckt1, const Subckt& ckt2)
    {
        return (ckt1.numInsts() > ckt2.numInsts());
    }
};


PROJECT_NAMESPACE_END

#endif // __SUBCKT_H__
