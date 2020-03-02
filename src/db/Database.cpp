//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
//Initial version obtained from Wuxi Li (UTDA) and modified by Rachel Selina Rajarathnam (UTDA)
#include <fstream>
#include <stack>
#include <list>
#include "global/global.h"
#include "global/type.h"
#include "db/Database.h"
#include "parser/BookshelfDriver.h"
#include "util/Boost_graph_wrapper.h"

//To dump additional files
static constexpr bool       OneDotOut = true; //Dump out dotfile => TRUE: Single concatenated file; FALSE: Multiple dot files created

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Variables to be set for logic reduction techniques 
static constexpr bool       DeviceReduction      = true; 
//Options for Device Reduction
static constexpr bool       ParallelReductionRES = true;
static constexpr bool       ParallelReductionCAP = false;
static constexpr bool       ParallelReductionDIO = true;
static constexpr bool       ParallelReductionBJT = true;
static constexpr bool       ParallelReductionFET = true;
//Settings for Series Reduction
static constexpr bool       SeriesReductionRES   = false;
static constexpr bool       SeriesReductionCAP   = false;
static constexpr bool       SeriesReductionFET   = false;

////Divide and Conquer Settings for Overall Runtime reduction
static constexpr uint32_t   MinInstanceCount     = 8000; //Employ divide and conquer if total instance count within subckt after flattening exceeds this value
static constexpr uint16_t   BatchValue           = 750; //Value of individual segment size
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

PROJECT_NAMESPACE_BEGIN

/// Parse input files that in bookself format
void Database::parse(std::string const &auxPath)
{
    _fileInfo.setAuxFile(auxPath);
    BookshelfParser::Driver driver(*this);
    std::ifstream ifs;

    // Parse bookshelf AUX file
    INF("Parsing file %s", auxPath.c_str());
    ifs.open(auxPath);
    if (! ifs.good())
    {
        ERR("Cannot open file %s", auxPath.c_str());
    }
    driver.parse_stream(ifs);
    INF("Completed parsing file %s", auxPath.c_str());
    ifs.close();
}

/// Identify Leaf subckts and write out csv file
void Database::analyzeNetlist()
{ 
    /// Dump out analysis for subckts with devices (leaf elements) in CSV format
    std::ofstream fout;

    _leafSubckts.clear();
    _nonLeaves.clear();

    std::unordered_map<IndexType, IndexType> ckt2PrtMap;
    std::vector<Byte> traverseSubckts(_subcktArray.size(), 0); //To identify leaf subckts that have port update due to tie-off

    for (IndexType i = 0; i < _subcktArray.size(); ++i)
    {
        //All Subckts have instances - subckts without instances are ignored during parsing!
        Subckt &newckt = _subcktArray[i];

        if ((newckt.nfet() + newckt.pfet() + newckt.bjt() + newckt.dio() + newckt.res() + newckt.cap()) > 0) 
        {
            newckt.setLeaf();
            _leafSubckts.emplace_back(i); //Identify leaf subckts in design
        } else 
        {
            newckt.setNotLeaf();
            _nonLeaves.emplace_back(i); //Non leaf subckts
        }

        //Handle tie-offs here
        if (newckt.comp() > 0)
        {
            //Visit mixed and hierarchical subckts
            IndexType tieLoIdx(0), tieHiIdx(0);
            
            for (IndexType ist = 0; ist < newckt.numInsts(); ++ist)
            {
                Inst &nInst = newckt.instArray(ist);

                if ((nInst.type() == InstType::COMPONENT) && (traverseSubckts[_subcktToIdMap[nInst.cName()]] == 1))
                {
                    //Remove additional pin
                    nInst.pinArray().pop_back();
                }

                if ((nInst.type() == InstType::NFET) || (nInst.type() == InstType::PFET))
                {
                    //FETs with gate tied off - assume only one tie-off exists
                    IndexType nBulk, pBulk;

                    if (nInst.type() == InstType::NFET)
                    {
                        nBulk = nInst.pinId(3);
                    } 
                    if (nInst.type() == InstType::PFET)
                    {
                        pBulk = nInst.pinId(3);
                    }
                    //Check if Gate = Bulk
                    if ((nInst.pinId(1) == nBulk) || (nInst.pinId(1) == pBulk))
                    {
                        IndexType tiePinId;
                        std::string tiePin;

                        if (nInst.pinId(1) == nBulk)
                        {
                            tiePinId = nBulk;
                            tiePin = "tieLo";
                        }
                        else
                        {
                            tiePinId = pBulk;
                            tiePin = "tieHi";
                        }

                        SubPin newPin;
                        newPin.setName(tiePin);
                        newPin.setId(newckt.numPins());
                        newPin.setPinDir(PinDir::INOUT);
                        newckt.pinName2IdMap[newPin.name()] = newPin.id();
                        newckt.addPin(newPin);
                        newckt.pinArray(newPin.id()).setPinType(PinType::NET);

                        SubNet newNet;
                        newNet.setName(tiePin);
                        newNet.setId(newckt.numNets());
                        newckt.addNet(newNet);
                        newckt.netArray(newNet.id()).setNetType(PinType::NET);

                        nInst.pinId(1) = newPin.id();
                    }
                } else if (nInst.type() == InstType::COMPONENT)
                {
                    Subckt &nckt = _subcktArray[_subcktToIdMap[nInst.cName()]];
                    IndexType vddPortId, vssPortId;

                    for (IndexType pn = 0; pn < nckt.numPorts(); ++pn)
                    {
                        if (nckt.getPortPwr(pn) == 1)
                        {
                            vddPortId = pn;
                            if (newckt.isPort(nInst.pinId(pn)))
                            {
                               newckt.setPortType(nInst.pinId(pn), 1); 
                            }
                        }
                        if (nckt.getPortPwr(pn) == 2)
                        {
                            vssPortId = pn;
                            if (newckt.isPort(nInst.pinId(pn)))
                            {
                               newckt.setPortType(nInst.pinId(pn), 2); 
                            }
                        }
                    }

                    for (IndexType ipn = 0; ipn < nInst.numPins(); ++ipn)
                    {
                        Byte tieCK(0);
                        std::string tiePinName;
                        if ((ipn != vddPortId) && (nInst.pinId(ipn) == nInst.pinId(vddPortId)))
                        {
                            tieCK = 1;
                            tiePinName = "tieHi" + toString(tieHiIdx);
                        }
                        if ((ipn != vssPortId) && (nInst.pinId(ipn) == nInst.pinId(vssPortId)))
                        {
                            tieCK = 2;
                            tiePinName = "tieLo" + toString(tieLoIdx);
                        }

                        if (tieCK > 0)
                        {
                            SubPin newPin;
                            newPin.setName(tiePinName);
                            newPin.setId(newckt.numPins());
                            newPin.setPinDir(PinDir::INOUT);
                            newckt.pinName2IdMap[newPin.name()] = newPin.id();
                            newckt.addPin(newPin);
                            newckt.pinArray(newPin.id()).setPinType(PinType::NET);
                            newckt.pinArray(newPin.id()).setTieOff();

                            SubNet newNet;
                            newNet.setName(tiePinName);
                            newNet.setId(newckt.numNets());
                            newckt.addNet(newNet);
                            newckt.netArray(newNet.id()).setNetType(PinType::NET);

                            nInst.pinId(ipn) = newPin.id();

                            if (tieCK == 2)
                            {
                                ++tieLoIdx;
                            } else
                            {
                                ++tieHiIdx;
                            }
                            //std::cout << "Created new Pin: " << newPin.name() << " for Inst: " << nInst.cName() << " within " << newckt.name() << std::endl;
                        }
                    }
                }
            }
        } else
        {
            //Visit logic gate defintions of leaf subckts
            for (IndexType pn = 0; pn < newckt.numPorts(); ++pn)
            {
                if (newckt.getPortPwr(pn) > 0)
                {
                    newckt.pinArray(newckt.port(pn)).setPinDir(PinDir::INOUT);
                }
            }

            SubPin &lport = newckt.pinArray(newckt.port(newckt.numPorts()-1));

            if ((newckt.getPortPwr(newckt.numPorts()-1) == 0) && (lport.dir() == PinDir::INOUT))
            {
                //Remove additional port in subckt
                ckt2PrtMap[newckt.id()] = newckt.numPorts()-1; //new no of ports
                IndexType prtId (lport.id());

                //Remove additional port
                newckt.ports().pop_back();
                traverseSubckts[newckt.id()] = 1;

                //Replace the removed pin reference everywhere to avoid dangles
                IndexType pwrPin(prtId); //Value to swap the extra pin
                std::vector<std::pair<IndexType, IndexType>> instPinIdx; //get inst id and corresponding pin id for replacement

                for (IndexType ist = 0; ist < newckt.numInsts(); ++ist)
                {
                    Inst &nInst = newckt.instArray(ist);
                    if ((nInst.type() == InstType::NFET) || (nInst.type() == InstType::PFET))
                    {
                        //Ignore GATE and check other connections
                        IndexType dPin(nInst.pinId(0)), sPin(nInst.pinId(2)), bPin(nInst.pinId(3));
                        Byte fPwr(3), fPrt(1), fpl(1);

                        if (dPin == prtId)
                        {
                            fPrt = 0; 
                        } else if (newckt.isPort(dPin))
                        {
                            fPwr = newckt.getPortType(dPin); 
                            if (fPwr > 0)
                            {
                                fpl = 0;
                            }
                        }
                        if (sPin == prtId)
                        {
                            fPrt = 2; 
                        } else if (newckt.isPort(sPin))
                        {
                            fPwr = newckt.getPortType(sPin); 
                            if(fPwr > 0)
                            {
                                fpl = 2;
                            }
                        }
                        if (bPin == prtId)
                        {
                            fPrt = 3; 
                        } else if (newckt.isPort(bPin))
                        {
                            fPwr = newckt.getPortType(bPin); 
                            if(fPwr > 0)
                            {
                                fpl = 3;
                            }
                        }

                        if (fPrt != 1)
                        {
                            instPinIdx.emplace_back(std::make_pair(ist, fPrt));

                            if ((fpl != 1) && (pwrPin == prtId))
                            {
                                pwrPin = newckt.instArray(ist).pinId(fpl);
                            }
                        }
                    }
                }

                if (pwrPin != prtId)
                {
                    for (const std::pair<IndexType, IndexType> &elem : instPinIdx)
                    {
                        newckt.instArray(elem.first).pinId(elem.second) = pwrPin;
                    }
                }
            }
        }
    }
    INF("Analysis of SPICE netlist completed");
}

