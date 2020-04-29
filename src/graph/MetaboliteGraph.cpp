//==============================================================================
// MetaboliteGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/MetaboliteGraph.h"

//------------------------------------------------------------------------------
//	Customized Vertex Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Edge Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Customized Layout Functions
//------------------------------------------------------------------------------
//
//  UndirectedBaseGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void printGraph( const MetaboliteGraph &graph ) {
	cerr << "num_vertices = " << num_vertices( graph ) << endl;
	cerr << "num_edges = " << num_edges( graph ) << endl;

//#ifdef  DEBUG
	// print vertex information
	BGL_FORALL_VERTICES( vd, graph, MetaboliteGraph ) {
			
			cerr << " vid = " << graph[ vd ].id
			     << " ( " << *graph[ vd ].namePtr << " ) "
			     << " isAlias = " << graph[ vd ].isAlias << endl;

/*
        cerr << " vid = " << graph[vd].id
             << " gID = " << graph[vd].groupID
             << " initID = " << graph[vd].initID
             << " x = " << graph[vd].coordPtr->x()
             << " y = " << graph[vd].coordPtr->y()
             << " w = " << *graph[vd].widthPtr
             << " h = " << *graph[vd].heightPtr
             << " namePixelWidth = " << *graph[vd].namePixelWidthPtr
             << " namePixelHeight = " << *graph[vd].namePixelHeightPtr
             << endl;
*/
		}
	cerr << endl;
//#endif  // DEBUG

//#ifdef  DEBUG
	// print edge information
	BGL_FORALL_EDGES( ed, graph, MetaboliteGraph ) {
			
			MetaboliteGraph::vertex_descriptor vdS = source( ed, graph );
			MetaboliteGraph::vertex_descriptor vdT = target( ed, graph );
			
			cerr << " eid = " << graph[ ed ].id
			     << " ( " << graph[ vdS ].id
			     << ", " << graph[ vdT ].id << " )"
			     << " ( " << *graph[ vdS ].namePtr
			     << ", " << *graph[ vdT ].namePtr << " )"
			     << " w = " << graph[ ed ].weight
			     << endl;
		}
	cerr << endl;
//#endif  // DEBUG
}

//
//  MetaboliteGraph::clearGraph -- clear the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void clearGraph( MetaboliteGraph &graph ) {
	// clear edges
	MetaboliteGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( graph );
	for( e_next = ei; ei != ei_end; ei = e_next ) {
		e_next++;
		remove_edge( *ei, graph );
	}
#ifdef  SKIP
	BGL_FORALL_EDGES( edge, graph, MetaboliteGraph )
	{
		remove_edge( edge, graph );
	}
#endif  //SKIP
	
	// clear vertices
	pair< MetaboliteGraph::vertex_iterator, MetaboliteGraph::vertex_iterator > vp;
	for( vp = vertices( graph ); vp.first != vp.second; ) {
		MetaboliteGraph::vertex_descriptor vd = ( *vp.first );
		++vp.first;
		clear_vertex( vd, graph );
		remove_vertex( vd, graph );
	}
}
