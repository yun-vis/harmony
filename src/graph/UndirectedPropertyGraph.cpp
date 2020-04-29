//==============================================================================
// BaseGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

#include "graph/UndirectedPropertyGraph.h"


//------------------------------------------------------------------------------
//	Customized Vertex Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Edge Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Layout Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Customized UndirectedPropertyGraph Functions
//------------------------------------------------------------------------------
//
//  UndirectedPropertyGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void printGraph( UndirectedPropertyGraph &graph ) {
	cerr << "num_vertices = " << num_vertices( graph ) << endl;
	cerr << "num_edges = " << num_edges( graph ) << endl;

#ifdef  DEBUG
	// print vertex information
	BGL_FORALL_VERTICES( vd, graph, UndirectedPropertyGraph ) {

		UndirectedPropertyGraph::degree_size_type      degrees         = out_degree( vd, graph );
	}
#endif  // DEBUG

#ifdef  DEBUG
	// print edge information
	BGL_FORALL_EDGES( ed, graph, UndirectedPropertyGraph ) {

		UndirectedPropertyGraph::vertex_descriptor vdS = source( ed, graph );
		UndirectedPropertyGraph::vertex_descriptor vdT = target( ed, graph );

		cerr << "eid = " << graph[ ed ].id << " ( " << graph[ vdS ].id << " == " << graph[ vdT ].id << " ) " << endl;
	}
#endif  // DEBUG
}

//
//  UndirectedPropertyGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( UndirectedPropertyGraph &graph ) {
	// clear edges
	UndirectedPropertyGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( graph );
	for( e_next = ei; ei != ei_end; ei = e_next ) {
		e_next++;
		remove_edge( *ei, graph );
	}

#ifdef  SKIP
	BGL_FORALL_EDGES( edge, graph, BaseGraph )
	{
		remove_edge( edge, graph );
	}
#endif  // SKIP

	// clear vertices
	pair< UndirectedPropertyGraph::vertex_iterator, UndirectedPropertyGraph::vertex_iterator > vp;
	for( vp = vertices( graph ); vp.first != vp.second; ) {
		UndirectedPropertyGraph::vertex_descriptor vd = ( *vp.first );
		++vp.first;
		clear_vertex( vd, graph );
		remove_vertex( vd, graph );
	}
}