//Add gate to netlist
void addGate(IndexType const gIndex, Subckt &newckt, std::vector<IndexType> const &Insts, IndexType const gCount, Module &nl)
{
    std::string gType, cellName;
    GateType gate;
    std::vector<IndexType> cPins;
    std::vector<std::string> cCons;
    Cell newcell;
    bool inverter(false), logicGate(true);

    //Get input/output/IO trueports
    switch(gIndex) 
    {
        case 0: //Component/Device type
            {  
                logicGate = false;

                switch(newckt.instArray(Insts[0]).type())
                {
                    case InstType::COMPONENT: 
                        gate = GateType::COMPONENT;
                        cellName = "X" + toString(gCount);
                        break;
                    case InstType::NFET:
                    case InstType::PFET:
                        gate = GateType::NOT_GATE;
                        cellName = "M" + toString(gCount);
                        break;
                    case InstType::DIODE:
                        gate = GateType::NOT_GATE;
                        cellName = "D" + toString(gCount);
                        break;
                    case InstType::RES:
                        gate = GateType::NOT_GATE;
                        cellName = "R" + toString(gCount);
                        break;
                    case InstType::CAP:
                        gate = GateType::NOT_GATE;
                        cellName = "C" + toString(gCount);
                        break;
                    case InstType::NPN:
                    case InstType::PNP:
                        gate = GateType::NOT_GATE;
                        cellName = "Q" + toString(gCount);
                        break;
                }

                if (newckt.instArray(Insts[0]).numfeatures() > 0)
                {
                    newcell.setFeatures(newckt.instArray(Insts[0]).featureArray());
                }

                gType = newckt.instArray(Insts[0]).cName();

                newcell.setType(gate);
                newcell.setInstId(Insts[0]);
                newcell.setName(cellName);
                newcell.setGName(gType);
                newcell.setId(nl.numCells());
                if (gate == GateType::NOT_GATE)
                {
                    newcell.setlValue(newckt.instArray(Insts[0]).lValue());
                    newcell.setwValue(newckt.instArray(Insts[0]).wValue());
                    newcell.setfins(newckt.instArray(Insts[0]).fins());
                    newcell.setmFactor(newckt.instArray(Insts[0]).mFactor());
                    newcell.setsFactor(newckt.instArray(Insts[0]).sFactor());
                }

                if (newckt.instArray(Insts[0]).numPins() > 0)
                {
                    for (IndexType i=0; i < newckt.instArray(Insts[0]).numPins(); ++i)
                    {
                        cPins.emplace_back(newckt.instArray(Insts[0]).pinId(i));
                        cCons.emplace_back("d_pin");
                    }
                }
                break;
            }
#include "library/lib_gateType.cpp"
    }

    for (IndexType i=0; i < cPins.size(); ++i)
    {
        if (logicGate)
        {
            newcell.setPname();
        } else
        {
            newcell.setNotPname();
        }
        IndexType newPin(0);
        if (nl.pinName2IdMap.find(newckt.pinArray(cPins[i]).name()) != nl.pinName2IdMap.end())
        {
            newPin = nl.pinName2IdMap[newckt.pinArray(cPins[i]).name()];
        } else 
        {
            Pin nPin;
            nPin.setId(nl.numPins());
            nPin.setName(newckt.pinArray(cPins[i]).name());
            nPin.setPinType(newckt.pinArray(cPins[i]).type());
            nPin.setPinDir(newckt.pinArray(cPins[i]).dir());
            if (newckt.pinArray(cPins[i]).isTieOff())
            {
                nPin.setTieOff();
            }
            if (newckt.pinArray(cPins[i]).isPower())
            {
                nPin.setPower();
            } else
            {
                nPin.setNotPower();
            }
            nl.pinName2IdMap[nPin.name()] = nPin.id();
            nl.addPin(nPin);
            newPin = nPin.id();
        }
        if ((nl.portList[newPin] == false) && !nl.pinArray(newPin).isTieOff())
        {
            nl.addWire(newPin);
        }
        newcell.addPin(newPin);
        newcell.addpName(cCons[i]);
    }

    //Free up memory
    cPins.clear();
    cCons.clear();

    bool ck(false);
    for (IndexType i=0; i < Insts.size(); ++i)
    {
        if (newckt.instArray(Insts[i]).isAnalog())
        {
            ck = true;
            break;
        }
    }

    if (ck == true)
    {
        newcell.setAnalog();
    } else
    {
        newcell.setNotAnalog();
    }

    if (inverter == true)
    {
        newcell.setInverter();
        nl.addInv(newcell.id());
    } else
    {
        newcell.setNotInverter();
    }

    nl.addCell(newcell);
}

/// Port Propagation
void Database::portPropagation()
{
    IndexType startIndex = _subcktArray.size()-1;

    //Using BFS
    std::list<IndexType> portPropagation;       
    portPropagation.push_back(startIndex);

    //portmapping
    std::vector<std::string> portMap;
    std::vector<bool> powerMap;
    IndexType pIndex(0);

    _gPort.addVertex(startIndex, portMap, powerMap); //root will have empty portMap
    _gPortMap[startIndex].emplace_back(_gPort.vertexSize()-1);

    //Check for power ports in Top module
    for (IndexType i=0; i < _subcktArray[startIndex].numPorts(); ++i)
    {
        std::string tpin(_subcktArray[startIndex].pinArray(_subcktArray[startIndex].port(i)).name());
        if ((tpin == "VDD") || (tpin == "AVDD") || (tpin == "DVDD") || (tpin == "VSS") || (tpin == "AVSS") || (tpin == "GND"))
        {
            _subcktArray[startIndex].pinArray(_subcktArray[startIndex].port(i)).setPower();
            _subcktArray[startIndex].pinArray(_subcktArray[startIndex].port(i)).setPinDir(PinDir::INOUT);
        } else
        {
            _subcktArray[startIndex].pinArray(_subcktArray[startIndex].port(i)).setNotPower();
        }
    }

    while (!portPropagation.empty())
    {
        IndexType val = portPropagation.front();
        std::unordered_map<std::string, std::string> p2pMap; //port map from parent to child

        if ((pIndex > 0) && (_subcktArray[val].comp() > 0))
        {
            std::vector<std::string> pMap(_gPort.getGraph()[pIndex].portArray()); //port Map from parent

            for (IndexType i=0; i < _subcktArray[val].numPorts(); ++i)
            {
                p2pMap[_subcktArray[val].pinArray(_subcktArray[val].port(i)).name()] = pMap[i];
            }
        }

        for (IndexType i=0; i < _subcktArray[val].numInsts(); ++i)
        {
            if (_subcktArray[val].instArray(i).type() == InstType::COMPONENT)
            {
                portMap.clear();
                powerMap.clear();
                
                //get PortMap
                for (IndexType p=0; p < _subcktArray[val].instArray(i).numPins(); ++p)
                {
                    if ((pIndex > 0) && (p2pMap.find(_subcktArray[val].pinArray(_subcktArray[val].instArray(i).pinId(p)).name()) != p2pMap.end()))
                    {
                        portMap.emplace_back(p2pMap[_subcktArray[val].pinArray(_subcktArray[val].instArray(i).pinId(p)).name()]);
                        
                        if ((portMap[portMap.size()-1] == "VDD") || (portMap[portMap.size()-1] == "AVDD") || (portMap[portMap.size()-1] == "DVDD") || 
                            (portMap[portMap.size()-1] == "VSS") || (portMap[portMap.size()-1] == "AVSS") || (portMap[portMap.size()-1] == "GND"))
                        {
                            powerMap.emplace_back(true);
                            _subcktArray[val].pinArray(_subcktArray[val].pinName2IdMap[portMap[portMap.size()-1]]).setPower();
                            _subcktArray[val].pinArray(_subcktArray[val].pinName2IdMap[portMap[portMap.size()-1]]).setPinDir(PinDir::INOUT);
                        } else
                        {
                            _subcktArray[val].pinArray(_subcktArray[val].pinName2IdMap[portMap[portMap.size()-1]]).setNotPower();
                            powerMap.emplace_back(false);
                        }
                    } else
                    {
                        portMap.emplace_back(_subcktArray[val].pinArray(_subcktArray[val].instArray(i).pinId(p)).name());
                        
                        if ((portMap[portMap.size()-1] == "VDD") || (portMap[portMap.size()-1] == "AVDD") || (portMap[portMap.size()-1] == "DVDD") || 
                            (portMap[portMap.size()-1] == "VSS") || (portMap[portMap.size()-1] == "AVSS") || (portMap[portMap.size()-1] == "GND"))
                        {
                            powerMap.emplace_back(true);
                            _subcktArray[val].pinArray(_subcktArray[val].pinName2IdMap[portMap[portMap.size()-1]]).setPower();
                            _subcktArray[val].pinArray(_subcktArray[val].pinName2IdMap[portMap[portMap.size()-1]]).setPinDir(PinDir::INOUT);
                        } else
                        {
                            _subcktArray[val].pinArray(_subcktArray[val].pinName2IdMap[portMap[portMap.size()-1]]).setNotPower();
                            powerMap.emplace_back(false);
                        }
                    }
                }
                //get subckt ID
                IndexType sId = _subcktToIdMap[_subcktArray[val].instArray(i).cName()];
                IndexType cId = _gPort.vertexSize();
                _gPort.addVertex(sId, portMap, powerMap);
                _gPortMap[sId].emplace_back(_gPort.vertexSize()-1);
                _gPort.addEdge(pIndex, cId);

                portPropagation.push_back(sId);
            }
        }
        portPropagation.pop_front();
        ++pIndex; //increment with each element removed
    }
    INF("Port propagation completed");
}

