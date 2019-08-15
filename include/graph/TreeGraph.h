#ifndef _TreeGraph_H
#define _TreeGraph_H


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

// Tree-directed layout
#include <boost/graph/fruchterman_reingold.hpp>
//#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/connected_components.hpp>

using namespace boost;

#include "base/Coord2.h"
#include "graph/TreeGraphProperty.h"
#include "graph/TreeVertexProperty.h"
#include "graph/TreeEdgeProperty.h"


typedef adjacency_list< listS, listS, undirectedS,
		TreeVertexProperty, TreeEdgeProperty,
		TreeGraphProperty >  TreeGraph;

//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------

void printGraph( const TreeGraph & g );
void clearGraph( TreeGraph & g );

#endif  // _TreeGraph_H
