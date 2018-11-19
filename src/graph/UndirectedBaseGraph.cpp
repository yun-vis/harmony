//==============================================================================
// BaseGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/UndirectedBaseGraph.h"


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
//	Customized UndirectedBaseGraph Functions
//------------------------------------------------------------------------------
//
//  UndirectedBaseGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void printGraph( UndirectedBaseGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, UndirectedBaseGraph ) {

        UndirectedBaseGraph::degree_size_type      degrees         = out_degree( vd, graph );
    }
#endif  // DEBUG

#ifdef  DEBUG
    // print edge information
    BGL_FORALL_EDGES( ed, graph, UndirectedBaseGraph ) {

        UndirectedBaseGraph::vertex_descriptor vdS = source( ed, graph );
        UndirectedBaseGraph::vertex_descriptor vdT = target( ed, graph );

        cerr << "eid = " << graph[ ed ].id << " ( " << graph[ vdS ].id << " == " << graph[ vdT ].id << " ) " << endl;
    }
#endif  // DEBUG
}

//
//  UndirectedBaseGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( UndirectedBaseGraph & graph )
{
    // clear edges
	UndirectedBaseGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( graph );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
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
    pair< UndirectedBaseGraph::vertex_iterator, UndirectedBaseGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
		UndirectedBaseGraph::vertex_descriptor vd = (*vp.first);
		++vp.first;
		clear_vertex( vd, graph );
		remove_vertex( vd, graph );
    }
}