void Database::deepFlatten(Subckt &instCell, Subckt &temp, Subckt &newckt, Module &nl, IndexType const prfx, IndexType const i, IndexType &tempInst, IndexType plevel,
                           IndexType level, std::unordered_map<std::string, std::string> &cell2InstPortMap, std::unordered_map<std::string, std::string> &l0PortMap)
{
    //Temporary map file for multiple level hierarchy
    std::unordered_map<std::string, std::string> tempPortMap, interimPortMap;

    //Update interim portmap - to avoid data corruption
    for (auto& tmap : cell2InstPortMap)
    {
        interimPortMap[tmap.first] = tmap.second;
    }

    if (level == 0) //at newckt level
    {
        for (IndexType pt=0; pt < instCell.numPorts(); ++pt) 
        { 
            cell2InstPortMap[instCell.pinArray(instCell.port(pt)).name()] = newckt.pinArray(newckt.instArray(i).pinId(pt)).name();
            l0PortMap[instCell.pinArray(instCell.port(pt)).name()] = newckt.pinArray(newckt.instArray(i).pinId(pt)).name();
        }
    } else
    { //depth>1
        if ((plevel == 0) && (level > 1))
        {
            cell2InstPortMap.clear();
            tempPortMap.clear();
            cell2InstPortMap = l0PortMap;

            for (IndexType pt=0; pt < instCell.numPorts(); ++pt) 
            { 
                if (cell2InstPortMap.find(newckt.pinArray(newckt.instArray(i).pinId(pt)).name()) != cell2InstPortMap.end())
                {
                    if (_gPortMap[instCell.id()].size() != 1)
                    {
                        cell2InstPortMap[instCell.pinArray(instCell.port(pt)).name()] = l0PortMap[newckt.pinArray(newckt.instArray(i).pinId(pt)).name()];
                    } else
                    {
                        if (instCell.pinArray(instCell.port(pt)).name() != cell2InstPortMap[newckt.pinArray(newckt.instArray(i).pinId(pt)).name()])
                        {
                            tempPortMap[instCell.pinArray(instCell.port(pt)).name()] = l0PortMap[newckt.pinArray(newckt.instArray(i).pinId(pt)).name()];
                        }
                    }
                }
            }
        } else
        {
            for (IndexType pt=0; pt < instCell.numPorts(); ++pt) 
            { 

                if (cell2InstPortMap.find(newckt.pinArray(newckt.instArray(i).pinId(pt)).name()) != cell2InstPortMap.end())
                {
                    if (_gPortMap[instCell.id()].size() != 1)
                    {
                        cell2InstPortMap[instCell.pinArray(instCell.port(pt)).name()] = interimPortMap[newckt.pinArray(newckt.instArray(i).pinId(pt)).name()];
                    } else
                    {
                        if (instCell.pinArray(instCell.port(pt)).name() != cell2InstPortMap[newckt.pinArray(newckt.instArray(i).pinId(pt)).name()])
                        {
                            tempPortMap[instCell.pinArray(instCell.port(pt)).name()] = interimPortMap[newckt.pinArray(newckt.instArray(i).pinId(pt)).name()];
                        }
                    }
                }
            }
        }
    }

    //Set previous level to avoid port map issues during multiple hierarchy
    plevel = level;

    //Update contents of cell2InstPortMap based on tempPortMap for multiple level hierarchy
    if (tempPortMap.size() > 0)
    {
        for (auto& tmap : tempPortMap)
        {
            cell2InstPortMap[tmap.first] = tmap.second;
        }
    }

    for (IndexType cinst=0; cinst < instCell.numInsts(); ++cinst)
    {
        if (instCell.instArray(cinst).type() == InstType::COMPONENT) 
        {
            std::string kname(_subcktArray[_subcktToIdMap[instCell.instArray(cinst).cName()]].name());
            if ( kname != instCell.instArray(cinst).cName())
            {
                //There are some esdnsh cells that have 4 terminals like MOS but are represented as components
                if ((instCell.instArray(cinst).numPins() == 4)  || (instCell.instArray(cinst).numPins() == 2))
                {
                    Inst newInst(instCell.instArray(cinst));
                    newInst.setId(temp.numInsts());
                    if (instCell.instArray(cinst).numPins() == 4)
                    {
                        newInst.setName("M" + toString(tempInst));
                        if (instCell.instArray(cinst).cName().find("pfet") == std::string::npos) 
                        {
                            newInst.setType(InstType::NFET);
                        } else
                        {
                            newInst.setType(InstType::PFET);
                        }
                    } else
                    {
                        newInst.setName("D" + toString(tempInst));
                        newInst.setType(InstType::DIODE);
                    }
                    if (instCell.instArray(cinst).isAnalog())
                    {
                        temp.addAnalogInst(newInst.id());
                    } 
                    newInst.clrPins();
                    //add pin information
                    for (IndexType cp=0; cp < instCell.instArray(cinst).numPins(); ++cp)
                    {
                        IndexType newPin(0), newNet(0);
                        std::string pinName(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name());

                        if (cell2InstPortMap.find(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()) != cell2InstPortMap.end())
                        {
                            if(instCell.pinName2IdMap.find(cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]) == instCell.pinName2IdMap.end())
                            {
                                SubPin nPin;
                                nPin.setName(cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]);
                                nPin.setId(temp.numPins());
                                nPin.setNotPower();
                                temp.pinName2IdMap[nPin.name()] = nPin.id();
                                newPin = nPin.id();
                                temp.addPin(nPin);

                                SubNet nNet;
                                nNet.setName(cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]);
                                nNet.setId(temp.numNets());
                                newNet = nNet.id();
                                temp.addNet(nNet);

                                if ((instCell.numInsts() >= 2) && (instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir() != PinDir::INVALID))
                                {
                                    temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                                } else
                                {
                                    switch(cp)
                                    {
                                        case 0:
                                            {
                                                if (instCell.instArray(cinst).numPins() == 2)
                                                {
                                                    temp.pinArray(newPin).setPinDir(PinDir::INPUT);
                                                } else
                                                {
                                                    if (instCell.instArray(cinst).pinId(0) == instCell.instArray(cinst).pinId(3))
                                                    {
                                                        temp.pinArray(newPin).setPinDir(PinDir::INOUT);
                                                    } else
                                                    {
                                                        temp.pinArray(newPin).setPinDir(PinDir::OUTPUT);
                                                    }
                                                }
                                            }
                                        case 1:
                                            {
                                                if (instCell.instArray(cinst).numPins() == 2)
                                                {
                                                    temp.pinArray(newPin).setPinDir(PinDir::OUTPUT);
                                                } else
                                                {
                                                    temp.pinArray(newPin).setPinDir(PinDir::INPUT);
                                                }
                                            }
                                        case 2:
                                            {
                                                if (instCell.instArray(cinst).numPins() == 4)
                                                {
                                                    if (instCell.instArray(cinst).pinId(2) == instCell.instArray(cinst).pinId(3))
                                                    {
                                                        temp.pinArray(newPin).setPinDir(PinDir::INOUT);
                                                    } else
                                                    {
                                                        temp.pinArray(newPin).setPinDir(PinDir::OUTPUT);
                                                    }
                                                } 
                                            }
                                        case 3:
                                            {
                                                if (instCell.instArray(cinst).numPins() == 4)
                                                {
                                                    temp.pinArray(newPin).setPinDir(PinDir::INOUT);
                                                }
                                            }
                                    }
                                }

                                temp.pinArray(newPin).setPinType(PinType::NET);
                                temp.netArray(newNet).setNetType(PinType::NET);
                                temp.netArray(newNet).addInst(newInst.id());

                                if (temp.portList[newPin] == false)
                                {
                                    if (level == 0) temp.addWire(newPin);
                                }

                            } else
                            {
                                newPin = temp.pinName2IdMap[cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]];
                                newNet = temp.pinName2IdMap[cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]];
                                temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                                temp.netArray(newNet).addInst(newInst.id());
                            }
                        } else
                        {
                            if (temp.pinName2IdMap.find("n" + pinName + toString(i) + toString(prfx)) == temp.pinName2IdMap.end()) 
                            {
                                SubPin nPin;
                                nPin.setName("n" + pinName + toString(i) + toString(prfx));
                                nPin.setId(temp.numPins());
                                nPin.setNotPower();
                                newPin = nPin.id();
                                temp.pinName2IdMap[nPin.name()] = nPin.id();
                                temp.addPin(nPin);

                                SubNet nNet;
                                nNet.setName("n" + pinName + toString(i) + toString(prfx));
                                nNet.setId(temp.numNets());
                                newNet = nNet.id();
                                temp.addNet(nNet);

                                temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                                temp.pinArray(newPin).setPinType(PinType::NET);

                                temp.netArray(newNet).setNetType(PinType::NET);
                                temp.netArray(newNet).addInst(newInst.id());

                                if (temp.portList[newPin] == false)
                                {
                                    if(level == 0) temp.addWire(newPin);
                                }
                            } else
                            {
                                newPin = temp.pinName2IdMap["n" + pinName + toString(i) + toString(prfx)];
                                newNet = temp.pinName2IdMap["n" + pinName + toString(i) + toString(prfx)];
                                temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                                temp.netArray(newNet).addInst(newInst.id());
                            }
                        }
                        newInst.addPin(newPin);
                    }
                    newInst.setmFactor(1);

                    if ( (newInst.type() == InstType::NFET) || (newInst.type() == InstType::PFET) || 
                            (newInst.type() == InstType::RES) || (newInst.type() == InstType::CAP) )
                    {
                        newInst.setsFactor(1);
                    } else
                    {
                        newInst.setsFactor(0);
                    }
                    temp.addInst(newInst);
                    ++tempInst;
                } else 
                {
                    ERR("Not found subckt definition of %s", temp.instArray(cinst).cName().c_str());
                }
            } else
            {
                ++level;
                deepFlatten(_subcktArray[_subcktToIdMap[instCell.instArray(cinst).cName()]], temp, instCell, nl, i, cinst, tempInst, plevel, level, cell2InstPortMap, l0PortMap);
            }
        } else 
        {
            Inst newInst(instCell.instArray(cinst));
            newInst.setName(instCell.instArray(cinst).name()[0] + toString(tempInst));
            newInst.setId(temp.numInsts());

            if (instCell.isMapped())
            {
                newInst.setType(InstType::INVALID);
            }
            newInst.clrPins();

            for (IndexType cp=0; cp < instCell.instArray(cinst).numPins(); ++cp)
            {
                IndexType newPin(0), newNet(0);
                std::string pinName = instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name();

                if (cell2InstPortMap.find(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()) != cell2InstPortMap.end())
                {
                    if(temp.pinName2IdMap.find(cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]) == temp.pinName2IdMap.end())
                    {
                        SubPin nPin;
                        nPin.setName(cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]);
                        nPin.setId(temp.numPins());
                        nPin.setNotPower();
                        newPin = nPin.id();
                        temp.pinName2IdMap[nPin.name()] = nPin.id();
                        temp.addPin(nPin);

                        SubNet nNet;
                        nNet.setName(cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]);
                        nNet.setId(temp.numNets());
                        newNet = nNet.id();
                        temp.addNet(nNet);

                        temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                        temp.pinArray(newPin).setPinType(PinType::NET);

                        temp.netArray(newNet).setNetType(PinType::NET);
                        temp.netArray(newNet).addInst(newInst.id());

                        if (temp.portList[newPin] == false)
                        {
                            if (level == 0) temp.addWire(newPin);
                        }

                    } else
                    {
                        newPin = temp.pinName2IdMap[cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]];
                        newNet = temp.pinName2IdMap[cell2InstPortMap[instCell.pinArray(instCell.instArray(cinst).pinId(cp)).name()]];
                        temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                        temp.netArray(newNet).addInst(newInst.id());
                    }
                } else
                {
                    if (temp.pinName2IdMap.find("n" + pinName + toString(i) + toString(prfx)) == temp.pinName2IdMap.end()) 
                    {
                        SubPin nPin;
                        nPin.setName("n" + pinName + toString(i) + toString(prfx));
                        nPin.setId(temp.numPins());
                        nPin.setNotPower();
                        newPin = nPin.id();
                        temp.pinName2IdMap[nPin.name()] = nPin.id();
                        temp.addPin(nPin);

                        SubNet nNet;
                        nNet.setName("n" + pinName + toString(i) + toString(prfx));
                        nNet.setId(temp.numNets());
                        newNet = nNet.id();
                        temp.addNet(nNet);

                        temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                        temp.pinArray(newPin).setPinType(PinType::NET);

                        temp.netArray(newNet).setNetType(PinType::NET);
                        temp.netArray(newNet).addInst(newInst.id());

                        if (temp.portList[newPin] == false)
                        {
                            if(level == 0) temp.addWire(newPin);
                        }
                    } else
                    {
                        newPin = temp.pinName2IdMap["n" + pinName + toString(i) + toString(prfx)];
                        newNet = temp.pinName2IdMap["n" + pinName + toString(i) + toString(prfx)];
                        temp.pinArray(newPin).setPinDir(instCell.pinArray(instCell.instArray(cinst).pinId(cp)).dir());
                        temp.netArray(newNet).addInst(newInst.id());
                    }
                }
                newInst.addPin(newPin);
            }

            if (DeviceReduction)
            {
                bool foundParallel(false);
                bool foundSeries(false);
                //Device Reduction
                for (IndexType p=0; p < temp.numInsts(); ++p)
                {
                    if ( (temp.instArray(p).type() == newInst.type()) && (temp.instArray(p).cName() == newInst.cName()) ) //Group only similar devices
                    {
                        switch(temp.instArray(p).type())
                        {
                            case InstType::RES: 
                                {
                                    //Check for Parallel Connections
                                    if (ParallelReductionRES)
                                    {
                                        if ( (temp.instArray(p).pinId(0) == newInst.pinId(0))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(1)) )
                                        {
                                            foundParallel = true;
                                            temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);
                                        } else if ( (temp.instArray(p).pinId(0) == newInst.pinId(1))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(0)) )
                                        {
                                            foundParallel = true;
                                            temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);
                                        }
                                    }
                                    //Check for Series Connections
                                    if (SeriesReductionRES && (temp.instArray(p).mFactor() == 1))
                                    {
                                        if ( (temp.instArray(p).pinId(0) == newInst.pinId(1))  &&  (temp.instArray(p).pinId(1) != newInst.pinId(0)) &&
                                                (temp.netArray(temp.instArray(p).pinId(0)).numInsts() <= 2) )
                                        {
                                            foundSeries = true;
                                            temp.instArray(p).pinId(0) = newInst.pinId(0);
                                            temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                        } else if ( (temp.instArray(p).pinId(0) != newInst.pinId(1))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(0)) &&
                                                (temp.netArray(temp.instArray(p).pinId(1)).numInsts() <= 2) )
                                        {
                                            foundSeries = true;
                                            temp.instArray(p).pinId(1) = newInst.pinId(1);
                                            temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                        } else if ( (temp.instArray(p).pinId(0) == newInst.pinId(0))  &&  (temp.instArray(p).pinId(1) != newInst.pinId(1)) && 
                                                (temp.netArray(temp.instArray(p).pinId(0)).numInsts() <= 2) )
                                        {
                                            foundSeries = true;
                                            temp.instArray(p).pinId(0) = newInst.pinId(1);
                                            temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                        } else if ( (temp.instArray(p).pinId(0) != newInst.pinId(0))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(1)) &&
                                                (temp.netArray(temp.instArray(p).pinId(1)).numInsts() <= 2) )
                                        {
                                            foundSeries = true;
                                            temp.instArray(p).pinId(1) = newInst.pinId(0);
                                            temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                        }
                                    }
                                }
                            case InstType::CAP: 
                                {   
                                    //Check for Parallel Connections
                                    if (ParallelReductionCAP)
                                    {
                                        if ( (temp.instArray(p).pinId(0) == newInst.pinId(0))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(1)) )
                                        {
                                            foundParallel = true;
                                            temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);
                                        } else if ( (temp.instArray(p).pinId(0) == newInst.pinId(1))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(0)) )
                                        {
                                            foundParallel = true;
                                            temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);
                                        }
                                    }
                                    //Check for Series Connections
                                    if (SeriesReductionCAP && (temp.instArray(p).mFactor() == 1))
                                    {
                                        if ( (temp.instArray(p).pinId(0) == newInst.pinId(1))  &&  (temp.instArray(p).pinId(1) != newInst.pinId(0)) &&
                                                (temp.netArray(temp.instArray(p).pinId(0)).numInsts() <= 2) )
                                        {
                                            foundSeries = true;
                                            temp.instArray(p).pinId(0) = newInst.pinId(0);
                                            temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                        } else if ( (temp.instArray(p).pinId(0) != newInst.pinId(1))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(0)) &&
                                                (temp.netArray(temp.instArray(p).pinId(1)).numInsts() <= 2) )
                                        {
                                            foundSeries = true;
                                            temp.instArray(p).pinId(1) = newInst.pinId(1);
                                            temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                        }
                                    }
                                    break;
                                }                        
                            case InstType::DIODE: 
                                {
                                    //Check for Parallel Connection
                                    if(ParallelReductionDIO)
                                    {
                                        if ( (temp.instArray(p).pinId(0) == newInst.pinId(0))  &&  (temp.instArray(p).pinId(1) == newInst.pinId(1)) )
                                        {
                                            foundParallel = true;
                                            temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);
                                        } 
                                    }
                                    break;
                                }
                            case InstType::PNP: 
                            case InstType::NPN: 
                                {
                                    //Check for Parallel Connection
                                    if(ParallelReductionBJT)
                                    {
                                        if ( (temp.instArray(p).pinId(0) == newInst.pinId(0)) && (temp.instArray(p).pinId(1) == newInst.pinId(1)) && 
                                                (temp.instArray(p).pinId(2) == newInst.pinId(2)) )
                                        {
                                            foundParallel = true;
                                            temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);
                                        }
                                    }
                                    break;
                                }
                            case InstType::NFET: 
                            case InstType::PFET: 
                                {
                                    if ((temp.instArray(p).pinId(1) == newInst.pinId(1)) && (temp.instArray(p).pinId(3) == newInst.pinId(3)) && 
                                            (temp.instArray(p).lValue() == newInst.lValue()) && (temp.instArray(p).wValue() == newInst.wValue()))
                                    {
                                        //Check for Parallel Connections
                                        if (ParallelReductionFET)
                                        {
                                            if ( (temp.instArray(p).pinId(0) == newInst.pinId(0)) && (temp.instArray(p).pinId(2) == newInst.pinId(2)) )
                                            {
                                                foundParallel = true;
                                                temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);

                                            } else if ( (temp.instArray(p).pinId(0) == newInst.pinId(2)) && (temp.instArray(p).pinId(2) == newInst.pinId(0)) )
                                            {
                                                foundParallel = true;
                                                temp.instArray(p).setmFactor(temp.instArray(p).mFactor() + 1);
                                            }
                                        }
                                        //Check for Series Connections
                                        if (SeriesReductionFET && (temp.instArray(p).mFactor() == 1))
                                        {
                                            if ( (temp.instArray(p).pinId(0) == newInst.pinId(0))  &&  (temp.instArray(p).pinId(2) != newInst.pinId(2)) &&
                                                    (temp.netArray(temp.instArray(p).pinId(0)).numInsts() <= 2) )
                                            {
                                                foundSeries = true;
                                                temp.instArray(p).pinId(0) = newInst.pinId(2);
                                                temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                            } else if ( (temp.instArray(p).pinId(2) == newInst.pinId(0))  &&  (temp.instArray(p).pinId(0) != newInst.pinId(2)) &&
                                                    (temp.netArray(temp.instArray(p).pinId(2)).numInsts() <= 2) )
                                            {
                                                foundSeries = true;
                                                temp.instArray(p).pinId(2) = newInst.pinId(2);
                                                temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                            } else if ( (temp.instArray(p).pinId(0) == newInst.pinId(2))  &&  (temp.instArray(p).pinId(2) != newInst.pinId(0)) &&
                                                    (temp.netArray(temp.instArray(p).pinId(0)).numInsts() <= 2) )
                                            {
                                                foundSeries = true;
                                                temp.instArray(p).pinId(0) = newInst.pinId(0);
                                                temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                            } else if ( (temp.instArray(p).pinId(2) == newInst.pinId(2))  &&  (temp.instArray(p).pinId(0) != newInst.pinId(0)) &&
                                                    (temp.netArray(temp.instArray(p).pinId(2)).numInsts() <= 2) )
                                            {
                                                foundSeries = true;
                                                temp.instArray(p).pinId(2) = newInst.pinId(0);
                                                temp.instArray(p).setsFactor(temp.instArray(p).sFactor() + 1);
                                            }
                                        }
                                    }
                                    break;
                                }
                        }
                    }
                }
                if (!foundParallel && !foundSeries)
                {
                    if (instCell.instArray(cinst).isAnalog())
                    {
                        temp.addAnalogInst(newInst.id());
                    } 
                    temp.addInst(newInst);
                    ++tempInst;
                } 
            } else
            {
                if (instCell.instArray(cinst).isAnalog())
                {
                    temp.addAnalogInst(newInst.id());
                } 
                temp.addInst(newInst);
                ++tempInst;
            }
        }
    }

    //Creating a new variable to avoid repetitive net names within cells
    IndexType fixRep(0), cVal(prfx);
    if (prfx == cVal)
    {
        fixRep = nl.numCells();
    }

    if (instCell.isMapped())
    {
        Module &nTemp(_netlistArray[instCell.id()]);
        for (IndexType mld=0; mld < nTemp.numCells(); ++mld)
        {
            if (nTemp.cellArray(mld).type() != GateType::INVALID)
            {
                Cell newCell(nTemp.cellArray(mld));
                newCell.setId(nl.numCells());
                newCell.setName(nTemp.cellArray(mld).cName()[0] + toString(nl.numCells()));
                newCell.clrPins();
                newCell.setNotAnalog();

                if (nTemp.cellArray(mld).isInverter())
                {
                    nl.addInv(newCell.id());
                }

                for (IndexType mpn=0; mpn < nTemp.cellArray(mld).numPins(); ++mpn)
                {
                    IndexType newPin(0);
                    std::string pName = nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).name();

                    if (!nTemp.isPort(nTemp.cellArray(mld).pinId(mpn))) 
                    {
                        pName = pName + toString(i) + toString(prfx) + toString(fixRep);
                    }

                    if (_gPortMap[instCell.id()].size() == 1)
                    {
                        if (nl.pinName2IdMap.find(pName) == nl.pinName2IdMap.end()) 
                        {
                            Pin nPin;
                            nPin.setName(pName);
                            nPin.setId(nl.numPins());
                            nPin.setNotPower();
                            newPin = nPin.id();
                            nl.pinName2IdMap[nPin.name()] = nPin.id();
                            nl.addPin(nPin);

                            nl.pinArray(newPin).setPinDir(nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir());
                            nl.pinArray(newPin).setPinType(PinType::NET);

                            if (nl.portList[newPin] == false)
                            {
                                nl.addWire(newPin);
                            }
                        } else
                        {
                            newPin = nl.pinName2IdMap[pName];

                            if ((nl.pinArray(newPin).dir() == PinDir::OUTPUT) || (nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir() == PinDir::OUTPUT) ||
                                    (newCell.numPins() == 0))
                            {
                                nl.pinArray(newPin).setPinDir(PinDir::OUTPUT);
                            } else
                            {
                                nl.pinArray(newPin).setPinDir(nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir());
                            }
                        }
                    } else
                    {
                        if (cell2InstPortMap.find(pName) != cell2InstPortMap.end())
                        {
                            if(nl.pinName2IdMap.find(cell2InstPortMap[pName]) == nl.pinName2IdMap.end())
                            {
                                Pin nPin;
                                nPin.setName(cell2InstPortMap[pName]);
                                nPin.setId(nl.numPins());
                                nPin.setNotPower();
                                newPin = nPin.id();
                                nl.pinName2IdMap[nPin.name()] = nPin.id();
                                nl.addPin(nPin);

                                nl.pinArray(newPin).setPinDir(nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir());
                                nl.pinArray(newPin).setPinType(PinType::NET);

                                if (nl.portList[newPin] == false)
                                {
                                    nl.addWire(newPin);
                                }

                            } else
                            {
                                newPin = nl.pinName2IdMap[cell2InstPortMap[pName]];
                                nl.pinArray(newPin).setPinDir(nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir());
                            }
                        } else
                        {
                            if (nl.pinName2IdMap.find("n" + pName + toString(i) + toString(prfx) + toString(fixRep)) == nl.pinName2IdMap.end()) 
                            {
                                Pin nPin;
                                nPin.setName("n" + pName + toString(i) + toString(prfx) + toString(fixRep));
                                nPin.setId(nl.numPins());
                                nPin.setNotPower();
                                newPin = nPin.id();
                                nl.pinName2IdMap[nPin.name()] = nPin.id();
                                nl.addPin(nPin);

                                nl.pinArray(newPin).setPinDir(nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir());
                                nl.pinArray(newPin).setPinType(PinType::NET);

                                if (nl.portList[newPin] == false)
                                {
                                    nl.addWire(newPin);
                                }
                            } else
                            {
                                newPin = nl.pinName2IdMap["n" + pName + toString(i) + toString(prfx) + toString(fixRep)];

                                if ((nl.pinArray(newPin).dir() == PinDir::OUTPUT) || (nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir() == PinDir::OUTPUT) ||
                                        (newCell.numPins() == 0))
                                {
                                    nl.pinArray(newPin).setPinDir(PinDir::OUTPUT);
                                } else
                                {
                                    nl.pinArray(newPin).setPinDir(nTemp.pinArray(nTemp.cellArray(mld).pinId(mpn)).dir());
                                }
                            }
                        }
                    }
                    newCell.addPin(newPin);
                }
                nl.addCell(newCell);
            }
        }
    }
}

