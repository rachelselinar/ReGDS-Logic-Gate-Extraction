//LGE  GPL-3.0-or-later Copyright (C) 2020 The University of Texas at Austin

#ifndef VF2_SUBGRAPH_ISO_H_
#define VF2_SUBGRAPH_ISO_H_

/// ================================================================================ 
/// Vertex List Graph
/// A wrapper or implementation for boost vertex list graph type: Initial version obtained from Keren Zhu (UTDA) and modified by Rachel Selina Rajarathnam (UTDA).
/// Implementation use adjacency_list as graph.
/// It is VertexAndEdgeListGraph, MutablePropertyGraph, CopyConstructible, Assignable, and Serializable.
/// ================================================================================ 

#include <vector>
#include <fstream>
#include <typeinfo>
#include "global/global.h"
#include "global/namespace.h"
#include <boost/assert.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/one_bit_color_map.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>
#include <boost/graph/vf2_sub_graph_iso.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/graph/graphviz.hpp>

PROJECT_NAMESPACE_BEGIN

//*********************************************************//
//FOR PORT PROPAGATION
//*********************************************************//

class VertexType
{
public:
    explicit VertexType() = default;
    
    void                                addPort(std::string const &port)              { _portMap.emplace_back(port); }
    void                                addPwr(bool const &port)                      { _powerMap.emplace_back(port); }
    //Getters
    IndexType                           id() const                                    { return _id; }
    IndexType &                         id()                                          { return _id; }
    IndexType                           sid() const                                   { return _sId; }
    IndexType &                         sid()                                         { return _sId; }
    const std::vector<std::string> &    portArray() const                             { return _portMap; }
    std::vector<std::string> &          portArray()                                   { return _portMap; }
    std::string const                   port(IndexType i) const                       { return _portMap.at(i); }
    std::string &                       port(IndexType i)                             { return _portMap.at(i); }
    IndexType                           numPorts() const                              { return _portMap.size(); }

    const std::vector<bool> &           powerArray() const                            { return _powerMap; }
    std::vector<bool> &                 powerArray()                                  { return _powerMap; }
    bool const                          isPower(IndexType i) const                    { return _powerMap.at(i); }
    bool                                isPower(IndexType i)                          { return _powerMap.at(i); }

    // Setters
    void                                setId(IndexType id)                           { _id = id; }
    void                                setSubcktId(IndexType sid)                    { _sId = sid; }
    void                                setPowerPort(IndexType id)                    { _powerMap[id] = true; }

    void resetPowerMap()
    {
        _powerMap.resize(_portMap.size(), false);
    }

    void clear()
    {
        _id = 0;
        _sId = 0;
        _portMap.clear();
        _powerMap.clear();
    }
private:
    IndexType _id;
    IndexType _sId;
    std::vector<std::string> _portMap;
    std::vector<bool> _powerMap;
};


class GraphP 
{
public:

    typedef boost::property<boost::edge_name_t, IndexType> edge_property;
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexType, edge_property> graphp;
    typedef boost::graph_traits<graphp>::vertex_descriptor  Vertex;
    typedef boost::graph_traits<graphp>::edge_descriptor    Edge; 

    GraphP() = default;
    GraphP(IndexType vSize) :_graph(vSize) { _vertexArray.resize(vSize); }

    /// Getters
    const graphp &                  getGraph() const                                        { return _graph; }
    graphp &                        getGraph()                                              { return _graph; }
    const std::vector<Vertex> &     getVertexArray() const                                  { return _vertexArray; }
    std::vector<Vertex> &           getVertexArray()                                        { return _vertexArray; }
    const std::vector<Edge> &       getEdgeArray() const                                    { return _edgeArray; }
    std::vector<Edge> &             getEdgeArray()                                          { return _edgeArray; }

    /// Graph building/remove
    void      addVertex(IndexType sId, std::vector<std::string> &portMap, std::vector<bool> &powerMap); 
    void      removeEdge(IndexType fromIdx, IndexType toIdx);
    void      removeEdge(IndexType edgeIdx);
    void      clearVertex(IndexType idx);
    void      removeVertex(IndexType idx);
    IndexType addEdge(IndexType fromIdx, IndexType toIdx);

    Edge getEdge(IndexType fromIdx, IndexType toIdx) 
    {
        std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
        if (!edgePair.second) { PROJECT_NAMESPACE::WRN("%s: (%u, %u) not existing.", __PRETTY_FUNCTION__, fromIdx, toIdx); }
        return edgePair.first;
    }
    Edge getEdge(IndexType fromIdx, IndexType toIdx) const 
    {
        std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
        if (!edgePair.second) { PROJECT_NAMESPACE::WRN("%s: (%u, %u) not existing.", __PRETTY_FUNCTION__, fromIdx, toIdx); }
        return edgePair.first;
    }

