//==============================================================================
// BaseGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/ForceGraph.h"


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
//	Customized ForceGraph Functions
//------------------------------------------------------------------------------
//
//  ForceGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void printGraph( ForceGraph & graph )
{
    cerr << "num_vertices = " << num_vertices( graph ) << endl;
    cerr << "num_edges = " << num_edges( graph ) << endl;

//#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, ForceGraph ) {

        ForceGraph::degree_size_type      degrees         = out_degree( vd, graph );
		cerr << " vid = " << graph[vd].id
			 //<< " ( " << *graph[vd].namePtr << " ) "
			 << " w = " << graph[vd].weight << endl;
    }
//#endif  // DEBUG

//#ifdef  DEBUG
    // print edge information
    BGL_FORALL_EDGES( ed, graph, ForceGraph ) {

        ForceGraph::vertex_descriptor vdS = source( ed, graph );
        ForceGraph::vertex_descriptor vdT = target( ed, graph );

		cerr << " eid = " << graph[ed].id
			 << " ( " << graph[vdS].id
			 << ", " << graph[vdT].id << " )"
			 //<< " ( " << *graph[vdS].namePtr
			 //<< ", " << *graph[vdT].namePtr << " )"
			 << " w = " << graph[ed].weight
			 << endl;
	}
//#endif  // DEBUG
}

//
//  ForceGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( ForceGraph & graph )
{
    // clear edges
	ForceGraph::edge_iterator ei, ei_end, e_next;
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
    pair< ForceGraph::vertex_iterator, ForceGraph::vertex_iterator > vp;
    for ( vp = vertices( graph ); vp.first != vp.second;  ) {
		ForceGraph::vertex_descriptor vd = (*vp.first);
		++vp.first;
		clear_vertex( vd, graph );
		remove_vertex( vd, graph );
    }
}