//flatten a leaf level subckt - with devices and components and update subckt array information
void Database::flattenSubckt(Subckt &newckt, Module &nl, IndexType &cellIndex)
{
    Subckt temp;

    temp.setId(newckt.id());
    temp.setName(newckt.name());
    if (newckt.isLeaf())
    {
        temp.setLeaf();
    } else
    {
        temp.setNotLeaf();
    }
    if (newckt.isAnalog())
    {
        temp.setAnalog();
        temp.setTraversed();
    } else
    {
        temp.setNotAnalog();
        temp.setNotTraversed();
    }

    //Update number of instances
    temp.setTNFET(newckt.tnfet());
    temp.setTPFET(newckt.tpfet());
    temp.setTBJT(newckt.tbjt());
    temp.setTDIO(newckt.tdio());
    temp.setTRES(newckt.tres());
    temp.setTCAP(newckt.tcap());

    temp.setNFET(newckt.nfet());
    temp.setPFET(newckt.pfet());
    temp.setBJT(newckt.bjt());
    temp.setDIO(newckt.dio());
    temp.setRES(newckt.res());
    temp.setCAP(newckt.cap());
    temp.setCOMP(newckt.comp());

    temp.portList.resize(1000000, false);

    //Add port pins from newckt to temp
    for (IndexType pt=0; pt < newckt.numPorts(); ++pt)
    {
        if (temp.pinName2IdMap.find(newckt.pinArray(newckt.port(pt)).name()) == temp.pinName2IdMap.end())
        {
            SubPin newPin;
            newPin.setId(temp.numPins());
            newPin.setPinType(PinType::PORT);
            newPin.setPinDir(newckt.pinArray(newckt.port(pt)).dir());
            newPin.setName(newckt.pinArray(newckt.port(pt)).name());
            newPin.setConns(newckt.pinArray(newckt.port(pt)).conns());
            if (newckt.pinArray(newckt.port(pt)).isPower()) 
            {
                newPin.setPower();
            } else
            {
                newPin.setNotPower();
            }
            temp.pinName2IdMap[newPin.name()] = newPin.id();
            temp.addPin(newPin);

            SubNet newNet(newckt.netArray(newckt.port(pt)));    
            newNet.setId(temp.numNets());
            newNet.clrInsts();
            temp.addNet(newNet);

            temp.addPort(newPin.id());
            temp.portList[newPin.id()] = true;
        } else
        {
            temp.addPort(temp.pinName2IdMap[newckt.pinArray(newckt.port(pt)).name()]);
            temp.portList[temp.pinName2IdMap[newckt.pinArray(newckt.port(pt)).name()]] = true;
        }
    }
    //Update insts
    IndexType tempInst(0);

    for(IndexType i=0; i < newckt.numInsts(); ++i)
    {
        if (newckt.instArray(i).type() == InstType::COMPONENT) 
        {
            std::string kname(_subcktArray[_subcktToIdMap[newckt.instArray(i).cName()]].name());
            if ( kname != newckt.instArray(i).cName() )
            {
                //There are some esdnsh cells that have 4 terminals like MOS but are represented as components
                if ((newckt.instArray(i).numPins() == 4) || (newckt.instArray(i).numPins() == 2))
                {
                    Inst newInst(newckt.instArray(i));
                    newInst.setId(temp.numInsts());
                    if (newckt.instArray(i).numPins() ==4)
                    {
                        newInst.setName("M" + toString(tempInst));
                        if (newckt.instArray(i).cName().find("pfet") == std::string::npos) 
                        {
                            newInst.setType(InstType::NFET);
                        } else
                        {
                            newInst.setType(InstType::PFET);
                        }
                    } else
                    {
                        newInst.setName("D" + toString(tempInst));
                        newInst.setType(InstType::DIODE);
                    }

                    if (newckt.instArray(i).isAnalog())
                    {
                        temp.addAnalogInst(newInst.id());
                    } 

                    newInst.clrPins();

                    //add pin information
                    for (IndexType cp=0; cp < newckt.instArray(i).numPins(); ++cp)
                    {
                        IndexType newPin(0), newNet(0);
                        PinDir pDir(newckt.pinArray(newckt.instArray(i).pinId(cp)).dir());
                        std::string pinName(newckt.pinArray(newckt.instArray(i).pinId(cp)).name());

                        if(temp.pinName2IdMap.find(pinName) == temp.pinName2IdMap.end())
                        {
                            SubPin nPin;
                            nPin.setName(pinName);
                            nPin.setId(temp.numPins());
                            nPin.setNotPower();
                            newPin = nPin.id();
                            temp.addPin(nPin);
                            temp.pinName2IdMap[nPin.name()] = nPin.id();

                            SubNet nNet;
                            nNet.setName(pinName);
                            nNet.setId(temp.numNets());
                            newNet = nNet.id();
                            temp.addNet(nNet);

                            temp.pinArray(newPin).setPinDir(pDir);
                            temp.pinArray(newPin).setPinType(PinType::NET);

                            temp.netArray(newNet).setNetType(PinType::NET);
                            temp.netArray(newNet).addInst(newInst.id());
                        } else
                        {
                            newPin = temp.pinName2IdMap[pinName];
                            newNet = temp.pinName2IdMap[pinName];
                            temp.pinArray(newPin).setPinDir(pDir);
                            temp.netArray(newNet).addInst(newInst.id());
                        }
                        if (temp.portList[newPin] == false)
                        {
                            temp.addWire(newPin);
                        }
                        newInst.addPin(newPin);
                    }

                    newInst.setmFactor(1);
                    if ( (newInst.type() == InstType::NFET) || (newInst.type() == InstType::PFET) || 
                         (newInst.type() == InstType::RES) || (newInst.type() == InstType::CAP) )
                    {
                        newInst.setsFactor(1);
                    } else
                    {
                        newInst.setsFactor(0);
                    }
                    temp.addInst(newInst);
                    ++tempInst;
                } else
                {
                    INF("Not found subckt definition for cell %s", newckt.instArray(i).cName().c_str());
                }
            } else 
            {   
                std::unordered_map<std::string, std::string> cell2InstPortMap, l0PortMap;
                deepFlatten(_subcktArray[_subcktToIdMap[newckt.instArray(i).cName()]], temp, newckt, nl, i, i, tempInst, 0, 0, cell2InstPortMap, l0PortMap);
            }
        }
        else 
        {
            Inst newInst(newckt.instArray(i));
            newInst.setId(temp.numInsts());
            newInst.setName(newckt.instArray(i).name()[0] + toString(tempInst));
            newInst.clrPins();

            //add pin information
            for (IndexType cp=0; cp < newckt.instArray(i).numPins(); ++cp)
            {
                IndexType newPin(0), newNet(0);
                PinDir pDir(newckt.pinArray(newckt.instArray(i).pinId(cp)).dir());
                std::string pinName(newckt.pinArray(newckt.instArray(i).pinId(cp)).name());

                if(temp.pinName2IdMap.find(pinName) == temp.pinName2IdMap.end())
                {
                    SubPin nPin;
                    nPin.setName(pinName);
                    nPin.setId(temp.numPins());
                    nPin.setNotPower();
                    newPin = nPin.id();
                    temp.addPin(nPin);
                    temp.pinName2IdMap[pinName] = nPin.id();

                    SubNet nNet;
                    nNet.setName(pinName);
                    nNet.setId(temp.numNets());
                    newNet = nNet.id();
                    temp.addNet(nNet);

                    temp.pinArray(newPin).setPinDir(pDir);
                    temp.pinArray(newPin).setPinType(PinType::NET);

                    temp.netArray(newNet).setNetType(PinType::NET);
                    temp.netArray(newNet).addInst(newInst.id());
                } else
                {
                    newPin = temp.pinName2IdMap[pinName];
                    newNet = temp.pinName2IdMap[pinName];
                    temp.pinArray(newPin).setPinDir(pDir);
                    temp.netArray(newNet).addInst(newInst.id());
                }
                if (temp.portList[newPin] == false)
                {
                    temp.addWire(newPin);
                }
                newInst.addPin(newPin);
            }

            if (newckt.instArray(i).isAnalog())
            {
                temp.addAnalogInst(newInst.id());
            } 
            temp.addInst(newInst);
            ++tempInst;
        }
    }
    _subcktArray[cellIndex] = temp;
    newckt.clear();
    newckt = temp;
}

