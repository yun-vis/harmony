#ifndef _MetaboliteGraph_H
#define _MetaboliteGraph_H

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
//#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>

// force-directed layout
#include <boost/graph/fruchterman_reingold.hpp>
//#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;

#include "graph/BaseGraphProperty.h"
#include "graph/MetaboliteVertexProperty.h"
#include "graph/MetaboliteEdgeProperty.h"


typedef adjacency_list< listS, listS, bidirectionalS,
		MetaboliteVertexProperty, MetaboliteEdgeProperty, BaseGraphProperty > MetaboliteGraph;

void printGraph( const MetaboliteGraph &graph );

void clearGraph( MetaboliteGraph &graph );

#endif  // _MetaboliteGraph_H
