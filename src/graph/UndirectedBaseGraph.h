#ifndef _UndirectedBaseGraph_H
#define _UndirectedBaseGraph_H


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

#include "core/Coord2.h"
#include "graph/BaseGraphProperty.h"
#include "graph/BaseVertexProperty.h"
#include "graph/BaseEdgeProperty.h"


typedef adjacency_list< listS, listS, undirectedS,
		BaseVertexProperty, BaseEdgeProperty,
		BaseGraphProperty > UndirectedBaseGraph;

//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------

void printGraph( const UndirectedBaseGraph &g );

void clearGraph( UndirectedBaseGraph &g );

#endif  // _UndirectedBaseGraph_H