//add Cells based on mapping
void updateCell(Subckt &newckt, Module &nl, std::vector<IndexType> const &gateMap, std::vector<IndexType> const &cktMap, 
                                IndexType &gCount, IndexType const gType, IndexType const gSize)
{
    for(IndexType i=0; i < gateMap.size(); i+=gSize)
    {
        std::vector<IndexType> insts;
        for (IndexType j=0; j < gSize; ++j)
        {
            insts.emplace_back(cktMap[gateMap[i+j]]);
        }
        ++gCount;
        addGate(gType, newckt, insts, gCount, nl); 
    }
}

void updateCktmap (std::vector<bool>& gateSet, std::vector<IndexType>& cktMap)
{
    for (IndexType i=0; i < gateSet.size() ; ++i) 
    {
        if (gateSet[i] == false)
        {   
            cktMap.emplace_back(i);
        }
    }
}

//Without staggering based on decreasing order of no of vertices
void findLogicGateNS (GraphB &graph, Subckt &newckt, std::vector<IndexType> &cktMap, std::vector<bool> &gateSet, Module &nl, IndexType &gCount)
{
    std::vector<IndexType> gateMap;
    IndexType vCount(cktMap.size()); //Variable to check no of vertices available for comparison

    GraphB::graphType g1 = graph.getGraph();

#include "library/lib_findGate.cpp"

}

//Flatten leaf subckts
void Database::leafFlatten()
{
    GraphP::graphp nGraph = _gPort.getGraph();
    //Deal with leaf subckts
    for (IndexType i=0; i < _leafSubckts.size(); ++i)
    {
        Subckt &newckt = _subcktArray[_leafSubckts[i]];
        Module &nl = _netlistArray[_leafSubckts[i]];
        nl.portList.resize(1000000, false);

        if (newckt.isAnalog() == false)
        {
           //Port update for leaf subckts to aid logic gate extraction
            if ((_gPortMap[_leafSubckts[i]].size() > 0) && (_gPortMap.find(_leafSubckts[i]) != _gPortMap.end()) && (_leafSubckts[i] < _subcktArray.size()-1))
            {
                if (_gPortMap[_leafSubckts[i]].size() == 1)
                {
                    //Update all ports for unique instances
                    for (IndexType np=0; np < newckt.numPorts(); ++np)
                    {
                        //Check if net of same name already exists
                        if (newckt.pinName2IdMap.find(nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)) != newckt.pinName2IdMap.end())
                        {
                            newckt.pinArray(newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)]).setName("net"+nGraph[_gPortMap[_leafSubckts[i]][0]].port(np));
                            newckt.pinName2IdMap["net"+nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)] = newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)];
                        }
                        newckt.pinArray(newckt.port(np)).setName(nGraph[_gPortMap[_leafSubckts[i]][0]].port(np));
                        newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)] = newckt.port(np);
                    }
                } else
                {
                    //Update power for multiple instantiations
                    for (IndexType np=0; np < newckt.numPorts(); ++np)
                    {
                        if (nGraph[_gPortMap[_leafSubckts[i]][0]].isPower(np))
                        {
                            newckt.pinArray(newckt.port(np)).setName(nGraph[_gPortMap[_leafSubckts[i]][0]].port(np));
                            newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)] = newckt.port(np);
                        }
                    }
                }
            }

            //update ports of Module nl based on Subckt newckt
            for (IndexType pt=0; pt < newckt.numPorts(); ++pt)
            {
                if (nl.pinName2IdMap.find(newckt.pinArray(newckt.port(pt)).name()) == nl.pinName2IdMap.end())
                {
                    IndexType newPin(newckt.port(pt));
                    Pin nPin(newckt.pinArray(newPin).name(), PinType::PORT, newckt.pinArray(newPin).dir(), newckt.pinArray(newPin).isPower());
                    nPin.setId(nl.numPins());
                    nl.addPin(nPin);
                    nl.pinName2IdMap[nPin.name()] = nPin.id();

                    nl.addPort(nPin.id());
                    nl.portList[nPin.id()] = true;
                } else
                {
                    nl.addPort(nl.pinName2IdMap[newckt.pinArray(newckt.port(pt)).name()]);
                    nl.portList[nl.pinName2IdMap[newckt.pinArray(newckt.port(pt)).name()]] = true;
                }
            }
            Database::flattenSubckt(newckt, nl, _leafSubckts[i]); //flatten if there are components 'X' present in list of instances
        } else
        {
            INF("%s is considered as ANALOG", newckt.name().c_str());
        }
    }
}