    const Vertex &          getVertex(IndexType vertexIdx) const                 { return _vertexArray.at(vertexIdx); }
    Vertex &                getVertex(IndexType vertexIdx)                       { return _vertexArray.at(vertexIdx); }

    /// Size
    IndexType               vertexSize() const                                   { return _vertexArray.size(); }
    IndexType               graphSize() const                                    { return boost::num_vertices(_graph); }

private:
    graphp                     _graph;
    VertexType                 _vertex;
    std::vector<Vertex>        _vertexArray;
    std::vector<Edge>          _edgeArray;
};

inline void GraphP::addVertex(IndexType sId, std::vector<std::string> &portMap, std::vector<bool> &powerMap) 
{
    _vertex.clear();
    _vertex.setId(_vertexArray.size());
    _vertex.setSubcktId(sId);
    _vertex.portArray() = portMap;
    _vertex.powerArray() = powerMap;
    _vertexArray.emplace_back(boost::add_vertex(_vertex, _graph));
}

inline IndexType GraphP::addEdge(IndexType fromIdx, IndexType toIdx) 
{
    std::pair<Edge, bool> returnedEdge = boost::add_edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
    if (!returnedEdge.second) 
    {
        PROJECT_NAMESPACE::WRN("%s: Failed to add edge (%u, %u), the edge has already existed and the graph is not allowing parallel edge \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
    }
    IndexType idx = _edgeArray.size();
    _edgeArray.emplace_back(returnedEdge.first);
    return idx;
}

inline void GraphP::removeEdge(IndexType fromIdx, IndexType toIdx) 
{
    std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
    if (!edgePair.second) 
    {
        PROJECT_NAMESPACE::WRN("%s: Failed to remove the return, (%u, %u) does not exist \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
    }
    boost::remove_edge(edgePair.first, _graph);
}

inline void GraphP::removeEdge(IndexType edgeIdx) 
{
    boost::remove_edge(_edgeArray.at(edgeIdx), _graph);
}

inline void GraphP::clearVertex(IndexType idx) 
{
    boost::clear_vertex(_vertexArray.at(idx), _graph);
}

inline void GraphP::removeVertex(IndexType idx) 
{
    boost::remove_vertex(_vertexArray.at(idx), _graph);
}

//*********************************************************//
//FOR LOGIC GATE IDENTIFICATION USING SUB-GRAPH ISOMORPHISM
//*********************************************************//

class GraphB 
{
public: 

    typedef boost::property<boost::edge_name_t, IndexType> edge_property;
    typedef boost::property<boost::vertex_name_t, char, boost::property<boost::vertex_index_t, int> > vertex_property;
    // Using a vecS graphs => the index maps are implicit.
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, vertex_property, edge_property> graphType;
    // create predicates
    typedef boost::property_map<graphType, boost::vertex_index_t>::type IndexMap;
    typedef boost::property_map<graphType, boost::vertex_name_t>::type vertex_name_map_t;
    typedef boost::property_map_equivalent<vertex_name_map_t, vertex_name_map_t> vertex_comp_t;
    typedef boost::property_map<graphType, boost::edge_name_t>::type edge_name_map_t;
    typedef boost::property_map_equivalent<edge_name_map_t, edge_name_map_t> edge_comp_t;

    typedef boost::graph_traits<graphType>::vertex_descriptor  Vertex;
    typedef boost::graph_traits<graphType>::edge_descriptor    Edge; 

    GraphB() = default;
    GraphB(IndexType vSize) :_graph(vSize) { _vertexArray.resize(vSize);}
    explicit GraphB(Subckt& newckt, std::vector<IndexType> const& cktMap) { createGraph(newckt, cktMap); }

    graphType createGraph(Subckt& newckt, std::vector<IndexType> const& cktMap);
    void createGraphLib(Subckt& newckt, std::ofstream& libGatec);

    /// Getters
    const graphType &                  getGraph() const                                        { return _graph; }
    graphType &                        getGraph()                                              { return _graph; }
    const std::vector<Vertex> &        getVertexArray() const                                  { return _vertexArray; }
    std::vector<Vertex> &              getVertexArray()                                        { return _vertexArray; }
    const std::vector<Edge> &          getEdgeArray() const                                    { return _edgeArray; }
    std::vector<Edge> &                getEdgeArray()                                          { return _edgeArray; }

    /// Graph building/remove
    void      removeEdge(IndexType fromIdx, IndexType toIdx);
    void      removeEdge(IndexType edgeIdx);
    void      clearVertex(IndexType idx);
    void      removeVertex(IndexType idx);
    IndexType addVertex(char c); 
    IndexType addEdge(IndexType fromIdx, IndexType toIdx, IndexType edgeWeight);

    Edge getEdge(IndexType fromIdx, IndexType toIdx) 
    {
        std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
        if (!edgePair.second) 
        {
            PROJECT_NAMESPACE::WRN("%s: (%u, %u) not existing.", __PRETTY_FUNCTION__, fromIdx, toIdx); 
        }
        return edgePair.first;
    }
    Edge getEdge(IndexType fromIdx, IndexType toIdx) const 
    {
        std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
        if (!edgePair.second) 
        {
            PROJECT_NAMESPACE::WRN("%s: (%u, %u) not existing.", __PRETTY_FUNCTION__, fromIdx, toIdx); 
        }
        return edgePair.first;
    }

    const Vertex &          getVertex(IndexType vertexIdx) const                 { return _vertexArray.at(vertexIdx); }
    Vertex &                getVertex(IndexType vertexIdx)                       { return _vertexArray.at(vertexIdx); }

    /// Size
    IndexType               vertexSize() const                                   { return _vertexArray.size(); }
    IndexType               graphSize() const                                    { return boost::num_vertices(_graph); }

    // Updated default callback 
    struct vf2_print_callback {

        vf2_print_callback(graphType const& graph1, graphType &graph, IndexType cnt, std::vector<IndexType>& gateMap, std::vector<bool> &gateSet, std::vector<Vertex> &vArray,
                           std::vector<IndexType>& cktMap) : _graph1(graph1), _graph(graph), _cnt(cnt), _gateMap(gateMap), _gateSet(gateSet), 
                                                             _vertexArray(vArray), _vIndex(0), _cktMap(cktMap) {}

        template <typename CorrespondenceMap1To2,
                 typename CorrespondenceMap2To1>
                     bool operator()(CorrespondenceMap1To2 f, CorrespondenceMap2To1) {
                         // Print (sub)graph isomorphism map
                         BGL_FORALL_VERTICES_T(v, _graph1, graphType) 
                         {
                             _vIndex = boost::get(boost::vertex_index_t(), _graph, boost::get(f, v));
                             if (_gateSet[_cktMap[_vIndex]] == false) 
                             {
                                 _temp.emplace_back(_vIndex);
                             } 
                         }
                         _removed = false;
                         if (_temp.size() == _cnt)
                         {
                             for (IndexType i=0; i < _temp.size(); ++i)
                             {
                                 //boost::clear_vertex(_vertexArray.at(_temp[i]), _graph);
                                 for (IndexType j=i+1; j <_temp.size(); ++j)
                                 {
                                     _ePair = boost::edge(_vertexArray.at(_temp[i]), _vertexArray.at(_temp[j]), _graph);
                                     if (_ePair.second) 
                                     {
                                         boost::remove_edge(_ePair.first, _graph);
                                         _removed = true;
                                     }
                                 }
                                 if (_removed)
                                 {
                                     _gateMap.emplace_back(_temp[i]);
                                     _gateSet[_cktMap[_temp[i]]] = true;
                                 }
                             }
                         }
                         if (!_removed)
                         {
                            return true;
                         }
                         _temp.clear();
                         return true;
                     }
        private:
        graphType const& _graph1;
        graphType &_graph;
        IndexType const _cnt;
        std::vector<IndexType> &_gateMap, &_cktMap;
        std::vector<bool> & _gateSet;
        std::vector<Vertex> &_vertexArray;
        IndexType _vIndex;
        std::vector<IndexType> _temp;
        std::pair<Edge, bool> _ePair;
        bool _removed;
    };

#include "library/lib_gates.h"

    void clear()
    {
        _graph.clear();
        _vertexArray.clear();
        _edgeArray.clear();
        _newckt.clear();
        _gate.clear();
    }


private:
    Subckt              _newckt;
    IndexType           _cnt;
    graphType           _graph;
    std::string         _gate;
    std::vector<Edge>   _edgeArray;
    std::vector<Vertex> _vertexArray;
};

inline IndexType  GraphB::addVertex(char c) 
{
    IndexType idx = _vertexArray.size();
    _vertexArray.emplace_back(boost::add_vertex(vertex_property(c), _graph));
    return idx;
}

inline IndexType GraphB::addEdge(IndexType fromIdx, IndexType toIdx, IndexType edgeWeight) 
{
    std::pair<Edge, bool> returnedEdge = boost::add_edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), edgeWeight, _graph);
    if (!returnedEdge.second) 
    {
        PROJECT_NAMESPACE::WRN("%s: Failed to add edge (%u, %u), the edge has already existed and the graph is not allowing parallel edge \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
    }
    IndexType idx = _edgeArray.size();
    _edgeArray.emplace_back(returnedEdge.first);
    return idx;
}

inline void GraphB::removeEdge(IndexType fromIdx, IndexType toIdx) 
{
    std::pair<Edge, bool> edgePair = boost::edge(_vertexArray.at(fromIdx), _vertexArray.at(toIdx), _graph);
    if (!edgePair.second) 
    {
        PROJECT_NAMESPACE::WRN("%s: Failed to remove the return, (%u, %u) does not exist \n", __PRETTY_FUNCTION__, fromIdx, toIdx);
    }
    boost::remove_edge(edgePair.first, _graph);
}

inline void GraphB::removeEdge(IndexType edgeIdx) 
{
    boost::remove_edge(_edgeArray.at(edgeIdx), _graph);
}

inline void GraphB::clearVertex(IndexType idx) 
{
    boost::clear_vertex(_vertexArray.at(idx), _graph);
}

inline void GraphB::removeVertex(IndexType idx) 
{
    boost::remove_vertex(_vertexArray.at(idx), _graph);
}

inline GraphB::graphType GraphB::createGraph(Subckt& newckt, std::vector<IndexType> const& cktMap)
{
    IndexType vertexCount(0), edgeCount(0);

    //Add vertices based on device type
    for (IndexType i=0; i < cktMap.size(); ++i) 
    {
        if(newckt.instArray(cktMap[i]).type() == InstType::NFET)
        {
            addVertex('n');
            ++vertexCount;
        } else 
        {
            addVertex('p');
            ++vertexCount;
        }
    }

    IndexType weight[3][3] = {{8, 2, 8}, {2, 32, 2}, {8, 2, 8}}; //DCI Coding scheme

    //Add edges based on connectivity
    for (IndexType i=0; i < cktMap.size(); ++i) 
    {   
        for (IndexType j=i+1; j < cktMap.size(); ++j)
        {   
            IndexType connWeight(0);
            if (((newckt.instArray(cktMap[i]).type() == InstType::NFET) ||(newckt.instArray(cktMap[i]).type() == InstType::PFET)) && 
                ((newckt.instArray(cktMap[j]).type() == InstType::NFET) ||(newckt.instArray(cktMap[j]).type() == InstType::PFET))) 
            { //Only consider connection between MOS for now
                if (newckt.instArray(cktMap[i]).pinId(0) == newckt.instArray(cktMap[j]).pinId(0))
                {
                    connWeight += weight[0][0]; 
                }
                else if (newckt.instArray(cktMap[i]).pinId(0) == newckt.instArray(cktMap[j]).pinId(1))
                {
                    connWeight += weight[0][1]; 
                }
                else if (newckt.instArray(cktMap[i]).pinId(0) == newckt.instArray(cktMap[j]).pinId(2)) 
                {
                    connWeight += weight[0][2]; 
                }

                if (newckt.instArray(cktMap[i]).pinId(1) == newckt.instArray(cktMap[j]).pinId(0))
                {
                    connWeight += weight[1][0]; 
                }
                else if (newckt.instArray(cktMap[i]).pinId(1) == newckt.instArray(cktMap[j]).pinId(1)) 
                {
                    connWeight += weight[1][1]; 
                }
                else if (newckt.instArray(cktMap[i]).pinId(1) == newckt.instArray(cktMap[j]).pinId(2)) 
                {
                    connWeight += weight[1][2]; 
                }

                if (newckt.instArray(cktMap[i]).pinId(2) == newckt.instArray(cktMap[j]).pinId(0)) 
                {
                    connWeight += weight[2][0]; 
                }
                else if (newckt.instArray(cktMap[i]).pinId(2) == newckt.instArray(cktMap[j]).pinId(1)) 
                {
                    connWeight += weight[2][1]; 
                }
                else if (newckt.instArray(cktMap[i]).pinId(2) == newckt.instArray(cktMap[j]).pinId(2))
                {
                    connWeight += weight[2][2]; 
                }

                if (((newckt.instArray(cktMap[i]).pinId(2) == newckt.instArray(cktMap[i]).pinId(3)) || 
                     (newckt.instArray(cktMap[i]).pinId(0) == newckt.instArray(cktMap[i]).pinId(3))) && 
                    ((newckt.instArray(cktMap[j]).pinId(2) == newckt.instArray(cktMap[j]).pinId(3)) || 
                     (newckt.instArray(cktMap[j]).pinId(0) == newckt.instArray(cktMap[j]).pinId(3)))) 
                {
                        connWeight += 1; //Include if S-B is tied off for both Mi and Mj 
                }
                if (connWeight > 1) //Include only if direct connections exist between the FETs - Ignore if only S-B connections match!
                {
                    addEdge(i, j, connWeight); 
                    ++edgeCount;
                }
            } else
            {
                Assert(!(newckt.instArray(cktMap[i]).type() == InstType::COMPONENT) && !(newckt.instArray(cktMap[j]).type() == InstType::COMPONENT)); 
            }
        }
    }

    return _graph;
}

inline void GraphB::createGraphLib(Subckt& newckt, std::ofstream& libGatec)
{
    //Add vertices based on device type
    for (IndexType i=0; i < newckt.numInsts(); ++i) 
    {
        if(newckt.instArray(i).type() == InstType::NFET)
        {
            libGatec << "\n\tIndexType vidx" << i << " = graph_1.addVertex('n');";
        } else 
        {
            libGatec << "\n\tIndexType vidx" << i << " = graph_1.addVertex('p');";
        }
    }

    IndexType weight[3][3] = {{8, 2, 8}, {2, 32, 2}, {8, 2, 8}}; //DCI Coding scheme

    //Add edges based on connectivity
    for (IndexType i=0; i < newckt.numInsts(); ++i) 
    {   
        for (IndexType j=i+1; j < newckt.numInsts(); ++j)
        {   
            IndexType connWeight(0);
            if (((newckt.instArray(i).type() == InstType::NFET) ||(newckt.instArray(i).type() == InstType::PFET)) && 
                ((newckt.instArray(j).type() == InstType::NFET) ||(newckt.instArray(j).type() == InstType::PFET))) 
            { //Only consider connection between MOS for now
                if (newckt.instArray(i).pinId(0) == newckt.instArray(j).pinId(0))
                {
                    connWeight += weight[0][0]; 
                }
                else if (newckt.instArray(i).pinId(0) == newckt.instArray(j).pinId(1))
                {
                    connWeight += weight[0][1]; 
                }
                else if (newckt.instArray(i).pinId(0) == newckt.instArray(j).pinId(2)) 
                {
                    connWeight += weight[0][2]; 
                }

                if (newckt.instArray(i).pinId(1) == newckt.instArray(j).pinId(0))
                {
                    connWeight += weight[1][0]; 
                }
                else if (newckt.instArray(i).pinId(1) == newckt.instArray(j).pinId(1)) 
                {
                    connWeight += weight[1][1]; 
                }
                else if (newckt.instArray(i).pinId(1) == newckt.instArray(j).pinId(2)) 
                {
                    connWeight += weight[1][2]; 
                }

                if (newckt.instArray(i).pinId(2) == newckt.instArray(j).pinId(0)) 
                {
                    connWeight += weight[2][0]; 
                }
                else if (newckt.instArray(i).pinId(2) == newckt.instArray(j).pinId(1)) 
                {
                    connWeight += weight[2][1]; 
                }
                else if (newckt.instArray(i).pinId(2) == newckt.instArray(j).pinId(2))
                {
                    connWeight += weight[2][2]; 
                }

                if (((newckt.instArray(i).pinId(2) == newckt.instArray(i).pinId(3)) || (newckt.instArray(i).pinId(0) == newckt.instArray(i).pinId(3))) && 
                    ((newckt.instArray(j).pinId(2) == newckt.instArray(j).pinId(3)) || (newckt.instArray(j).pinId(0) == newckt.instArray(j).pinId(3)))) 
                {
                    connWeight += 1; //Include if S-B is tied off for both Mi and Mj 
                }
                if (connWeight > 1) //Include only if direct connections exist between the FETs - Ignore if only S-B connections match!
                {
                    libGatec << "\n\tgraph_1.addEdge(vidx" << i << ", vidx" << j << ", " << connWeight << ");";
                }
            } 
        }
    }
}

#include "library/lib_gates.cpp"

PROJECT_NAMESPACE_END
#endif ///VF2_SUBGRAPH_ISO_H_
