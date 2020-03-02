//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <unordered_map>
#include "db/FileInfo.h"
#include "db/Netlist.h"
#include "db/Subckt.h"
#include "util/utility.h"
#include "util/Boost_graph_wrapper.h"
#include "parser/BookshelfDatabase.h"
#include <boost/algorithm/string.hpp>

//DEBUG statement
#define debugDatabaseh 0

//Variables to be set for logic reduction techniques
static constexpr bool       Parse_DeviceReduction = true; 
//Options for Device Reduction
static constexpr bool       Parse_ParallelReduction = true;
//Settings for Series Reduction
static constexpr bool       Parse_SeriesReductionRES = false; //affects port cardinality
static constexpr bool       Parse_SeriesReductionCAP = false;
static constexpr bool       Parse_SeriesReductionFET = false;

PROJECT_NAMESPACE_BEGIN

/// The database of the whole placement engine
class Database : public BookshelfParser::BookshelfDatabase
{
public:
    explicit Database() = default;

    // Updating library information
    void                                     libraryUpdate(std::string const &libPath);

    // Parsing top function
    void                                     parse(std::string const &auxPath);
    //Identify leaf subckts for flattening and write out cell count in CSV
    void                                     analyzeNetlist();
    //Propagate power ports and ports in unique instantiations of subckts
    void                                     portPropagation();
    //Flatten subckts
    void                                     leafFlatten();
    void                                     flattenSubckt(Subckt &newckt, 
                                                           Module &nl, 
                                                           IndexType &cellIndex);
    void                                     deepFlatten(Subckt &instCell,
                                                         Subckt &temp,
                                                         Subckt &newckt,
                                                         Module &nl, 
                                                         IndexType const prfx,
                                                         IndexType const i,
                                                         IndexType &tempInst,
                                                         IndexType plevel,
                                                         IndexType level,
                                                         std::unordered_map<std::string, std::string> &cell2InstPortMap,
                                                         std::unordered_map<std::string, std::string> &l0PortMap);
    //Extract Logic gates for hierarchical SPICE netlist
    void                                     hierExtraction();
    //Port direction update for non-leaf subckts
    void                                     portDirUpdate();
    //Write out verilog file
    void                                     outVerilog(bool const spOut, std::string const &spiLib);
    //Write out dot file to plot using Graphviz
    void                                     dotOut();

private:
    //////////////////////////////////
    //                              //
    //  Parser Call Back Functions  //
    //                              //
    //////////////////////////////////

    /* parsing SPICE file */
    void  createCellCbk(std::string const &Cname);
    void  addPortCbk(std::string const &port);
    void  addfeatureCbk(std::string const &ftr);
    void  endofcellCbk(); 

    /* MOS information */
    void  addMOSCbk(std::string const &Mname, std::string const &Drain, std::string const &Gate, 
                    std::string const &Source, std::string const &Bulk, std::string const &Mtype);

    /* DIO information */
    void  addDioCbk(std::string const &Dname, std::string const &DIn, std::string const &DOut, std::string const &DType);

    /* BJT information */
    void  addBjtCbk(std::string const &Bname, std::string const &Collector, std::string const &Base, 
                    std::string const &Emitter, std::string const &BType);

    /* COMP information */
    void  addCompCbk(std::string const &Comp, std::string const &CType);
    void  addCompPortCbk(std::string const &Cport);

    /* RES information */
    void  addResCbk(std::string const &Res, std::string const &DIn, std::string const &DOut);
    void  addResSCbk(std::string const &Res, std::string const &DIn, std::string const &DOut, std::string const &ResName);
    void  addResNCbk(std::string const &Res, std::string const &DIn, std::string const &DOut, std::string const &Dpwr, std::string const &ResName);

    /* CAP information */
    void  addCapCbk(std::string const &Cap, std::string const &CIn, std::string const &COut);
    void  addCapSCbk(std::string const &Cap, std::string const &CIn, std::string const &COut, std::string const &CapName);
    void  addCapNCbk(std::string const &Cap, std::string const &CIn, std::string const &COut, std::string const &Cpwr, std::string const &CapName);

private:
    FileInfo                                _fileInfo;
    Subckt                                  _newckt;
    Module                                  _nl;
    Inst                                    _inst;
    std::vector<Module>                     _netlistArray;
    std::vector<Subckt>                     _subcktArray;
    std::vector<IndexType>                  _leafSubckts, _nonLeaves;

    //Port Propagation
    GraphP                                                  _gPort; //For port propagation
    std::unordered_map<IndexType, std::vector<IndexType> >  _gPortMap; //Subckt ID to instances in _gPort
    
    //Subckt information storage
    std::unordered_map<std::string, IndexType> _subcktToIdMap;
    bool                                        endSubckt(); //true - end of cell; false - otherwise;
};

///////////////////////////////////////
//                                   //
//  Some Parser Call Back Functions  //
//                                   //
///////////////////////////////////////

inline bool Database::endSubckt() // end of cell
{
    bool ret = false;

    if (_newckt.numInsts() > 0) 
    { 
        for (IndexType pt=0; pt < _newckt.numPorts(); ++pt)
        {
            std::vector<IndexType> iList(_newckt.netArray(_newckt.port(pt)).instArray());
            std::sort( iList.begin(), iList.end() );
            iList.erase( std::unique( iList.begin(), iList.end() ), iList.end() );
            IndexType conval(_newckt.pinArray(_newckt.port(pt)).conns());
            if (conval > 0)
            {
                _newckt.pinArray(_newckt.port(pt)).setConns(conval + iList.size()-1);
            } else
            {
                _newckt.pinArray(_newckt.port(pt)).setConns(iList.size());
            }
        }

        //Update device count
        _newckt.setTNFET(_newckt.tnfet() + _newckt.nfet());
        _newckt.setTPFET(_newckt.tpfet() + _newckt.pfet());
        _newckt.setTBJT(_newckt.tbjt() + _newckt.bjt());
        _newckt.setTDIO(_newckt.tdio() + _newckt.dio());
        _newckt.setTRES(_newckt.tres() + _newckt.res());
        _newckt.setTCAP(_newckt.tcap() + _newckt.cap());

        //Update array only for subckts with instances
        _subcktArray.emplace_back(_newckt);
        _netlistArray.emplace_back(Module(_newckt.name()));
        _subcktToIdMap[_newckt.name()] = _subcktArray.size() - 1;

        ret = true;
    }
    _newckt.clear();
    _inst.clear();
    _newckt.portList.resize(1000000, false);
    return ret;
}