//Hierarchical logic gate extraction 
void Database::hierExtraction()
{
    GraphP::graphp nGraph = _gPort.getGraph();
    //Deal with leaf subckts
    for (IndexType i=0; i < _leafSubckts.size(); ++i)
    {
        Subckt &newckt = _subcktArray[_leafSubckts[i]];
        Module &nl = _netlistArray[_leafSubckts[i]];
        nl.portList.resize(1000000, false);

        if (newckt.isAnalog() == false)
        {
            //Port update for leaf subckts to aid logic gate extraction
            if ((_gPortMap[_leafSubckts[i]].size() > 0) && (_gPortMap.find(_leafSubckts[i]) != _gPortMap.end()) && (_leafSubckts[i] < _subcktArray.size()-1))
            {
                if (_gPortMap[_leafSubckts[i]].size() == 1)
                {
                    //Update all ports for unique instances
                    for (IndexType np=0; np < newckt.numPorts(); ++np)
                    {
                        //Check if net already exists with same name - avoid duplication and data corruption
                        if (newckt.pinName2IdMap.find(nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)) != newckt.pinName2IdMap.end() && 
                            !newckt.isPort(newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)]))
                        {
                            newckt.pinArray(newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)]).setName("net"+nGraph[_gPortMap[_leafSubckts[i]][0]].port(np));
                            newckt.pinName2IdMap["net"+nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)] = newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)];
                        }
                        newckt.pinArray(newckt.port(np)).setName(nGraph[_gPortMap[_leafSubckts[i]][0]].port(np));
                        newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)] = newckt.port(np);
                    }
                } else
                {
                    //Update power for multiple instantiations
                    for (IndexType np=0; np < newckt.numPorts(); ++np)
                    {
                        if (nGraph[_gPortMap[_leafSubckts[i]][0]].isPower(np))
                        {
                            newckt.pinArray(newckt.port(np)).setName(nGraph[_gPortMap[_leafSubckts[i]][0]].port(np));
                            newckt.pinName2IdMap[nGraph[_gPortMap[_leafSubckts[i]][0]].port(np)] = newckt.port(np);
                        }
                    }
                }
            }

            //update ports of Module nl based on Subckt newckt
            for (IndexType pt=0; pt < newckt.numPorts(); ++pt)
            {
                if (nl.pinName2IdMap.find(newckt.pinArray(newckt.port(pt)).name()) == nl.pinName2IdMap.end())
                {
                    IndexType newPin(newckt.port(pt));
                    Pin nPin(newckt.pinArray(newPin).name(), PinType::PORT, newckt.pinArray(newPin).dir(), newckt.pinArray(newPin).isPower());
                    
                    nPin.setId(nl.numPins());
                    nl.addPin(nPin);
                    nl.pinName2IdMap[nPin.name()] = nPin.id();

                    nl.addPort(nPin.id());
                    nl.portList[nPin.id()] = true;
                } else
                {
                    nl.addPort(nl.pinName2IdMap[newckt.pinArray(newckt.port(pt)).name()]);
                    nl.portList[nl.pinName2IdMap[newckt.pinArray(newckt.port(pt)).name()]] = true;
                }
            }

            Database::flattenSubckt(newckt, nl, _leafSubckts[i]); //flatten if there are components 'X' present in list of instances

            //Completed flattening of leaf subckt - Begin logic gate extraction
            IndexType gCount(0), val(_leafSubckts[i]);

            if (nl.numCells() > 0)
            {
                gCount = nl.numCells()-1;
            }

            //Logic gate extraction 
            if((newckt.tnfet() > 0) && (newckt.tpfet() > 0)) //Skip logic gate extraction if PFET or NFET count = 0
            {
                std::vector<IndexType> cktMap, rInsts;
                std::vector<bool> nSet(newckt.numInsts(), false);

                for (IndexType ai=0; ai < newckt.numAnalogInsts(); ++ai) 
                {
                    if ((newckt.instArray(newckt.analogInst(ai)).type() != InstType::INVALID) && !nSet[newckt.analogInst(ai)])
                    {
                        nSet[newckt.analogInst(ai)] = true;
                        rInsts.emplace_back(newckt.analogInst(ai));
                    }
                }

                for (IndexType ni=0; ni < newckt.numInsts(); ++ni) 
                {
                    if (newckt.instArray(ni).type() == InstType::INVALID)
                    {
                        nSet[ni] = true;
                    } else 
                    {
                        if (((newckt.instArray(ni).type() != InstType::NFET) && (newckt.instArray(ni).type() != InstType::PFET)) && !nSet[ni])
                        {
                            nSet[ni] = true;
                            rInsts.emplace_back(ni);
                        }
                    }
                }

                updateCktmap(nSet, cktMap); 

                //Deal based on number on instances: Runtime increases exponentially for graph with > 2000 vertices 
                if (cktMap.size() <= MinInstanceCount) 
                {
                    GraphB graph(newckt, cktMap);

                    findLogicGateNS (graph, newckt, cktMap, nSet, nl, gCount);
                } else
                {
                    IndexType loopIdx(0), batch(BatchValue), prevCount(0), nextCount(1);
                    bool loopCondition(false);

                    if (cktMap.size() > MinInstanceCount)
                    {
                        loopCondition = true;
                    } 

                    while (loopCondition)
                    {
                        ++loopIdx;
                        std::cout << "Loop for divide and conquer " << loopIdx << std::endl;
                        //Divide p/n type devices present 
                        std::vector<IndexType> pMap, nMap;

                        for (IndexType cm=0; cm < cktMap.size(); ++cm)
                        {
                            if (newckt.instArray(cktMap[cm]).type() == InstType::NFET)
                            {
                                nMap.emplace_back(cktMap[cm]);

                            } else if (newckt.instArray(cktMap[cm]).type() == InstType::PFET)
                            {
                                pMap.emplace_back(cktMap[cm]);
                            }
                        }

                        //Now perform Divide and Conquer using equal n-type and p-type devices
                        IndexType nstart(0), pstart(0), nend(0), pend(0), pCount(pMap.size()/batch), nCount(nMap.size()/batch);
                        for (IndexType nc=0; nc <= nCount; ++nc)
                        {
                            pstart = nc*batch;
                            nstart = nc*batch;
                            nend = nstart+batch;
                            pend = pstart+batch;

                            if (nstart >= nMap.size())
                            {
                                nstart = nMap.size()-1;
                            } 
                            if (pstart >= pMap.size())
                            {
                                pstart = pMap.size()-1;
                            }
                            if (nend >= nMap.size())
                            {
                                nend = nMap.size()-1;
                            } 
                            if (pend >= pMap.size())
                            {
                                pend = pMap.size()-1;
                            }

                            std::vector<IndexType> cMap;
                            IndexType nelem(0), pelem(0);

                            for (IndexType np = nstart; np < nend; ++np)
                            {
                                cMap.emplace_back(nMap[np]);
                                ++nelem;
                            }
                            for (IndexType np = pstart; np < pend; ++np)
                            {
                                cMap.emplace_back(pMap[np]);
                                ++pelem;
                            }

                            if ((nelem > 0) && (pelem > 0))
                            {
                                GraphB graph(newckt, cMap);
                                findLogicGateNS (graph, newckt, cMap, nSet, nl, gCount);
                                INF("Logic gate identification using Divide & Conquer");
                            }
                        }                
                        cktMap.clear();
                        updateCktmap(nSet, cktMap); 

                        if (nextCount == 1)
                        {
                            nextCount = cktMap.size();
                        } else
                        {
                            prevCount = nextCount;
                            nextCount = cktMap.size();
                        }
                        if (prevCount < nextCount)
                        {
                            loopCondition = true;
                        } else
                        {
                            loopCondition = false;
                        }

                    }
                    cktMap.clear();
                    updateCktmap(nSet, cktMap); 

                    std::cout << "Last iteration with instance count: " << cktMap.size() << std::endl;

                    GraphB graph(newckt, cktMap);

                    findLogicGateNS (graph, newckt, cktMap, nSet, nl, gCount);
                } 

                IndexType ckMapped(0);

                for (IndexType ns=0; ns < nSet.size(); ++ns)
                {
                    if (nSet[ns] == false)
                    {
                        rInsts.emplace_back(ns);
                        ++ckMapped;
                    }
                }

                //Uniqify contents of rInsts
                std::sort( rInsts.begin(), rInsts.end() );
                rInsts.erase( std::unique( rInsts.begin(), rInsts.end() ), rInsts.end() );

                std::vector<IndexType> insts;
                //Update Analog and remaining components
                for (IndexType j=0; j < rInsts.size(); ++j)
                {
                    insts.clear();
                    insts.emplace_back(rInsts[j]);
                    ++gCount;
                    addGate(0, newckt, insts, gCount, nl); 
                }

                //Update if mapped
                if (ckMapped > 0)
                {
                    newckt.setNotMapped();
                } else
                {
                    newckt.setMapped();
                }

                //Free up memory
                nSet.resize(1, false);

            } else
            {
                std::vector<IndexType> insts;
                for(IndexType j=0; j < newckt.numInsts(); ++j)
                {
                    insts.emplace_back(j);
                    ++gCount;
                    addGate(0, newckt, insts, gCount, nl); 
                    insts.clear();
                }
                newckt.setNotMapped();
            }
        } else
        {
            INF("%s is considered as ANALOG", newckt.name().c_str());
        }
    }
    INF("Completed hierarchical logic gate extraction");
}

void Database::portDirUpdate()
{
    for (IndexType i=0; i < _nonLeaves.size(); ++i)
    {
        Subckt &newckt = _subcktArray[_nonLeaves[i]];

        std::vector<char> pinList(newckt.numPins(), 'x');

        for (IndexType idx=0; idx < newckt.numInsts(); ++idx)
        {
            if ((newckt.instArray(idx).type() != InstType::INVALID) && (newckt.instArray(idx).type() == InstType::COMPONENT)) 
            {
                for (IndexType pt=0; pt < _subcktArray[_subcktToIdMap[newckt.instArray(idx).cName()]].numPorts(); ++pt) //Get lower hierarchy port dir info and update
                {
                    PinDir updDir(PinDir::INVALID);

                    if (_netlistArray[_subcktToIdMap[newckt.instArray(idx).cName()]].numCells() > 0)
                    {
                        updDir = _netlistArray[_subcktToIdMap[newckt.instArray(idx).cName()]].pinArray(_netlistArray[_subcktToIdMap[newckt.instArray(idx).cName()]].port(pt)).dir();
                        _subcktArray[_subcktToIdMap[newckt.instArray(idx).cName()]].pinArray(_subcktArray[_subcktToIdMap[newckt.instArray(idx).cName()]].port(pt)).setPinDir(updDir);
                    } else
                    {
                        updDir = _subcktArray[_subcktToIdMap[newckt.instArray(idx).cName()]].pinArray(_subcktArray[_subcktToIdMap[newckt.instArray(idx).cName()]].port(pt)).dir();
                    }

                    if (pinList[newckt.instArray(idx).pinId(pt)] == 'x')
                    {
                        newckt.pinArray(newckt.instArray(idx).pinId(pt)).setPinDir(updDir);

                        switch(newckt.pinArray(newckt.instArray(idx).pinId(pt)).dir())
                        {
                            case PinDir::INPUT: pinList[newckt.instArray(idx).pinId(pt)] = 'i';
                                                break;
                            case PinDir::OUTPUT: pinList[newckt.instArray(idx).pinId(pt)] = 'o';
                                                break;
                            case PinDir::INOUT: pinList[newckt.instArray(idx).pinId(pt)] = 'n';
                                                break;
                        }
                    } else if (pinList[newckt.instArray(idx).pinId(pt)] != 'o')
                    {
                        newckt.pinArray(newckt.instArray(idx).pinId(pt)).setPinDir(updDir);

                        switch(newckt.pinArray(newckt.instArray(idx).pinId(pt)).dir())
                        {
                            case PinDir::INPUT: pinList[newckt.instArray(idx).pinId(pt)] = 'i';
                                                break;
                            case PinDir::OUTPUT: pinList[newckt.instArray(idx).pinId(pt)] = 'o';
                                                 break;
                            case PinDir::INOUT: pinList[newckt.instArray(idx).pinId(pt)] = 'n';
                                                break;
                        }
                    }
                }
            }
        }
        pinList.clear(); //free up memory space
    }
}

void subcktHeader(std::ofstream &fout, Subckt &newckt)
{
    std::vector<IndexType> wireList(newckt.wires());
    fout << "module " << newckt.name() << " ( "; 

    //Check wires to find VDD/VSS before writing out port info
    std::sort( wireList.begin(), wireList.end() );
    wireList.erase( std::unique( wireList.begin(), wireList.end() ), wireList.end() );

    //Find and delete
    std::vector<IndexType> wlTies;
    Byte fVdd(0), fVss(0);

    for (auto welem : wireList)
    {
        if(newckt.pinArray(welem).name().find("tie") != std::string::npos)
        {
            wlTies.emplace_back(welem);

            //Update tie-off name directly
            if (newckt.pinArray(welem).name().find("tieHi") != std::string::npos)
            {
                newckt.pinArray(welem).setName("VDD");
            } else
            {
                newckt.pinArray(welem).setName("VSS");
            }
        }

        if(newckt.pinArray(welem).name().find("VSS") != std::string::npos)
        {
            fVss = 1;
            wlTies.emplace_back(welem);
        }

        if(newckt.pinArray(welem).name().find("VDD") != std::string::npos)
        {
            fVdd = 1;
            wlTies.emplace_back(welem);
        }
    }

    for(auto wlt : wlTies) 
    {
        wireList.erase(std::remove(wireList.begin(), wireList.end(), wlt), wireList.end());
    } 

    if (newckt.numPorts() > 0)
    {
        for (IndexType i=0; i < newckt.numPorts()-1; ++i) 
        {
            fout << newckt.pinArray(newckt.port(i)).name() << ", ";
        }
        fout << newckt.pinArray(newckt.port(newckt.numPorts()-1)).name();

        if ((fVss > 0) || (fVdd > 0))
        {
            if ((fVss > 0) && (fVdd > 0))
            {
                fout << ", VSS, VDD ); \n" << std::endl;
            } else if (fVss > 0)
            {
                fout << ", VSS ); \n" << std::endl;
            } else
            {
                fout << ", VDD ); \n" << std::endl;
            }
        } else
        {
            fout << " ); \n" << std::endl;
        }

        //Display port names as list instead of individually
        std::vector<std::string> inPorts, outPorts, inOuts;

        for (IndexType i=0; i < newckt.numPorts(); ++i)
        {
            switch (newckt.pinArray(newckt.port(i)).dir())
            {
                case PinDir::INPUT: inPorts.emplace_back(newckt.pinArray(newckt.port(i)).name());
                                    break;
                case PinDir::OUTPUT: outPorts.emplace_back(newckt.pinArray(newckt.port(i)).name());
                                     break;
                case PinDir::INOUT: inOuts.emplace_back(newckt.pinArray(newckt.port(i)).name());
                                    break;
                default: inOuts.emplace_back(newckt.pinArray(newckt.port(i)).name()); //Ports not connecting to any cells within SUBCKT
            }
        }

        if (inPorts.size() > 0)
        {
            fout << "input " << inPorts[0];
            for (IndexType ip=1; ip < inPorts.size()-1; ++ip)
            {
                fout << ", " << inPorts[ip];
            }
            if (inPorts.size() > 1)
            {
                fout << ", " << inPorts[inPorts.size()-1] << ";" << std::endl;
            } else
            {
                fout << ";" << std::endl;
            }
        }
        if (outPorts.size() > 0)
        {
            fout << "output " << outPorts[0];
            for (IndexType ip=1; ip < outPorts.size()-1; ++ip)
            {
                fout << ", " << outPorts[ip];
            }
            if (outPorts.size() > 1)
            {
                fout << ", " << outPorts[outPorts.size()-1] << ";" << std::endl;
            } else
            {
                fout << ";" << std::endl;
            }
        }
        if ((inOuts.size() > 0) || (fVss > 0) || (fVdd > 0))
        {
            if (inOuts.size() > 0)
            {
                fout << "inout " << inOuts[0];
                for (IndexType ip=1; ip < inOuts.size()-1; ++ip)
                {
                    fout << ", " << inOuts[ip];
                }
                if (inOuts.size() > 1)
                {
                    fout << ", " << inOuts[inOuts.size()-1] << ";" << std::endl;
                } else
                {
                    fout << ";" << std::endl;
                }
            } else
            {
                fout << "inout ";

                if ((fVss > 0) && (fVdd > 0))
                {
                    fout << "VSS, VDD;" << std::endl;
                } else if (fVss > 0)
                {
                    fout << "VSS;" << std::endl;
                } else
                {
                    fout << "VDD;" << std::endl;
                }
            }
        }
    } else 
    {
        if ((fVss > 0) || (fVdd > 0))
        {
            if ((fVss > 0) && (fVdd > 0))
            {
                fout << "VSS, VDD ); \n" << std::endl;
            } else if (fVss > 0)
            {
                fout << "VSS ); \n" << std::endl;
            } else
            {
                fout << "VDD ); \n" << std::endl;
            }
        } else
        {
            fout << "); \n" << std::endl;
        }
    }

    //Wires definition
    if (wireList.size() > 0)
    {
        fout << "wire ";
        for (IndexType i=0; i < wireList.size()-1; ++i)
        {
            fout << newckt.pinArray(wireList[i]).name() << ", ";
        }
        fout << newckt.pinArray(wireList[wireList.size()-1]).name() << ";" << std::endl;
    }
    fout << std::endl;
}

