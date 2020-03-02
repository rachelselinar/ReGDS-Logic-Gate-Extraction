//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin
#include <fstream>
#include <stack>
#include <list>
#include "global/global.h"
#include "global/type.h"
#include "db/Database.h"
#include "parser/BookshelfDriver.h"
#include "util/Boost_graph_wrapper.h"

PROJECT_NAMESPACE_BEGIN

///Parse library information and update
void Database::libraryUpdate(std::string const &libPath)
{
    _fileInfo.setAuxFile(libPath);
    BookshelfParser::Driver driver(*this);
    std::ifstream ifs;

    // Parse bookshelf AUX file
    INF("Parsing file %s", libPath.c_str());
    ifs.open(libPath);
    if (! ifs.good())
    {
        ERR("Cannot open file %s", libPath.c_str());
    }
    driver.parse_stream(ifs);
    INF("Completed parsing file %s", libPath.c_str());
    ifs.close();

    //Use library information to create all library dependency files
    Subckt libCkt;
    Inst libInst;
    std::vector<Subckt> libCktArray;
    std::unordered_map<std::string, IndexType> libCkt2IdMap;
    std::ofstream libGateh, libGatec, libGateT, libFindG, spiLibPath;

    libGateh.open("../src/library/lib_gates.h", std::ofstream::out | std::ofstream::trunc); //Settings to delete previous content in file
    libGatec.open("../src/library/lib_gates.cpp", std::ofstream::out | std::ofstream::trunc); //Settings to delete previous content in file
    libGateT.open("../src/library/lib_gateType.cpp", std::ofstream::out | std::ofstream::trunc); //Settings to delete previous content in file
    libFindG.open("../src/library/lib_findGate.cpp", std::ofstream::out | std::ofstream::trunc); //Settings to delete previous content in file
    spiLibPath.open("../src/library/lib_path.cpp", std::ofstream::out | std::ofstream::trunc); //Settings to delete previous content in file

    //Provide lib path file
    spiLibPath << "spiLib = \"" << libPath << "\";" << std::endl;

    //Ensure all subckts in the library are leaf elements
    bool findComp(false);
    std::vector<Subckt> subcktsOrd;

    for (IndexType i = 0; i < _subcktArray.size(); ++i)
    {
        //All Subckts have instances - subckts without instances are ignored during parsing!
        Subckt &newckt = _subcktArray[i];

        if ((newckt.nfet() + newckt.pfet()) > 0) 
        {
            newckt.setLeaf();
            subcktsOrd.emplace_back(newckt);
        }
        if (newckt.comp() > 0)
        {
            findComp = true;
        }
    }

    if (findComp)
    {
        ERR("ABORT: Library contains hierarchical subckts!");
    }

    std::sort(subcktsOrd.begin(), subcktsOrd.end(), largerSize());

    //Update all dependency files
    for (IndexType gt = 0; gt < subcktsOrd.size(); ++gt)
    {
        GraphB sampleG;

        //Update in Boost_graph_wrapper.h
        libGateh << "\n\tvoid find" << subcktsOrd[gt].name() 
                 << "(Subckt& newckt, graphType& graph, std::vector<IndexType>& gateMap, std::vector<bool>& gateSet, std::vector<Vertex> &vArray, std::vector<IndexType>& cktMap);";
        libGatec << "\n inline void GraphB::find" << subcktsOrd[gt].name() << "(Subckt& newckt, GraphB::graphType& graph, std::vector<IndexType>& gateMap, std::vector<bool>& gateSet,"; 
        libGatec << "\n\t\t\t\t\t\t\t\t\tstd::vector<Vertex> &vArray, std::vector<IndexType>& cktMap)";
        libGatec << "\n{";
        libGatec << "\n\tGraphB graph_1;";

        sampleG.createGraphLib(subcktsOrd[gt], libGatec);

        libGatec << "\n\tGraphB::graphType graph2 = graph_1.getGraph();";
        libGatec << "\n";
        libGatec << "\n\tvertex_comp_t vertex_comp2 = boost::make_property_map_equivalent(get(boost::vertex_name, graph2), get(boost::vertex_name, graph));";
        libGatec << "\n\tedge_comp_t edge_comp2 = boost::make_property_map_equivalent(boost::get(boost::edge_name, graph2), boost::get(boost::edge_name, graph));";
        libGatec << "\n";
        libGatec << "\n\tvf2_print_callback callback" << gt+1 << "(graph2, graph, " << subcktsOrd[gt].numInsts() <<", gateMap, gateSet, vArray, cktMap);";
        libGatec << "\n\tboost::vf2_subgraph_iso(graph2, graph, callback" << gt+1 <<", vertex_order_by_mult(graph2), edges_equivalent(edge_comp2).vertices_equivalent(vertex_comp2));";
        libGatec << "\n}";
        libGatec << "\n";

        //Update in Database.cpp
        libFindG << "\n\tif (vCount > " << subcktsOrd[gt].numInsts()-1 << ")";
        libFindG << "\n\t{";
        libFindG << "\n\t\tgraph.find" << subcktsOrd[gt].name() << "(newckt, g1, gateMap, gateSet, graph.getVertexArray(), cktMap);";
        libFindG << "\n\t\tif (gateMap.size() > 0)";
        libFindG << "\n\t\t{";
        libFindG << "\n\t\t\tvCount = vCount - gateMap.size();";
        libFindG << "\n\t\t\tupdateCell(newckt, nl, gateMap, cktMap, gCount, " << gt+1 << ", " << subcktsOrd[gt].numInsts() << ");";
        libFindG << "\n\t\t\tstd::sort(gateMap.begin(), gateMap.end(), std::greater<IndexType>());";
        libFindG << "\n\t\t\tfor (IndexType i = 0; i < gateMap.size(); ++i)";
        libFindG << "\n\t\t\t{";
        libFindG << "\n\t\t\t\tgraph.clearVertex(gateMap[i]);";
        libFindG << "\n\t\t\t}";
        libFindG << "\n\t\t}";
        libFindG << "\n\t\tgateMap.clear();";
        libFindG << "\n\t}";
        libFindG << "\n";

        libGateT << "\n\t\tcase " << gt+1 << ": //" << subcktsOrd[gt].name(); 
        libGateT << "\n\t\t\t{";
        libGateT << "\n\t\t\t\tgType = \"" << subcktsOrd[gt].name() << "\";"; 
        libGateT << "\n\t\t\t\tcellName = \"X\" + toString(gCount);";
        libGateT << "\n";
        libGateT << "\n\t\t\t\tnewcell.setName(cellName);";
        libGateT << "\n\t\t\t\tnewcell.setGName(gType);";
        libGateT << "\n\t\t\t\tnewcell.setId(nl.numCells());";
        libGateT << "\n\t\t\t\tnewcell.setType(GateType::GATE);";
        libGateT << "\n";

        std::vector<IndexType> remPorts;
        std::vector<std::string> portConns(subcktsOrd[gt].numPorts(), "\n");
        for (IndexType pt = 0; pt < subcktsOrd[gt].numPorts(); ++pt)
        {
            bool found(false);
            for (IndexType nt = 0; nt < subcktsOrd[gt].netArray(subcktsOrd[gt].port(pt)).numInsts(); ++nt)
            {
                IndexType instID(subcktsOrd[gt].netArray(subcktsOrd[gt].port(pt)).instId(nt));

                for (IndexType ip = 0; ip < subcktsOrd[gt].instArray(instID).numPins(); ++ip)
                {
                    if (subcktsOrd[gt].port(pt) == subcktsOrd[gt].instArray(instID).pinId(ip))
                    {
                        if ((ip == 1) || (ip == 3))
                        {
                            portConns[pt] += "\t\t\t\tcPins.emplace_back(newckt.instArray(Insts[" + toString(subcktsOrd[gt].netArray(subcktsOrd[gt].port(pt)).instId(nt))
                                           + "]).pinId(" + toString(ip) + "));";
                            portConns[pt] += "\n\t\t\t\tcCons.emplace_back(\"" + subcktsOrd[gt].pinArray(subcktsOrd[gt].port(pt)).name() + "\");";

                            found = true;
                            break;
                        }
                    }
                }
                if (found)
                {
                    portConns[pt] += "\n";
                    break;
                } 
                if ((nt == subcktsOrd[gt].netArray(subcktsOrd[gt].port(pt)).numInsts()-1) && !found)
                {
                    remPorts.emplace_back(pt);
                }
            }
        }
        
        std::vector<IndexType> finalPorts;
        for (IndexType rp = 0; rp < remPorts.size(); ++rp)
        {
            bool foundPort(false);
            for (IndexType inst = 0; inst < subcktsOrd[gt].netArray(subcktsOrd[gt].port(remPorts[rp])).numInsts(); ++inst)
            {
                IndexType instID(subcktsOrd[gt].netArray(subcktsOrd[gt].port(remPorts[rp])).instId(inst));

                if ((subcktsOrd[gt].instArray(instID).pinId(0) == subcktsOrd[gt].instArray(instID).pinId(3)) ||
                        (subcktsOrd[gt].instArray(instID).pinId(2) == subcktsOrd[gt].instArray(instID).pinId(3))) // (0 or 2) eq 3
                {
                    portConns[remPorts[rp]] += "\t\t\t\tif (newckt.instArray(Insts[" + toString(instID) + "]).pinId(0) == newckt.instArray(Insts[" + toString(instID) + "]).pinId(3))";
                    portConns[remPorts[rp]] += "\n\t\t\t\t{";
                    portConns[remPorts[rp]] += "\n\t\t\t\t\tcPins.emplace_back(newckt.instArray(Insts[" + toString(instID) + "]).pinId(2));";
                    portConns[remPorts[rp]] += "\n\t\t\t\t\tcCons.emplace_back(\"" + subcktsOrd[gt].pinArray(subcktsOrd[gt].port(remPorts[rp])).name() + "\");";
                    portConns[remPorts[rp]] += "\n\t\t\t\t} else";
                    portConns[remPorts[rp]] += "\n\t\t\t\t{";
                    portConns[remPorts[rp]] += "\n\t\t\t\t\tcPins.emplace_back(newckt.instArray(Insts[" + toString(instID) + "]).pinId(0));";
                    portConns[remPorts[rp]] += "\n\t\t\t\t\tcCons.emplace_back(\"" + subcktsOrd[gt].pinArray(subcktsOrd[gt].port(remPorts[rp])).name() + "\");";
                    portConns[remPorts[rp]] += "\n\t\t\t\t}";
                    portConns[remPorts[rp]] += "\n";
                    foundPort = true;
                    break;
                } 
                if ((inst == subcktsOrd[gt].netArray(subcktsOrd[gt].port(remPorts[rp])).numInsts()-1) && !foundPort)
                {
                    finalPorts.emplace_back(remPorts[rp]);
                }
            }
        }

        for (IndexType fp = 0; fp < finalPorts.size(); ++fp)
        {
            bool foundPort(false);
            for (IndexType inst = 0; inst < subcktsOrd[gt].netArray(subcktsOrd[gt].port(finalPorts[fp])).numInsts(); ++inst)
            {
                IndexType instID(subcktsOrd[gt].netArray(subcktsOrd[gt].port(finalPorts[fp])).instId(inst));

                if (!((subcktsOrd[gt].instArray(instID).pinId(0) == subcktsOrd[gt].instArray(instID).pinId(3)) ||
                      (subcktsOrd[gt].instArray(instID).pinId(2) == subcktsOrd[gt].instArray(instID).pinId(3)))) // (0 or 2) eq 3
                {
                    IndexType otPin;

                    if (subcktsOrd[gt].instArray(instID).pinId(0) == subcktsOrd[gt].port(finalPorts[fp]))
                    {
                        otPin = subcktsOrd[gt].instArray(instID).pinId(2);
                    } else
                    {
                        otPin = subcktsOrd[gt].instArray(instID).pinId(0);
                    }

                    for (IndexType otInst = 0; otInst < subcktsOrd[gt].netArray(otPin).numInsts(); ++otInst)
                    {
                        IndexType oID(subcktsOrd[gt].netArray(otPin).instId(otInst));

                        if ((subcktsOrd[gt].instArray(oID).pinId(0) == subcktsOrd[gt].instArray(oID).pinId(3)) ||
                                (subcktsOrd[gt].instArray(oID).pinId(2) == subcktsOrd[gt].instArray(oID).pinId(3))) // (0 or 2) eq 3
                        {
                            portConns[finalPorts[fp]] += "\t\t\t\tif (newckt.instArray(Insts[" + toString(oID) + "]).pinId(0) == newckt.instArray(Insts[" 
                                                         + toString(oID) + "]).pinId(3))";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t{";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\tif (newckt.instArray(Insts[" + toString(oID) + "]).pinId(2) == newckt.instArray(Insts[" 
                                                         + toString(instID) + "]).pinId(0))";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t{";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcPins.emplace_back(newckt.instArray(Insts[" + toString(instID) + "]).pinId(2));";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcCons.emplace_back(\"" + subcktsOrd[gt].pinArray(subcktsOrd[gt].port(finalPorts[fp])).name() + "\");";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t} else";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t{";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcPins.emplace_back(newckt.instArray(Insts[" + toString(instID) + "]).pinId(0));";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcCons.emplace_back(\"" + subcktsOrd[gt].pinArray(subcktsOrd[gt].port(finalPorts[fp])).name() + "\");";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t}";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t} else if (newckt.instArray(Insts[" + toString(oID) + "]).pinId(2) == newckt.instArray(Insts[" 
                                                         + toString(oID) + "]).pinId(3))";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t{";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\tif (newckt.instArray(Insts[" + toString(oID) + "]).pinId(0) == newckt.instArray(Insts[" 
                                                         + toString(instID) + "]).pinId(0))";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t{";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcPins.emplace_back(newckt.instArray(Insts[" + toString(instID) + "]).pinId(2));";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcCons.emplace_back(\"" + subcktsOrd[gt].pinArray(subcktsOrd[gt].port(finalPorts[fp])).name() + "\");";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t} else";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t{";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcPins.emplace_back(newckt.instArray(Insts[" + toString(instID) + "]).pinId(0));";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t\tcCons.emplace_back(\"" + subcktsOrd[gt].pinArray(subcktsOrd[gt].port(finalPorts[fp])).name() + "\");";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t\t}";
                            portConns[finalPorts[fp]] += "\n\t\t\t\t}";
                            portConns[finalPorts[fp]] += "\n";
                            foundPort = true;
                            break;
                        }
                    }
                    if (foundPort)
                    {
                        break;
                    }
                }
            }
        }

        for (IndexType val = 0; val < portConns.size(); ++val)
        {
            libGateT << portConns[val];
        }
        libGateT << "\n\t\t\t\tbreak;";
        libGateT << "\n\t\t\t}";
    }

    libGateh.close();
    libGatec.close();
    libGateT.close();
    libFindG.close();
    spiLibPath.close();

    INF("Successfully generated logic gate graphs from library");
    INF("-----------------------------------------------------");
    INF("Rebuild for Logic Gate Identification");
}

PROJECT_NAMESPACE_END