/// Call back function to update Capacitor
inline void Database::addCapCbk(std::string const &Cap, std::string const &CIn, std::string const &COut)
{
    _inst.setName("C" + toString(_newckt.numInsts()));
    _inst.setCompName("capacitor");
    _inst.setType(InstType::CAP);
    _inst.setId(_newckt.numInsts());
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    //COut
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(COut) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[COut];
        cNet = _newckt.pinName2IdMap[COut];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(COut);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(COut);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //CIn
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(CIn) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[CIn];
        bNet = _newckt.pinName2IdMap[CIn];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(CIn);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(CIn);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(cPin);
    _inst.addPin(bPin);

    _inst.setmFactor(1);
    _inst.setsFactor(1);

    bool foundParallel(false);
    bool foundSeries(false);

    if (Parse_DeviceReduction)
    {
        //Device Reduction
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == InstType::CAP) && (_newckt.instArray(i).cName() == _inst.cName()))
            {
                //Check for Parallel connections
                if (Parse_ParallelReduction)
                {
                    if ( (_newckt.instArray(i).pinId(0) == bPin)  &&  (_newckt.instArray(i).pinId(1) == cPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    }
                }
                //Check for Series connections
                if (Parse_SeriesReductionCAP && (_newckt.instArray(i).mFactor() == 1))
                {
                    //Series reduction only if 2 pin net
                    if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) != bPin) && 
                         (_newckt.netArray(cNet).numInsts() <= 2) ) 
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin) && 
                              (_newckt.netArray(bNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    }
                }
            }
        }
    } 

    if ((!foundParallel && !foundSeries) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(_newckt.pinName2IdMap[COut]).addInst(_inst.id());
        _newckt.netArray(_newckt.pinName2IdMap[CIn]).addInst(_inst.id());

        if (_newckt.pinArray(_newckt.pinName2IdMap[COut]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[COut]).setPinDir(PinDir::OUTPUT);
        }
        if (_newckt.pinArray(_newckt.pinName2IdMap[CIn]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[CIn]).setPinDir(PinDir::INPUT);
        }

        //Update wire information
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            }
        } else 
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
        }

        _newckt.addInst(_inst);
        _newckt.incrCAP();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }

    _inst.clear();
}

inline void Database::addCapSCbk(std::string const &Cap, std::string const &CIn, std::string const &COut, std::string const &CapName)
{
    _inst.setName("C" + toString(_newckt.numInsts()));
    _inst.setCompName(CapName);
    _inst.setType(InstType::CAP);
    _inst.setId(_newckt.numInsts());
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    //COut
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(COut) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[COut];
        cNet = _newckt.pinName2IdMap[COut];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(COut);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(COut);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //CIn
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(CIn) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[CIn];
        bNet = _newckt.pinName2IdMap[CIn];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(CIn);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(CIn);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(cPin);
    _inst.addPin(bPin);

    _inst.setmFactor(1);
    _inst.setsFactor(1);

    bool foundParallel(false);
    bool foundSeries(false);

    if (Parse_DeviceReduction)
    {
        //Device Reduction
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == InstType::CAP) && (_newckt.instArray(i).cName() == _inst.cName()))
            {
                //Check for Parallel connections
                if (Parse_ParallelReduction)
                {
                    if ( (_newckt.instArray(i).pinId(0) == bPin) && (_newckt.instArray(i).pinId(1) == cPin))
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    }
                }
                //Check for Series connections for two pin nets only
                if (Parse_SeriesReductionCAP && (_newckt.instArray(i).mFactor() == 1))
                {
                    if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) != bPin) &&
                         (_newckt.netArray(cNet).numInsts() <= 2))
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin) && 
                            (_newckt.netArray(bNet).numInsts() <= 2))
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    }
                }
            }
        }
    } 
    if ((!foundParallel && !foundSeries) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(cNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());
        if (_newckt.pinArray(_newckt.pinName2IdMap[COut]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[COut]).setPinDir(PinDir::OUTPUT);
        }
        if (_newckt.pinArray(_newckt.pinName2IdMap[CIn]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[CIn]).setPinDir(PinDir::INPUT);
        }

        //Update wire information
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            }
        } else 
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
        }

        _newckt.addInst(_inst);
        _newckt.incrCAP();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }
    _inst.clear();
}

