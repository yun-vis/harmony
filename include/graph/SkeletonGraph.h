#ifndef _SkeletonGraph_H
#define _SkeletonGraph_H


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
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

//#include "base/Common.h"
#include "base/Coord2.h"
#include "graph/SkeletonGraphProperty.h"
#include "graph/SkeletonVertexProperty.h"
#include "graph/SkeletonEdgeProperty.h"

//#include "graph/UndirectedBaseGraph.h"
#include "graph/UndirectedPropertyGraph.h"


typedef adjacency_list< vecS, listS, undirectedS,
		SkeletonVertexProperty, SkeletonEdgeProperty,
		SkeletonGraphProperty >  SkeletonGraph;

typedef pair< SkeletonGraph::vertex_descriptor, SkeletonGraph::vertex_descriptor >	SkeletonGraphVVPair;

//------------------------------------------------------------------------------
//	Customized BaseGraph Functions
//------------------------------------------------------------------------------
void geodesicAssignment( SkeletonGraph & graph );
unsigned int zoneAssignment( SkeletonGraph & graph );
void radialPlacement( SkeletonGraph & graph );

void printGraph( SkeletonGraph & graph );
void clearGraph( SkeletonGraph & graph );

#endif  // _SkeletonGraph_H
