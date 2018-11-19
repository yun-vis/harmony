//==============================================================================
// BaseGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "base/SkeletonGraph.h"


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
//	Customized SkeletonGraph Functions
//------------------------------------------------------------------------------
//
//  SkeletonGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void printGraph( SkeletonGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {

        SkeletonGraph::degree_size_type      degrees         = out_degree( vd, graph );
    }
#endif  // DEBUG

#ifdef  DEBUG
    // print edge information
    BGL_FORALL_EDGES( ed, graph, SkeletonGraph ) {

        SkeletonGraph::vertex_descriptor vdS = source( ed, graph );
        SkeletonGraph::vertex_descriptor vdT = target( ed, graph );

        cerr << "eid = " << graph[ ed ].id << " ( " << graph[ vdS ].id << " == " << graph[ vdT ].id << " ) " << endl;
    }
#endif  // DEBUG
}

//
//  SkeletonGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void clearGraph( SkeletonGraph & graph )
{
    // clear edges
	SkeletonGraph::edge_iterator ei, ei_end, e_next;
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
    pair< SkeletonGraph::vertex_iterator, SkeletonGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
		SkeletonGraph::vertex_descriptor vd = (*vp.first);
		++vp.first;
		clear_vertex( vd, graph );
		remove_vertex( vd, graph );
    }
}