inline void Database::addCapNCbk(std::string const &Cap, std::string const &CIn, std::string const &COut, std::string const &Cpwr, std::string const &CapName)
{
    _inst.setName("C" + toString(_newckt.numInsts()));
    _inst.setCompName(CapName);
    _inst.setType(InstType::CAP);
    _inst.setId(_newckt.numInsts());
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    //COut
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(COut) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[COut];
        cNet = _newckt.pinName2IdMap[COut];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(COut);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(COut);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //CIn
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(CIn) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[CIn];
        bNet = _newckt.pinName2IdMap[CIn];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(CIn);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(CIn);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //Cpwr
    IndexType pPin(0), pNet(0);

    bool pNew(true);

    if (_newckt.pinName2IdMap.find(Cpwr) != _newckt.pinName2IdMap.end())
    {
        pPin = _newckt.pinName2IdMap[Cpwr];
        pNet = _newckt.pinName2IdMap[Cpwr];
        pNew = false;
    } else 
    {
        SubPin newPin;
        newPin.setName(Cpwr);
        newPin.setId(_newckt.numPins());
        pPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(Cpwr);
        newNet.setId(_newckt.numNets());
        pNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(cPin);
    _inst.addPin(bPin);
    _inst.addPin(pPin);

    _inst.setmFactor(1);
    _inst.setsFactor(1);

    bool foundParallel(false);
    bool foundSeries(false);

    if (Parse_DeviceReduction)
    {
        //Device Reduction
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == InstType::CAP) && (_newckt.instArray(i).cName() == _inst.cName()))
            {
                //Check for Parallel connections
                if (Parse_ParallelReduction)
                {
                    if ( (_newckt.instArray(i).pinId(0) == bPin) && (_newckt.instArray(i).pinId(1) == cPin) &&
                            (_newckt.instArray(i).pinId(2) == pPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    }
                }
                //Check for Series connections for two pin nets only
                if (Parse_SeriesReductionCAP && (_newckt.instArray(i).mFactor() == 1))
                {
                    if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) != bPin) &&
                            (_newckt.instArray(i).pinId(2) == pPin)  && (_newckt.netArray(cNet).numInsts() <= 2))
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin) && 
                            (_newckt.netArray(bNet).numInsts() <= 2) && (_newckt.instArray(i).pinId(2) == pPin) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    }
                }
            }
        }
    } 
    if ((!foundParallel && !foundSeries) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(cNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());
        _newckt.netArray(pNet).addInst(_inst.id());
        if (_newckt.pinArray(_newckt.pinName2IdMap[COut]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[COut]).setPinDir(PinDir::OUTPUT);
        }
        if (_newckt.pinArray(_newckt.pinName2IdMap[CIn]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[CIn]).setPinDir(PinDir::INPUT);
        }
        _newckt.pinArray(_newckt.pinName2IdMap[Cpwr]).setPinDir(PinDir::INOUT);

        //Update wire information
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            }
            if (_newckt.portList[pPin] == false)
            {
                _newckt.addWire(pPin);
            }
        } else 
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
            _newckt.addWire(pPin);
        }

        _newckt.addInst(_inst);
        _newckt.incrCAP();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (pNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }
    _inst.clear();
}

/// Call back function to update Resistor
inline void Database::addResCbk(std::string const &Res, std::string const &DIn, std::string const &DOut)
{
    _inst.setName("R" + toString(_newckt.numInsts()));
    _inst.setCompName("resistor");
    _inst.setType(InstType::RES);
    _inst.setId(_newckt.numInsts());
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    //DOut
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(DOut) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[DOut];
        cNet = _newckt.pinName2IdMap[DOut];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DOut);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DOut);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //DIn
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(DIn) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[DIn];
        bNet = _newckt.pinName2IdMap[DIn];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DIn);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DIn);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(cPin);
    _inst.addPin(bPin);

    _inst.setmFactor(1);
    _inst.setsFactor(1);

    bool foundParallel(false);
    bool foundSeries(false);

    if (Parse_DeviceReduction)
    {
        //Device Reduction
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == InstType::RES) && (_newckt.instArray(i).cName() == _inst.cName()))
            {
                //Check for Parallel connections
                if (Parse_ParallelReduction)
                {
                    if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) == bPin)  &&  (_newckt.instArray(i).pinId(1) == cPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    }
                }
                //Check for Series connections
                if (Parse_SeriesReductionRES && (_newckt.instArray(i).mFactor() == 1))
                {
                    if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) != bPin) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) == bPin)  &&  (_newckt.instArray(i).pinId(1) != cPin) && 
                            (_newckt.netArray(bNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != bPin)  &&  (_newckt.instArray(i).pinId(1) == cPin) &&
                            (_newckt.netArray(cNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    }
                }
            }
        }
    } 

    if ((!foundParallel && !foundSeries) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(cNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());
        if (_newckt.pinArray(_newckt.pinName2IdMap[DOut]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DOut]).setPinDir(PinDir::OUTPUT);
        }
        if (_newckt.pinArray(_newckt.pinName2IdMap[DIn]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DIn]).setPinDir(PinDir::INPUT);
        }

        //Update wire information
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            }
        } else 
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
        }

        _newckt.addInst(_inst);
        _newckt.incrRES();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }

    _inst.clear();
}

inline void Database::addResSCbk(std::string const &Res, std::string const &DIn, std::string const &DOut, std::string const &ResName)
{
    _inst.setName("R" + toString(_newckt.numInsts()));
    _inst.setCompName(ResName);
    _inst.setType(InstType::RES);
    _inst.setId(_newckt.numInsts());
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    //DOut
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(DOut) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[DOut];
        cNet = _newckt.pinName2IdMap[DOut];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DOut);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DOut);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //DIn
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(DIn) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[DIn];
        bNet = _newckt.pinName2IdMap[DIn];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DIn);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DIn);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(bPin);
    _inst.addPin(cPin);

    _inst.setmFactor(1);
    _inst.setsFactor(1);

    bool foundParallel(false);
    bool foundSeries(false);

    if (Parse_DeviceReduction)
    {
        //Device Reduction
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == InstType::RES) && (_newckt.instArray(i).cName() == _inst.cName()))
            {
                //Check for Parallel connections
                if (Parse_ParallelReduction)
                {
                    if ( (_newckt.instArray(i).pinId(0) == bPin) && (_newckt.instArray(i).pinId(1) == cPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) == cPin) && (_newckt.instArray(i).pinId(1) == bPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    } 
                }
                //Check for Series connections
                if (Parse_SeriesReductionRES && (_newckt.instArray(i).mFactor() == 1))
                {
                    if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) != bPin) &&
                          (_newckt.netArray(cNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin) &&
                              (_newckt.netArray(bNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) == bPin)  &&  (_newckt.instArray(i).pinId(1) != cPin) && 
                              (_newckt.netArray(bNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != bPin)  &&  (_newckt.instArray(i).pinId(1) == cPin) &&
                              (_newckt.netArray(cNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    }
                }
            }
        }
    } 

    if ((!foundParallel && !foundSeries) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(cNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());
        if (_newckt.pinArray(_newckt.pinName2IdMap[DOut]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DOut]).setPinDir(PinDir::OUTPUT);
        }
        if (_newckt.pinArray(_newckt.pinName2IdMap[DIn]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DIn]).setPinDir(PinDir::INPUT);
        }

        //Update wire information
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            }
        } else 
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
        }

        _newckt.addInst(_inst);
        _newckt.incrRES();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }
    _inst.clear();
}