void netlistHeader(std::ofstream &fout, Module &nDB)
{
    std::vector<IndexType> wireList(nDB.wires());
    fout << "module " << nDB.name() << " ( "; 

    //Check wires to find VDD/VSS before writing out port info
    std::sort( wireList.begin(), wireList.end() );
    wireList.erase( std::unique( wireList.begin(), wireList.end() ), wireList.end() );

    //Find and delete
    std::vector<IndexType> wlTies;
    Byte fVdd(0), fVss(0);

    for (auto welem : wireList)
    {
        if(nDB.pinArray(welem).name().find("tie") != std::string::npos)
        {
            wlTies.emplace_back(welem);

            //Update tie-off name directly
            if (nDB.pinArray(welem).name().find("tieHi") != std::string::npos)
            {
                nDB.pinArray(welem).setName("VDD");
            } else
            {
                nDB.pinArray(welem).setName("VSS");
            }
        }

        if(nDB.pinArray(welem).name().find("VSS") != std::string::npos)
        {
            fVss = 1;
            wlTies.emplace_back(welem);
        }

        if(nDB.pinArray(welem).name().find("VDD") != std::string::npos)
        {
            fVdd = 1;
            wlTies.emplace_back(welem);
        }
    }

    for(auto wlt : wlTies) 
    {
        wireList.erase(std::remove(wireList.begin(), wireList.end(), wlt), wireList.end());
    } 

    if (nDB.numPorts() > 0)
    {
        for (IndexType i=0; i < nDB.numPorts()-1; ++i) 
        {
            fout << nDB.pinArray(nDB.port(i)).name() << ", ";
        }
        fout << nDB.pinArray(nDB.port(nDB.numPorts()-1)).name();

        if ((fVss > 0) || (fVdd > 0))
        {
            if ((fVss > 0) && (fVdd > 0))
            {
                fout << ", VSS, VDD ); \n" << std::endl;
            } else if (fVss > 0)
            {
                fout << ", VSS ); \n" << std::endl;
            } else
            {
                fout << ", VDD ); \n" << std::endl;
            }
        } else
        {
            fout << " ); \n" << std::endl;
        }

        //Display port names as list instead of individually
        std::vector<std::string> inPorts, outPorts, inOuts;

        for (IndexType i=0; i < nDB.numPorts(); ++i)
        {
            switch (nDB.pinArray(nDB.port(i)).dir())
            {
                case PinDir::INPUT: inPorts.emplace_back(nDB.pinArray(nDB.port(i)).name());
                                    break;
                case PinDir::OUTPUT: outPorts.emplace_back(nDB.pinArray(nDB.port(i)).name());
                                     break;
                case PinDir::INOUT: inOuts.emplace_back(nDB.pinArray(nDB.port(i)).name());
                                    break;
                default: inOuts.emplace_back(nDB.pinArray(nDB.port(i)).name()); //Ports not connecting to any cells within SUBCKT
            }
        }

        if (inPorts.size() > 0)
        {
            fout << "input " << inPorts[0];
            for (IndexType ip=1; ip < inPorts.size()-1; ++ip)
            {
                fout << ", " << inPorts[ip];
            }
            if (inPorts.size() > 1)
            {
                fout << ", " << inPorts[inPorts.size()-1] << ";" << std::endl;
            } else
            {
                fout << ";" << std::endl;
            }
        }
        if (outPorts.size() > 0)
        {
            fout << "output " << outPorts[0];
            for (IndexType ip=1; ip < outPorts.size()-1; ++ip)
            {
                fout << ", " << outPorts[ip];
            }
            if (outPorts.size() > 1)
            {
                fout << ", " << outPorts[outPorts.size()-1] << ";" << std::endl;
            } else
            {
                fout << ";" << std::endl;
            }
        }
        if ((inOuts.size() > 0) || (fVss > 0) || (fVdd > 0))
        {
            if (inOuts.size() > 0)
            {
                fout << "inout " << inOuts[0];
                for (IndexType ip=1; ip < inOuts.size()-1; ++ip)
                {
                    fout << ", " << inOuts[ip];
                }
                if (inOuts.size() > 1)
                {
                    fout << ", " << inOuts[inOuts.size()-1] << ";" << std::endl;
                } else
                {
                    fout << ";" << std::endl;
                }
            } else
            {
                fout << "inout ";

                if ((fVss > 0) && (fVdd > 0))
                {
                    fout << "VSS, VDD;" << std::endl;
                } else if (fVss > 0)
                {
                    fout << "VSS;" << std::endl;
                } else
                {
                    fout << "VDD;" << std::endl;
                }
            }
        }
    } else 
    {
        if ((fVss > 0) || (fVdd > 0))
        {
            if ((fVss > 0) && (fVdd > 0))
            {
                fout << "VSS, VDD ); \n" << std::endl;
            } else if (fVss > 0)
            {
                fout << "VSS ); \n" << std::endl;
            } else
            {
                fout << "VDD ); \n" << std::endl;
            }
        } else
        {
            fout << "); \n" << std::endl;
        }
    }

    //Wires definition
    if (wireList.size() > 0)
    {
        fout << "wire ";
        for (IndexType i=0; i < wireList.size()-1; ++i)
        {
            fout << nDB.pinArray(wireList[i]).name() << ", ";
        }
        fout << nDB.pinArray(wireList[wireList.size()-1]).name() << ";" << std::endl;
    }

    fout << std::endl;
}

void writeVerilog(std::ofstream &fout, Module &nDB, Subckt &newckt, bool const mode)
{
    IndexType gIdx(0);
    //Mode options: 0 - header/ports definition; 1- instance definitions; 2- gate definition
    switch (mode) {

        case 0 : //Module and ports definition
            if (newckt.isLeaf()) 
            {
                netlistHeader(fout, nDB);
            } else
            {
                subcktHeader(fout, newckt);
            }
            break;
        case 1 : 
            {
                //list of instances/gates
                for (IndexType i=0; i < nDB.numCells(); ++i)
                {
                    if (nDB.cellArray(i).type() != GateType::INVALID)
                    {
                        ++gIdx;
                        fout << nDB.cellArray(i).gName() << " " << nDB.cellArray(i).cName()[0] << gIdx << " (";
                        if (nDB.cellArray(i).numPins() > 0) 
                        {
                            for (IndexType cl=0; cl < nDB.cellArray(i).numPins()-1; ++cl) 
                            {
                                //Check for tie off and rename
                                if(nDB.pinArray(nDB.cellArray(i).pinId(cl)).name().find("tie") != std::string::npos)
                                {
                                    //Update tie-off name directly
                                    if (nDB.pinArray(nDB.cellArray(i).pinId(cl)).name().find("tieHi") != std::string::npos)
                                    {
                                        nDB.pinArray(nDB.cellArray(i).pinId(cl)).setName("VDD");
                                    } else
                                    {
                                        nDB.pinArray(nDB.cellArray(i).pinId(cl)).setName("VSS");
                                    }
                                }

                                if (nDB.cellArray(i).includePname() && (nDB.cellArray(i).numpNames() == nDB.cellArray(i).numPins()))
                                {
                                    fout << "." << nDB.cellArray(i).pName(cl) << "(" << nDB.pinArray(nDB.cellArray(i).pinId(cl)).name() << "), ";
                                } else
                                {
                                    fout << nDB.pinArray(nDB.cellArray(i).pinId(cl)).name() << ", ";
                                }
                            }
                            //Check for tie off and rename
                            if(nDB.pinArray(nDB.cellArray(i).pinId(nDB.cellArray(i).numPins()-1)).name().find("tie") != std::string::npos)
                            {
                                //Update tie-off name directly
                                if (nDB.pinArray(nDB.cellArray(i).pinId(nDB.cellArray(i).numPins()-1)).name().find("tieHi") != std::string::npos)
                                {
                                    nDB.pinArray(nDB.cellArray(i).pinId(nDB.cellArray(i).numPins()-1)).setName("VDD");
                                } else
                                {
                                    nDB.pinArray(nDB.cellArray(i).pinId(nDB.cellArray(i).numPins()-1)).setName("VSS");
                                }
                            }

                            if (nDB.cellArray(i).includePname() && (nDB.cellArray(i).numpNames() == nDB.cellArray(i).numPins()))
                            {
                                fout << "." << nDB.cellArray(i).pName(nDB.cellArray(i).numPins()-1) << "(" 
                                    << nDB.pinArray(nDB.cellArray(i).pinId(nDB.cellArray(i).numPins()-1)).name() << ") );";
                            } else
                            {
                                fout << nDB.pinArray(nDB.cellArray(i).pinId(nDB.cellArray(i).numPins()-1)).name() << ");";
                            }

                            if (nDB.cellArray(i).type() == GateType::NOT_GATE)
                            {
                                fout << "//";
                                if (nDB.cellArray(i).lValue() > 0)
                                {
                                    fout << " l=" << nDB.cellArray(i).lValue() << "n ";
                                }
                                if (nDB.cellArray(i).wValue() > 0)
                                {
                                    fout << " w=" << nDB.cellArray(i).wValue() << "n ";
                                }
                                if (nDB.cellArray(i).fins() > 0)
                                {
                                    fout << " nfin=" << nDB.cellArray(i).fins() << " "; 
                                }
                                for (IndexType f = 0; f < nDB.cellArray(i).numfeatures(); ++f)
                                {
                                    fout << " " << nDB.cellArray(i).feature(f) << " ";
                                }
                                if (nDB.cellArray(i).mFactor() > 1)
                                {
                                    fout << " m=" << nDB.cellArray(i).mFactor() << " ";
                                }
                                if (nDB.cellArray(i).sFactor() > 1)
                                {
                                    fout << " s=" << nDB.cellArray(i).sFactor() << " ";
                                }
                                //Check if Cell is FET and is Analog (G-D/G-S/D-S tie-off)
                                if (nDB.cellArray(i).type() == GateType::NOT_GATE)
                                {
                                    if ((nDB.cellArray(i).pinId(0) == nDB.cellArray(i).pinId(1)) || (nDB.cellArray(i).pinId(1) == nDB.cellArray(i).pinId(2)) || 
                                            (nDB.cellArray(i).pinId(0) == nDB.cellArray(i).pinId(2)))
                                    {
                                        nDB.cellArray(i).setAnalog();
                                    } 
                                }
                                if ((nDB.numPorts() == 0) || (nDB.cellArray(i).isAnalog()) || (nDB.cellArray(i).numPins() < 4))
                                {
                                    fout << "ANALOG//";
                                }
                            }
                        } else
                        {
                            fout << " );";
                        }
                        fout << std::endl;
                    }
                }
                break;
            }
    }
}

