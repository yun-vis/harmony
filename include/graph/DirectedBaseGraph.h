#ifndef _DirectedBaseGraph_H
#define _DirectedBaseGraph_H


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

// force-directed layout
#include <boost/graph/fruchterman_reingold.hpp>
//#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/connected_components.hpp>

using namespace boost;

#include "base/Coord2.h"
#include "graph/BaseGraphProperty.h"
#include "graph/BaseVertexProperty.h"
#include "graph/BaseEdgeProperty.h"


typedef adjacency_list< listS, listS, bidirectionalS,
		BaseVertexProperty, BaseEdgeProperty, BaseGraphProperty > DirectedBaseGraph;

// point map
typedef boost::rectangle_topology<> topologyType;
typedef topologyType::point_type pointType;
typedef vector< pointType > PositionVec;
typedef iterator_property_map< PositionVec::iterator,
		property_map< DirectedBaseGraph, unsigned int BaseVertexProperty::* >::type > PositionMap;

typedef boost::convex_topology< 2 >::point_difference PointDifference;
typedef vector< PointDifference > DifferenceVec;
typedef iterator_property_map< DifferenceVec::iterator,
		property_map< DirectedBaseGraph, unsigned int BaseVertexProperty::* >::type > DifferenceMap;
typedef vector< PointDifference > DifferenceVec;

//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------
void randomGraphLayout( DirectedBaseGraph &graph, double width, double height );

void fruchtermanGraphLayout( DirectedBaseGraph &graph, double width, double height );

void printGraph( const DirectedBaseGraph &g );

void clearGraph( DirectedBaseGraph &g );

#endif  // _DirectedBaseGraph_H