inline void Database::addResNCbk(std::string const &Res, std::string const &DIn, std::string const &DOut, std::string const &Dpwr, std::string const &ResName)
{
    _inst.setName("R" + toString(_newckt.numInsts()));
    _inst.setCompName(ResName);
    _inst.setType(InstType::RES);
    _inst.setId(_newckt.numInsts());
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    //DOut
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(DOut) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[DOut];
        cNet = _newckt.pinName2IdMap[DOut];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DOut);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DOut);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //DIn
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(DIn) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[DIn];
        bNet = _newckt.pinName2IdMap[DIn];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DIn);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DIn);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //Dpwr
    IndexType pPin(0), pNet(0);

    bool pNew(true);

    if (_newckt.pinName2IdMap.find(Dpwr) != _newckt.pinName2IdMap.end())
    {
        pPin = _newckt.pinName2IdMap[Dpwr];
        pNet = _newckt.pinName2IdMap[Dpwr];
        pNew = false;
    } else 
    {
        SubPin newPin;
        newPin.setName(Dpwr);
        newPin.setId(_newckt.numPins());
        pPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(Dpwr);
        newNet.setId(_newckt.numNets());
        pNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(bPin);
    _inst.addPin(cPin);
    _inst.addPin(pPin);

    _inst.setmFactor(1);
    _inst.setsFactor(1);

    bool foundParallel(false);
    bool foundSeries(false);

    if (Parse_DeviceReduction)
    {
        //Device Reduction
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == InstType::RES) && (_newckt.instArray(i).cName() == _inst.cName()))
            {
                //Check for Parallel connections
                if (Parse_ParallelReduction)
                {
                    if ( (_newckt.instArray(i).pinId(0) == bPin) && (_newckt.instArray(i).pinId(1) == cPin) &&
                            (_newckt.instArray(i).pinId(2) == pPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) == cPin) && (_newckt.instArray(i).pinId(1) == bPin) &&
                            (_newckt.instArray(i).pinId(2) == pPin) )
                    {
                        foundParallel = true;
                        _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                    } 
                }
                //Check for Series connections
                if (Parse_SeriesReductionRES && (_newckt.instArray(i).mFactor() == 1))
                {
                    if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) != bPin) &&
                            (_newckt.instArray(i).pinId(2) == pPin) && (_newckt.netArray(cNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin) &&
                            (_newckt.instArray(i).pinId(2) == pPin) && (_newckt.netArray(bNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) == bPin)  &&  (_newckt.instArray(i).pinId(1) != cPin) && 
                            (_newckt.instArray(i).pinId(2) == pPin) && (_newckt.netArray(bNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(0) = cPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    } else if ( (_newckt.instArray(i).pinId(0) != bPin)  &&  (_newckt.instArray(i).pinId(1) == cPin) &&
                            (_newckt.instArray(i).pinId(2) == pPin) && (_newckt.netArray(cNet).numInsts() <= 2) )
                    {
                        foundSeries = true;
                        _newckt.instArray(i).pinId(1) = bPin;
                        _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                    }
                }
            }
        }
    } 

    if ((!foundParallel && !foundSeries) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(cNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());
        _newckt.netArray(pNet).addInst(_inst.id());
        if (_newckt.pinArray(_newckt.pinName2IdMap[DOut]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DOut]).setPinDir(PinDir::OUTPUT);
        }
        if (_newckt.pinArray(_newckt.pinName2IdMap[DIn]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DIn]).setPinDir(PinDir::INPUT);
        }
        _newckt.pinArray(_newckt.pinName2IdMap[Dpwr]).setPinDir(PinDir::INOUT);

        //Update wire information
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            }
            if (_newckt.portList[pPin] == false)
            {
                _newckt.addWire(pPin);
            }
        } else 
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
            _newckt.addWire(pPin);
        }

        _newckt.addInst(_inst);
        _newckt.incrRES();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (pNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }
    _inst.clear();
}

/// Call back function to update Component Port
inline void Database::addCompPortCbk(std::string const &Cport)
{
    //Cport
    IndexType cPin(0), cNet(0);

    if (_newckt.pinName2IdMap.find(Cport) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[Cport];
        cNet = _newckt.pinName2IdMap[Cport];
    } else
    {
        SubPin newPin;
        newPin.setName(Cport);
        newPin.setId(_newckt.numPins());
        _newckt.addPin(newPin);
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);
        _newckt.pinArray(newPin.id()).setPinDir(PinDir::INVALID);
        cPin = newPin.id();

        SubNet newNet;
        newNet.setName(Cport);
        newNet.setId(_newckt.numNets());
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
        cNet = newNet.id();
    }

    _inst.addPin(cPin);

    //if Pin is not port - add as wire
    if (_newckt.numPorts() > 0)
    {
        if (_newckt.portList[cPin] == false)
        {
            _newckt.addWire(cPin);
        } 
    } else
    {
        _newckt.addWire(cPin);
    }
}