//Dump out verilog file -hierarchical
void Database::outVerilog(bool const spOut, std::string const &spiLib)
{
    std::ofstream fout, fspice;
    IndexType startIndex = _subcktArray.size()-1;
    std::string outVerilog = _subcktArray[startIndex].name() + ".v";//get top module name - last subckt 
    fout.open(outVerilog);
    fout << "// SPICE2Verilog Conversion" << std::endl;
    fout << std::endl;

    if (spOut)
    {
        fspice.open(_subcktArray[startIndex].name() + ".sp");
        fspice << "************************************************" << std::endl;
        fspice << ".INCLUDE " << spiLib << std::endl; 
        fspice << "************************************************" << std::endl;
        fspice << std::endl;
    }

    //Using DFS
    std::stack<IndexType> traversal;       
    traversal.push(startIndex);
    std::vector<IndexType> spiceOut;
    spiceOut.emplace_back(startIndex);
    std::vector<bool> tmap(_subcktArray.size(), false);

    GraphP::graphp nGraph = _gPort.getGraph();

    while (!traversal.empty())
    {
        IndexType val = traversal.top();
        Subckt &newckt = _subcktArray[val];
        spiceOut.emplace_back(val);

        //Port update moved before flattening of leaf subckts - Retaining for non leaf subckts here 
        if ((_gPortMap[val].size() > 0) && (_gPortMap.find(val) != _gPortMap.end()) && (val < _subcktArray.size()-1))
        {
            if (_gPortMap[val].size() == 1)
            {
                //Update all ports for unique instances
                for (IndexType i=0; i < _netlistArray[val].numPorts(); ++i)
                {
                    if (_netlistArray[val].pinName2IdMap.find(nGraph[_gPortMap[val][0]].port(i)) != _netlistArray[val].pinName2IdMap.end())
                    {
                        _netlistArray[val].pinArray(_netlistArray[val].pinName2IdMap[nGraph[_gPortMap[val][0]].port(i)]).setName("net"+nGraph[_gPortMap[val][0]].port(i));
                        _netlistArray[val].pinName2IdMap["net"+nGraph[_gPortMap[val][0]].port(i)] = _netlistArray[val].pinName2IdMap[nGraph[_gPortMap[val][0]].port(i)];
                    }
                    _netlistArray[val].pinArray(_netlistArray[val].port(i)).setName(nGraph[_gPortMap[val][0]].port(i));
                    _netlistArray[val].pinName2IdMap[nGraph[_gPortMap[val][0]].port(i)] = _netlistArray[val].port(i);
                }
            } else
            {
                //Update power for multiple instantiations
                for (IndexType i=0; i < _netlistArray[val].numPorts(); ++i)
                {
                    if (nGraph[_gPortMap[val][0]].isPower(i))
                    {
                        _netlistArray[val].pinArray(_netlistArray[val].port(i)).setName(nGraph[_gPortMap[val][0]].port(i));
                        _netlistArray[val].pinName2IdMap[nGraph[_gPortMap[val][0]].port(i)] = _netlistArray[val].port(i);
                    }
                }
            }
        } else
        {
            //INF("%s is not found in portMap", _netlistArray[val].name().c_str());
        }

        if (!newckt.isTraversed())
        {
            //if top subckt doesn't have component-X stop else get component-X elements - DFS
            writeVerilog(fout, _netlistArray[val], newckt, 0); //header + ports

            Module vOut = _netlistArray[val];
            if(!newckt.isLeaf()) 
            {
                //Update stack
                for (IndexType i=0; i < newckt.numInsts(); ++i)
                {
                    if(newckt.instArray(i).type() == InstType::COMPONENT) 
                    {
                        if (tmap[_subcktToIdMap[newckt.instArray(i).cName()]] == false)
                        {
                            traversal.push(_subcktToIdMap[newckt.instArray(i).cName()]);
                            tmap[_subcktToIdMap[newckt.instArray(i).cName()]] = true;
                        }
                    }
                }
                //Re-add top element - As it is not possible to access bottom element in stack 
                traversal.push(startIndex);

                IndexType gCount(0);
                std::vector<IndexType> insts;
                for(IndexType j=0; j < newckt.numInsts(); ++j)
                {
                    insts.emplace_back(j);
                    ++gCount;
                    addGate(0, newckt, insts, gCount, vOut); 
                    insts.clear();
                }
            }
            writeVerilog(fout,vOut, newckt, 1); //List all instances
            fout << std::endl;
            fout << "endmodule \n//////////////////////////////////" << std::endl;
            fout << std::endl;

            newckt.setTraversed();
            tmap[val] = true;
            _netlistArray[val] = vOut;
        }
        else
        {
            tmap[val] = true;
        }
        traversal.pop();
    }

    fout.close();

    INF("Completed writing verilog file");

    //Dump out SPICE netlist
    if (spOut)
    {
        std::sort( spiceOut.begin(), spiceOut.end() );
        spiceOut.erase( std::unique( spiceOut.begin(), spiceOut.end() ), spiceOut.end() );

        for (IndexType idx = 0; idx < spiceOut.size(); ++idx)
        {
            IndexType val = spiceOut[idx];
            Subckt newckt = _subcktArray[val];
            Module vOut = _netlistArray[val];

            fspice << ".SUBCKT " << newckt.name();

            //List of ports
            for (IndexType spt = 0; spt < newckt.numPorts(); ++spt)
            {
                fspice << " " << newckt.pinArray(newckt.port(spt)).name();
            }
            fspice << std::endl;

            //list of instances/gates
            for (IndexType c=0; c < vOut.numCells(); ++c)
            {
                fspice << vOut.cellArray(c).cName();

                for (IndexType cl=0; cl < vOut.cellArray(c).numPins(); ++cl) 
                {
                    fspice << " " << vOut.pinArray(vOut.cellArray(c).pinId(cl)).name();
                }

                fspice << " " << vOut.cellArray(c).gName();

                //List features if they exist
                if ( (vOut.cellArray(c).numfeatures() > 0) || (vOut.cellArray(c).type() == GateType::NOT_GATE) )
                {
                    if (vOut.cellArray(c).lValue() > 0)
                    {
                        fspice << " l=" << vOut.cellArray(c).lValue() << "n";
                    }
                    if (vOut.cellArray(c).wValue() > 0)
                    {
                        fspice << " w=" << vOut.cellArray(c).wValue() << "n";
                    }
                    if (vOut.cellArray(c).fins() > 0)
                    {
                        fspice << " nfin=" << vOut.cellArray(c).fins(); 
                    }
                    for (IndexType f = 0; f < vOut.cellArray(c).numfeatures(); ++f)
                    {
                        fspice << " " << vOut.cellArray(c).feature(f);
                    }
                    if (vOut.cellArray(c).mFactor() > 1)
                    {
                        fspice << " m=" << vOut.cellArray(c).mFactor();
                    }
                    if (vOut.cellArray(c).sFactor() > 1)
                    {
                        fspice << " s=" << vOut.cellArray(c).sFactor();
                    }
                    fspice << std::endl;
                } else
                {
                    fspice << std::endl;
                }
            }

            fspice << ".ENDS" << std::endl;
            fspice << "************************************************" << std::endl;

        }
        fspice.close();
        INF("Completed writing out SPICE file");
    }
}

//Write out dot file for visualization at subckt level
void Database::dotOut()
{
    std::ofstream fout;
    IndexType startIndex = _netlistArray.size()-1;
    if (OneDotOut)
    {
        std::string outDot = _netlistArray[startIndex].name() + ".dot";//get top module name - last subckt 
        fout.open(outDot);
    }

    //Using DFS
    std::stack<IndexType> traversal;       
    traversal.push(startIndex);
    std::vector<bool> tmap(_netlistArray.size(), false);

    while (!traversal.empty())
    {
        IndexType val = traversal.top();
        Module mod = _netlistArray[val];
        Subckt newckt = _subcktArray[val];

        if ((tmap[val] == false) && !newckt.isAnalog())
        {
            if (!OneDotOut)
            {
                std::string outDot = mod.name() + ".dot";//get top module name - last subckt 
                fout.open(outDot);
            }

            fout << "digraph " << mod.name() << "{" << std::endl;
            fout << " labelloc=\"t\";\n label=\"" << mod.name() << "\"; fontsize=18; fontname=Times;" << std::endl;
            fout << " splines=true;" << std::endl;
            fout << " overlap=false;" << std::endl;
            std::unordered_map<std::string, IndexType> pinMap;

            //Port definitions
            for (IndexType pn=0; pn < newckt.numPorts(); ++pn)
            {
                std::string pinName;
                IndexType pId;
                PinDir pDir;
                bool isPower(false);

                pinName = newckt.pinArray(newckt.port(pn)).name();
                pId = newckt.port(pn);
                pDir = newckt.pinArray(newckt.port(pn)).dir();
                if(newckt.pinArray(newckt.port(pn)).isPower())
                {
                    isPower = true;
                }

                switch (pDir)
                {
                    case PinDir::INPUT:
                        pinMap[pinName] = 1;
                        fout << "P" << pId << " [label=\"" << pinName  << "\", shape=circle, color=blue, fontsize=12, fontname=Times]; " << std::endl;
                        break;
                    case PinDir::OUTPUT:
                        pinMap[pinName] = 0;
                        fout << "P" << pId << " [label=\"" << pinName  << "\", shape=circle, color=deeppink, fontsize=12, fontname=Times]; " << std::endl;
                        break;
                    default:
                        pinMap[pinName] = 2;
                        if (isPower)
                        {
                            fout << "P" << pId << " [label=\"" << pinName  << "\", shape=circle, style=dotted, color=gray, fontsize=10, fontname=Times]; " << std::endl;
                        } else
                        {
                            fout << "P" << pId << " [label=\"" << pinName  << "\", shape=circle, color=black, fontsize=12, fontname=Times]; " << std::endl;
                        }
                }
            }

            //Wire definitions
            std::vector<IndexType> wireList;
            if (newckt.isLeaf())
            {
                wireList = mod.wires();

                std::sort( wireList.begin(), wireList.end() );
                wireList.erase( std::unique( wireList.begin(), wireList.end() ), wireList.end() );

                for (IndexType w=0; w < wireList.size(); ++w)
                {
                    fout << "P" << wireList[w] << " [label=\"" << mod.pinArray(wireList[w]).name() 
                        << "\", shape=square, style=dotted, color=brown, fontsize=10, fontname=Times]; " << std::endl;
                    pinMap[mod.pinArray(wireList[w]).name()] = 5;
                }

            } else
            {
                wireList = newckt.wires();

                std::sort( wireList.begin(), wireList.end() );
                wireList.erase( std::unique( wireList.begin(), wireList.end() ), wireList.end() );

                for (IndexType w=0; w < wireList.size(); ++w)
                {
                    fout << "P" << wireList[w] << " [label=\"" << newckt.pinArray(wireList[w]).name() 
                        << "\", shape=square, style=dotted, color=brown, fontsize=10, fontname=Times]; " << std::endl;
                    pinMap[newckt.pinArray(wireList[w]).name()] = 5;
                }
            }

            fout << std::endl;

            for (IndexType cl=0; cl < mod.numCells(); ++cl)
            {
                //Node initialization for different devices
                if (mod.cellArray(cl).type() == GateType::COMPONENT)
                {
                    traversal.push(_subcktToIdMap[mod.cellArray(cl).gName()]);
                    fout << mod.cellArray(cl).gName() << "_" << mod.cellArray(cl).cName() << "[shape=component, fontsize=14, fontname=Times]; " << std::endl;

                } else if (mod.cellArray(cl).type() == GateType::NOT_GATE)
                {
                    fout << mod.cellArray(cl).gName() << "_" << mod.cellArray(cl).cName() << "[shape=parallelogram, fontsize=12, fontname=Times]; " << std::endl;
                } else
                {
                    if (mod.cellArray(cl).type() != GateType::INVALID)
                    {
                        fout << mod.cellArray(cl).gName() << "_" << mod.cellArray(cl).cName() << "[shape=box3d, fontsize=14, fontname=Times]; " << std::endl;
                    }
                }

                //Connectivity information
                for (IndexType cp=0; cp < mod.cellArray(cl).numPins(); ++cp)
                {
                    std::string cellName(mod.cellArray(cl).gName()+"_"+mod.cellArray(cl).cName()), pinName;
                    IndexType pId, rCount(0);
                    pinName = mod.pinArray(mod.cellArray(cl).pinId(cp)).name();
                    pId = mod.cellArray(cl).pinId(cp);

                    switch(pinMap[pinName])
                    {
                        case 0: 
                            fout << cellName << " -> P" << pId << " [color=deeppink, style=solid]" << std::endl;
                            break;
                        case 1: 
                            fout << "P" << pId << " -> " << cellName << " [color=blue, style=solid]" << std::endl;
                            break;
                        case 2: 
                            if (mod.pinArray(mod.pinName2IdMap[pinName]).isPower())
                            {
                                fout << "P" << pId << " -> " << cellName << " [dir=none, style=dotted, color=gray]" << std::endl;
                            } else
                            {
                                fout << "P" << pId << " -> " << cellName << " [dir=none, style=solid, color=black]" << std::endl;
                            }
                            break;
                        default: 
                            fout << cellName << " -> P" << pId << " [dir=none, style=solid, color=brown]" << std::endl;
                            break;
                    }
                    ++rCount;
                }
                fout << std::endl;
            }
            traversal.push(startIndex);
            fout << "}\n" << std::endl;

            tmap[val] = true;

            if (!OneDotOut)
            {
                fout.close();
            }
        }
        traversal.pop();
    }
    if (OneDotOut)
    {
        fout.close();
    }

    INF("Completed writing out DOT file for graph visualization");
}

PROJECT_NAMESPACE_END
