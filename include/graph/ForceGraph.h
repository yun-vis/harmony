#ifndef _ForceGraph_H
#define _ForceGraph_H


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
#include "graph/ForceGraphProperty.h"
#include "graph/ForceVertexProperty.h"
#include "graph/ForceEdgeProperty.h"


typedef adjacency_list< listS, listS, undirectedS,
		ForceVertexProperty, ForceEdgeProperty,
		ForceGraphProperty > ForceGraph;

//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------

void printGraph( const ForceGraph &g );

void clearGraph( ForceGraph &g );

#endif  // _ForceGraph_H