/// Call back function to update Component inst/type
inline void Database::addCompCbk(std::string const &Comp, std::string const &CType)
{
    _inst.setName("X" + toString(_newckt.numInsts()));
    _inst.setCompName(CType);
    _inst.setType(InstType::COMPONENT);
    _inst.setId(_newckt.numInsts());

    if ((CType.find("sram") != std::string::npos) || (CType.find("ANALOG") != std::string::npos)) 
    {
        _inst.setAnalog();
        _newckt.addAnalogInst(_inst.id());
    } else
    {
        _inst.setNotAnalog();
    }

    _newckt.addInst(_inst);
    _newckt.incrCOMP();

    //update port direction for component
    if (_subcktToIdMap.find(CType) != _subcktToIdMap.end())
    {
        for (IndexType i=0; i < _inst.numPins(); ++i)
        {
            //Port dir update
            _newckt.pinArray(_newckt.instArray(_inst.id()).pinId(i)).setPinDir(_subcktArray[_subcktToIdMap[CType]].pinArray(_subcktArray[_subcktToIdMap[CType]].port(i)).dir());

            //Update inst count for net
            _newckt.netArray(_newckt.instArray(_inst.id()).pinId(i)).addInst(_inst.id());

            //update for port cardinality
            if (_newckt.pinArray(_newckt.instArray(_inst.id()).pinId(i)).type() == PinType::PORT) 
            {
                if (_newckt.pinArray(_newckt.instArray(_inst.id()).pinId(i)).conns() > 0)
                {
                    _newckt.pinArray(_newckt.instArray(_inst.id()).pinId(i)).setConns(_newckt.pinArray(_newckt.instArray(_inst.id()).pinId(i)).conns() + _subcktArray[_subcktToIdMap[CType]].pinArray(_subcktArray[_subcktToIdMap[CType]].port(i)).conns() -1);
                } else
                {
                    _newckt.pinArray(_newckt.instArray(_inst.id()).pinId(i)).setConns(_subcktArray[_subcktToIdMap[CType]].pinArray(_subcktArray[_subcktToIdMap[CType]].port(i)).conns());
                }
            }
        }
        //update number of devices
        _newckt.setTNFET(_newckt.tnfet() + _subcktArray[_subcktToIdMap[CType]].tnfet());
        _newckt.setTPFET(_newckt.tpfet() + _subcktArray[_subcktToIdMap[CType]].tpfet());
        _newckt.setTBJT(_newckt.tbjt() + _subcktArray[_subcktToIdMap[CType]].tbjt());
        _newckt.setTDIO(_newckt.tdio() + _subcktArray[_subcktToIdMap[CType]].tdio());
        _newckt.setTRES(_newckt.tres() + _subcktArray[_subcktToIdMap[CType]].tres());
        _newckt.setTCAP(_newckt.tcap() + _subcktArray[_subcktToIdMap[CType]].tcap());

    } 
    _inst.clear();
}

/// Call back function to update BJT inst/conns/type
inline void Database::addBjtCbk(std::string const &Bname, std::string const &Collector, std::string const &Base, std::string const &Emitter, std::string const &BType)
{
    _inst.setName("Q" + toString(_newckt.numInsts()));
    _inst.setCompName(BType);
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    if (BType.find("npn") == std::string::npos) 
    {
        _inst.setType(InstType::PNP);
    } else
    {
        _inst.setType(InstType::NPN);
    }

    _inst.setId(_newckt.numInsts());

    //Collector
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(Collector) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[Collector];
        cNet = _newckt.pinName2IdMap[Collector];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(Collector);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(Collector);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //Base
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(Base) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[Base];
        bNet = _newckt.pinName2IdMap[Base];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(Base);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(Base);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //Emitter
    IndexType ePin(0), eNet(0);

    bool eNew(true);

    if (_newckt.pinName2IdMap.find(Emitter) != _newckt.pinName2IdMap.end())
    {
        ePin = _newckt.pinName2IdMap[Emitter];
        eNet = _newckt.pinName2IdMap[Emitter];
        eNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(Emitter);
        newPin.setId(_newckt.numPins());
        ePin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(Emitter);
        newNet.setId(_newckt.numNets());
        eNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(cPin);
    _inst.addPin(bPin);
    _inst.addPin(ePin);

    //Update wire
    if (_newckt.numPorts() > 0)
    {
        //if Pin is not port - add as wire
        if (_newckt.portList[cPin] == false)
        {
            _newckt.addWire(cPin);
        }
        if (_newckt.portList[bPin] == false)
        {
            _newckt.addWire(bPin);
        }
        if (_newckt.portList[ePin] == false)
        {
            _newckt.addWire(ePin);
        }
    } else
    {
        _newckt.addWire(cPin);
        _newckt.addWire(bPin);
        _newckt.addWire(ePin);
    }

    _inst.setmFactor(1);
    _inst.setsFactor(0);

    bool foundParallel(false);

    if (Parse_DeviceReduction && Parse_ParallelReduction)
    {
        bool foundParallel(false);
        //Merge parallel connections
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == _inst.type()) && (_newckt.instArray(i).cName() ==  _inst.cName()))
            {
                if ( (_newckt.instArray(i).pinId(0) == cPin)  &&  (_newckt.instArray(i).pinId(1) == bPin)  &&  (_newckt.instArray(i).pinId(2) == ePin))
                {
                    foundParallel = true;
                    _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                }
            }
        }
    } 

    if ((!foundParallel) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(cNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());
        _newckt.netArray(eNet).addInst(_inst.id());
        _newckt.pinArray(_newckt.pinName2IdMap[Collector]).setPinDir(PinDir::OUTPUT);
        _newckt.pinArray(_newckt.pinName2IdMap[Base]).setPinDir(PinDir::INPUT);
        _newckt.pinArray(_newckt.pinName2IdMap[Emitter]).setPinDir(PinDir::INOUT);

        //Update wire
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            }
            if (_newckt.portList[ePin] == false)
            {
                _newckt.addWire(ePin);
            }
        } else
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
            _newckt.addWire(ePin);
        }

        _newckt.addInst(_inst);
        _newckt.incrBJT();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (eNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }
    _inst.clear();
}

/// Call back function to update Diode inst/conns/type
inline void Database::addDioCbk(std::string const &Dname, std::string const &DIn, std::string const &DOut, std::string const &DType)
{
    _inst.setName("D" + toString(_newckt.numInsts()));
    _inst.setCompName(DType);
    _inst.setType(InstType::DIODE);
    _inst.setId(_newckt.numInsts());
    _inst.setAnalog();
    _newckt.addAnalogInst(_inst.id());

    //DOut
    IndexType cPin(0), cNet(0);

    bool cNew(true);

    if (_newckt.pinName2IdMap.find(DOut) != _newckt.pinName2IdMap.end())
    {
        cPin = _newckt.pinName2IdMap[DOut];
        cNet = _newckt.pinName2IdMap[DOut];
        cNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DOut);
        newPin.setId(_newckt.numPins());
        cPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DOut);
        newNet.setId(_newckt.numNets());
        cNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    //DIn
    IndexType bPin(0), bNet(0);

    bool bNew(true);

    if (_newckt.pinName2IdMap.find(DIn) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[DIn];
        bNet = _newckt.pinName2IdMap[DIn];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(DIn);
        newPin.setId(_newckt.numPins());
        bPin = newPin.id();
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);

        SubNet newNet;
        newNet.setName(DIn);
        newNet.setId(_newckt.numNets());
        bNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
    }

    _inst.addPin(bPin);
    _inst.addPin(cPin);

    _inst.setmFactor(1);
    _inst.setsFactor(0);

    bool foundParallel(false);

    if (Parse_DeviceReduction && Parse_ParallelReduction)
    {
        //Merge parallel connections
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == InstType::DIODE) && (_newckt.instArray(i).cName() == _inst.cName()))
            {
                if ( (_newckt.instArray(i).pinId(0) == bPin)  &&  (_newckt.instArray(i).pinId(1) == cPin) )
                {
                    foundParallel = true;
                    _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                } 
            }
        }
    } 

    if ((!foundParallel) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(cNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());
        if (_newckt.pinArray(_newckt.pinName2IdMap[DOut]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DOut]).setPinDir(PinDir::OUTPUT);
        }
        if (_newckt.pinArray(_newckt.pinName2IdMap[DIn]).dir() == PinDir::INVALID)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[DIn]).setPinDir(PinDir::INPUT);
        }

        //Update wire
        if (_newckt.numPorts() > 0)
        {
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            } 
            if (_newckt.portList[cPin] == false)
            {
                _newckt.addWire(cPin);
            } 
        } else
        {
            _newckt.addWire(cPin);
            _newckt.addWire(bPin);
        }

        _newckt.addInst(_inst);
        _newckt.incrDIO();
    } else
    {
        //Delete redundant new entries of pin/net created
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (cNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }
    _inst.clear();
}

/// End of cell
inline void Database::endofcellCbk() 
{
    bool val = endSubckt();
    if(debugDatabaseh && (val == true))
    {
        std::cout << "End of subckt: Added cell: " << _subcktArray[_subcktToIdMap.size()-1].name()
                  << " and mapped as element " << _subcktToIdMap[_subcktArray[_subcktToIdMap.size()-1].name()] << std::endl;
    }
}

/// Call back function to create new Cell
inline void Database::createCellCbk(std::string const &Cname)
{
    _newckt.setName(Cname);
    _newckt.setId(_subcktArray.size());
    _newckt.setNotTraversed();

    if ((Cname.find("sram") != std::string::npos) || (Cname.find("ANALOG") != std::string::npos)) 
    {
        _newckt.setAnalog();
    } else
    {
        _newckt.setNotAnalog();
    }

    //Clear device count
    _newckt.rstTNFET();
    _newckt.rstTPFET();
    _newckt.rstTBJT();
    _newckt.rstTDIO();
    _newckt.rstTRES();
    _newckt.rstTCAP();
    _newckt.rstNFET();
    _newckt.rstPFET();
    _newckt.rstBJT();
    _newckt.rstDIO();
    _newckt.rstRES();
    _newckt.rstCAP();
    _newckt.rstCOMP();

    _inst.clear();
}

/// Call back function to add port
inline void Database::addPortCbk(std::string const &port) 
{
    //For the first subckt - do not remove
    if (_newckt.portList.size() < 50000) 
    {
        _newckt.portList.resize(1000000, false);
    }

    IndexType sPort(0), sNet(0);

    if (_newckt.pinName2IdMap.find(port) != _newckt.pinName2IdMap.end())
    {
        sPort = _newckt.pinName2IdMap[port];
        sNet  = _newckt.pinName2IdMap[port];
    } else 
    {
        SubPin newPin;
        newPin.setName(port);
        newPin.setId(_newckt.numPins());
        newPin.rstConns();
        sPort = newPin.id();
        _newckt.addPin(newPin);
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.pinArray(newPin.id()).setPinType(PinType::PORT);
        _newckt.pinArray(newPin.id()).setPinDir(PinDir::INVALID);

        SubNet newNet;
        newNet.setName(port);
        newNet.setId(_newckt.numNets());
        sNet = newNet.id();
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::PORT);
    }

    _newckt.addPort(sPort);

    //Added to address tie-off
    if ((port == "VDD") || (port == "vdd") || (port == "PWR"))
    {
        _newckt.setPortPwr(_newckt.numPorts()-1, 1);
    } 

    if ((port == "VSS") || (port == "vss") || (port == "GND") || (port == "gnd"))
    {
        _newckt.setPortPwr(_newckt.numPorts()-1, 2);
    } 

   _newckt.portList[sPort] = true;
}

/// Call back function to add feature
inline void Database::addfeatureCbk(std::string const &ftr)
{
    std::vector<std::string> ftype;
    boost::split(ftype, ftr, boost::is_any_of("="), boost::token_compress_on);
    std::string nfeature = ftr;
    if ( (ftype[0] == "l") || (ftype[0] == "L") )
    {
        std::vector<std::string> lval;
        boost::split(lval, ftype[1], boost::is_any_of("e"), boost::token_compress_on);

        if (lval.size() <= 1) //schematic based SPICE netlist
        {
            char metric = lval[0].back();
            lval[0].pop_back();
            if (metric == 'n')
            {
                _inst.setlValue(std::stof(lval[0]));
            } else if (metric == 'u')
            {
                _inst.setlValue(std::stof(lval[0])*1000);
            } else
            {
            _inst.addFeature(nfeature);
            }
        } else
        {
            //Note: Only e-06 to e-09 are dealt here and DEFAULT unit is nanometer 'n'
            if ((lval[1][2] == '6') || (lval[1][1] == '6'))
            {
                _inst.setlValue(std::stof(lval[0])*1000);
            } else if ((lval[1][2] == '7') || (lval[1][1] == '7'))
            {
                _inst.setlValue(std::stof(lval[0])*100);
            } else if ((lval[1][2] == '8') || (lval[1][1] == '8'))
            {
                _inst.setlValue(std::stof(lval[0])*10);
            } else if ((lval[1][2] == '9') || (lval[1][1] == '9'))
            {
                _inst.setlValue(std::stof(lval[0]));
            } else
            {
                _inst.addFeature(nfeature);
            }
        }
    } else if ( (ftype[0] == "w") || (ftype[0] == "W") )
    {
        std::vector<std::string> lval;
        boost::split(lval, ftype[1], boost::is_any_of("e"), boost::token_compress_on);

        if (lval.size() <= 1)
        {
            char metric = lval[0].back();
            lval[0].pop_back();
            if (metric == 'n')
            {
                _inst.setwValue(std::stof(lval[0]));
            } else if (metric == 'u')
            {
                _inst.setwValue(std::stof(lval[0])*1000);
            } else
            {
            _inst.addFeature(nfeature);
            }
        } else
        {
            //Note: Only e-06 to e-09 are dealt here and DEFAULT unit is nanometer 'n'
            if ((lval[1][2] == '6') || (lval[1][1] == '6'))
            {
                _inst.setwValue(std::stof(lval[0])*1000);
            } else if ((lval[1][2] == '7') || (lval[1][1] == '7'))
            {
                _inst.setwValue(std::stof(lval[0])*100);
            } else if ((lval[1][2] == '8') || (lval[1][1] == '8'))
            {
                _inst.setwValue(std::stof(lval[0])*10);
            } else if ((lval[1][2] == '9') || (lval[1][1] == '9'))
            {
                _inst.setwValue(std::stof(lval[0]));
            } else
            {
                _inst.addFeature(nfeature);
            }
        }
    } else if (ftype[0] == "nfin")
    {
        _inst.setfins(std::stof(ftype[1]));
    } else
    {
        _inst.addFeature(nfeature);
    }
}

/// Call back function to add MOS element
inline void Database::addMOSCbk(std::string const &Mname, std::string const &Drain, std::string const &Gate, 
                                std::string const &Source, std::string const &Bulk, std::string const &Mtype)
{
    _inst.setName("M" + toString(_newckt.numInsts()));
    _inst.setCompName(Mtype);

    if ((Mtype.find("nfet") == std::string::npos) && (Mtype.find("NFET") == std::string::npos) && 
        (Mtype.find("nmos") == std::string::npos) && (Mtype.find("NMOS") == std::string::npos) &&
        (Mtype.find("nch") == std::string::npos) && (Mtype.find("NCH") == std::string::npos))
    {
        _inst.setType(InstType::PFET);
    } else
    {
        _inst.setType(InstType::NFET);
    }

    _inst.setId(_newckt.numInsts());

    //Drain
    IndexType dPin(0), dNet(0);
    bool dNew(true);

    if (_newckt.pinName2IdMap.find(Drain) != _newckt.pinName2IdMap.end())
    {
        dPin = _newckt.pinName2IdMap[Drain];
        dNet = _newckt.pinName2IdMap[Drain];
        dNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(Drain);
        newPin.setId(_newckt.numPins());
        _newckt.addPin(newPin);
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);
        dPin = newPin.id();

        SubNet newNet;
        newNet.setName(Drain);
        newNet.setId(_newckt.numNets());
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
        dNet = newNet.id();
    }

    //Source
    IndexType sourcePin(0), sourceNet(0);
    bool sNew(true);

    if (_newckt.pinName2IdMap.find(Source) != _newckt.pinName2IdMap.end())
    {
        sourcePin = _newckt.pinName2IdMap[Source];
        sourceNet = _newckt.pinName2IdMap[Source];
        sNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(Source);
        newPin.setId(_newckt.numPins());
        _newckt.addPin(newPin);
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);
        sourcePin = newPin.id();

        SubNet newNet;
        newNet.setName(Source);
        newNet.setId(_newckt.numNets());
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
        sourceNet = newNet.id();
    }

    //Gate
    IndexType gPin(0), gNet(0);
    bool gNew(true);

    if (_newckt.pinName2IdMap.find(Gate) != _newckt.pinName2IdMap.end())
    {
        gPin = _newckt.pinName2IdMap[Gate];
        gNet = _newckt.pinName2IdMap[Gate];
        gNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(Gate);
        newPin.setId(_newckt.numPins());
        _newckt.addPin(newPin);
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);
        gPin = newPin.id();

        SubNet newNet;
        newNet.setName(Gate);
        newNet.setId(_newckt.numNets());
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
        gNet = newNet.id();
    }

    //Bulk
    IndexType bPin(0), bNet(0);
    bool bNew(true);

    if (_newckt.pinName2IdMap.find(Bulk) != _newckt.pinName2IdMap.end())
    {
        bPin = _newckt.pinName2IdMap[Bulk];
        bNet = _newckt.pinName2IdMap[Bulk];
        bNew = false;
    } else
    {
        SubPin newPin;
        newPin.setName(Bulk);
        newPin.setId(_newckt.numPins());
        _newckt.addPin(newPin);
        _newckt.pinName2IdMap[newPin.name()] = newPin.id();
        _newckt.pinArray(newPin.id()).setPinType(PinType::NET);
        bPin = newPin.id();

        SubNet newNet;
        newNet.setName(Bulk);
        newNet.setId(_newckt.numNets());
        _newckt.addNet(newNet);
        _newckt.netArray(newNet.id()).setNetType(PinType::NET);
        bNet = newNet.id();
    }

    if (Drain == Bulk)
    {
        _inst.addPin(sourcePin);
        _inst.addPin(gPin);
        _inst.addPin(dPin);
        _inst.addPin(bPin);
    } else
    {
        _inst.addPin(dPin);
        _inst.addPin(gPin);
        _inst.addPin(sourcePin);
        _inst.addPin(bPin);
    }

    _inst.setmFactor(1);
    _inst.setsFactor(1);

    bool foundParallel(false);
    bool foundSeries(false);

    if (Parse_DeviceReduction)
    {
        //Device Reduction
        for (IndexType i=0; i < _newckt.numInsts(); ++i)
        {
            if ((_newckt.instArray(i).type() == _inst.type()) && (_newckt.instArray(i).cName() ==  _inst.cName()) && 
                (_newckt.instArray(i).lValue() == _inst.lValue()) && (_newckt.instArray(i).wValue() == _inst.wValue()))
            {
                //Gate and Bulk connections should be the same
                if ((_newckt.instArray(i).pinId(1) == gPin) && (_newckt.instArray(i).pinId(3) == bPin))
                {
                    //Check for Parallel connections
                    if (Parse_ParallelReduction)
                    {
                        if (_newckt.instArray(i).pinId(0) == dPin)
                        {
                            if (_newckt.instArray(i).pinId(2) == sourcePin)
                            {
                                foundParallel = true;
                                _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                            }
                        } else if (_newckt.instArray(i).pinId(0) == sourcePin)
                        {
                            if (_newckt.instArray(i).pinId(2) == dPin)
                            {   
                                foundParallel = true;
                                _newckt.instArray(i).setmFactor(_newckt.instArray(i).mFactor() + 1);
                            }
                        }
                    }
                    //Check for Series connections
                    if (Parse_SeriesReductionFET && (_newckt.instArray(i).mFactor() == 1))
                    {
                        if ( (_newckt.instArray(i).pinId(0) == dPin)  &&  (_newckt.instArray(i).pinId(2) != sourcePin) &&
                             (_newckt.netArray(dNet).numInsts() <= 2) )
                        {
                            foundSeries = true;
                            _newckt.instArray(i).pinId(0) = sourcePin;
                            _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                        } else if ( (_newckt.instArray(i).pinId(2) == dPin)  &&  (_newckt.instArray(i).pinId(0) != sourcePin) &&
                                    (_newckt.netArray(dNet).numInsts() <= 2) )
                        {
                            foundSeries = true;
                            _newckt.instArray(i).pinId(2) = sourcePin;
                            _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                        } else if ( (_newckt.instArray(i).pinId(0) == sourcePin)  &&  (_newckt.instArray(i).pinId(2) != dPin) &&
                                (_newckt.netArray(sourceNet).numInsts() <= 2) )
                        {
                            foundSeries = true;
                            _newckt.instArray(i).pinId(0) = dPin;
                            _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                        } else if ( (_newckt.instArray(i).pinId(2) == sourcePin)  &&  (_newckt.instArray(i).pinId(0) != dPin) &&
                                (_newckt.netArray(sourceNet).numInsts() <= 2) )
                        {
                            foundSeries = true;
                            _newckt.instArray(i).pinId(2) = dPin;
                            _newckt.instArray(i).setsFactor(_newckt.instArray(i).sFactor() + 1);
                        }
                    }
                }
            }
        }
    } 

    if ((!foundParallel && !foundSeries) || (!Parse_DeviceReduction))
    {
        _newckt.netArray(dNet).addInst(_inst.id());
        _newckt.netArray(sourceNet).addInst(_inst.id());
        _newckt.netArray(gNet).addInst(_inst.id());
        _newckt.netArray(bNet).addInst(_inst.id());

        //Connectivity
        //Check if analog based on connection: Identify S based on connection with B and identify (i) D - S - B tied off (ii) G-D tied off
        if (Drain == Bulk)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[Drain]).setPinDir(PinDir::INOUT);
            _newckt.pinArray(_newckt.pinName2IdMap[Source]).setPinDir(PinDir::OUTPUT);
        } else if (Source == Bulk)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[Source]).setPinDir(PinDir::INOUT);
            _newckt.pinArray(_newckt.pinName2IdMap[Drain]).setPinDir(PinDir::OUTPUT);
        } else
        {
            _newckt.pinArray(_newckt.pinName2IdMap[Source]).setPinDir(PinDir::OUTPUT);
            _newckt.pinArray(_newckt.pinName2IdMap[Drain]).setPinDir(PinDir::OUTPUT);
        }

        if (_newckt.pinArray(_newckt.pinName2IdMap[Gate]).dir() != PinDir::OUTPUT)
        {
            _newckt.pinArray(_newckt.pinName2IdMap[Gate]).setPinDir(PinDir::INPUT);
        }

        _newckt.pinArray(_newckt.pinName2IdMap[Bulk]).setPinDir(PinDir::INOUT);

        if ((Drain == Source) || (Gate == Drain) || (Gate == Source))
        {
            _inst.setAnalog();
            _newckt.addAnalogInst(_inst.id());
        } else
        {
            _inst.setNotAnalog();
        }

        //Update wire
        if (_newckt.numPorts() > 0)
        {
            //if Pin is not port - add as wire
            if (_newckt.portList[dPin] == false)
            {
                _newckt.addWire(dPin);
            } 
            if (_newckt.portList[gPin] == false)
            {
                _newckt.addWire(gPin);
            } 
            if (_newckt.portList[sourcePin] == false)
            {
                _newckt.addWire(sourcePin);
            }
            if (_newckt.portList[bPin] == false)
            {
                _newckt.addWire(bPin);
            } 
        } else
        {
            _newckt.addWire(dPin);
            _newckt.addWire(sourcePin);
            _newckt.addWire(gPin);
            _newckt.addWire(bPin);
        }

        _newckt.addInst(_inst);

        if (_inst.type() == InstType::NFET)
        {
            _newckt.incrNFET();
        } else
        {
            _newckt.incrPFET();
        }
    } else
    {
        //Delete redundant new entries of pin/net created
        if (bNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (gNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (sNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
        if (dNew)
        {
            _newckt.pinArray().pop_back();
            _newckt.netArray().pop_back();
        }
    }
    _inst.clear();
}

PROJECT_NAMESPACE_END

#endif // __DATABASE_H__
