//==============================================================================
// BaseGraph.cpp
//      : program file for graph function
//
//==============================================================================

#include "graph/SkeletonGraph.h"
#include "optimization/Layout.h"

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
//  SkeletonGraph::radialPlacement -- radial placement
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void radialPlacement( SkeletonGraph & graph )
{
    Layout layout;

    layout.init( &graph );
	layout.radialPlacement();
}


//
//  SkeletonGraph::geodesicAssignment -- geodesic assignment
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
void geodesicAssignment( SkeletonGraph & graph )
{
//------------------------------------------------------------------------------
//      copy the bundled graph
//------------------------------------------------------------------------------
	UndirectedPropertyGraph propG;
	VertexIndexMap  vertexIndex = get( vertex_index, propG );
	EdgeIndexMap    edgeIndex   = get( edge_index, propG );

	BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {

		UndirectedPropertyGraph::vertex_descriptor vdNew = add_vertex( propG );
		//vertexIndex[ vdNew ] = graph[ vd ].id;
		//put( vertex_index, propG, vdNew, graph[ vd ].id );
	}
	BGL_FORALL_EDGES( ed, graph, SkeletonGraph ) {

		SkeletonGraph::vertex_descriptor vdS = source( ed, graph );
		SkeletonGraph::vertex_descriptor vdT = target( ed, graph );
		UndirectedPropertyGraph::vertex_descriptor src = vertex( graph[ vdS ].id, propG );
		UndirectedPropertyGraph::vertex_descriptor tar = vertex( graph[ vdT ].id, propG );

		pair< UndirectedPropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( src, tar, propG );
		UndirectedPropertyGraph::edge_descriptor foreED = foreE.first;
		edgeIndex[ foreED ] = graph[ ed ].id;
	}

#ifdef DEBUG
	BGL_FORALL_EDGES( ed, propG, UndirectedPropertyGraph ) {

        UndirectedPropertyGraph::vertex_descriptor vdS = source( ed, propG );
        UndirectedPropertyGraph::vertex_descriptor vdT = target( ed, propG );
        cerr << edgeIndex[ ed ] << " = " << vertexIndex[ vdS ] << " x " << vertexIndex[ vdT ] << endl;
    }
#endif // DEBUG

//------------------------------------------------------------------------------
//      Initialization
//------------------------------------------------------------------------------

	vector< double > unitWeight( num_edges( propG ) );
	BGL_FORALL_EDGES( ed, graph, SkeletonGraph ) {
		SkeletonGraph::vertex_descriptor vdS = source( ed, graph );
		SkeletonGraph::vertex_descriptor vdT = target( ed, graph );
		//unitWeight[ graph[ ed ].id ] = 1.0;
		unitWeight[ graph[ ed ].id ] = *graph[ vdS ].widthPtr + *graph[ vdT ].widthPtr;
	}
	BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {
		graph[ vd ].geodesicDist = 0.0;
	}

//------------------------------------------------------------------------------
//      Calculate vertex geodesic distances
//------------------------------------------------------------------------------
	BGL_FORALL_VERTICES( vdO, propG, UndirectedPropertyGraph ) {

			if( out_degree( vdO, propG ) != 0 ) {

				//UndirectedPropertyGraph::vertex_descriptor vdO = vertex( 0, propG );
				vector< double > distance( num_vertices( propG ) );
				dijkstra_shortest_paths( propG, vdO,
										 weight_map( make_iterator_property_map( unitWeight.begin(),
																				 get( edge_index, propG ) ) ).
												 distance_map( make_iterator_property_map( distance.begin(),
																						   get( vertex_index, propG ) ) ) );

				BGL_FORALL_VERTICES( vdI, graph, SkeletonGraph ) {
					//unsigned int idI = vertexID[ vdI ];
					double oldV = graph[ vdI ].geodesicDist;
					if( out_degree( vdI, graph ) == 0 ) distance[ graph[ vdI ].id ] = 0.0;
					double newV = oldV + distance[ graph[ vdI ].id ];
					graph[ vdI ].geodesicDist = newV;
				}

#ifdef DEBUG
				cerr << " ***  Handling Vertex ID " << vertexIndex[ vdO ] << endl;
            BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {
                cerr << "Vertex[ " << setw( 3 ) << graph[ vd ].id << " ] : intermediate geodesic = "
                     << graph[ vd ].geodesicDist << endl;
            }
#endif  // DEBUG
			}
		}

#ifdef DEBUG
	BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {
        cerr << "Vertex[ " << setw( 3 ) << graph[ vd ].id << " ] : final geodesic = "
             << graph[ vd ].geodesicDist << endl;
    }
#endif  // DEBUG
}

//
//  SkeletonGraph::zoneAssignment -- zone assignment
//
//  Inputs
//  g   : object of Graph
//
//  Outputs
//  none
//
unsigned int zoneAssignment( SkeletonGraph & graph )
{
//------------------------------------------------------------------------------
//      set the geodesic center
//------------------------------------------------------------------------------

	SkeletonGraph::vertex_descriptor vdC = vertex( 0, graph );
	double minDist = INFINITY;
	BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {
			if( num_vertices( graph ) == 1 ){
				vdC = vd;
				minDist = graph[ vd ].geodesicDist;
			}
			else if ( graph[ vd ].geodesicDist < minDist && out_degree( vd, graph ) > 0 ){
				vdC = vd;
				minDist = graph[ vd ].geodesicDist;
			}
			graph[ vd ].flag = false;
#ifdef DEBUG
			cerr << "vd = " << graph[ vd ].id << " deg = " << out_degree( vd, graph )
             << " geoDist = " << graph[ vd ].geodesicDist << endl;
#endif // DEBUG
		}

#ifdef DEBUG
	cerr << "num_vertices( graph ) = " << num_vertices( graph ) << endl;
    cerr << "vdC = " << graph[ vdC ].id << " geodesicDist = " << graph[ vdC ].geodesicDist << endl;
#endif // DEBUG

	list< SkeletonGraph::vertex_descriptor > topoDist;
	topoDist.push_back( vdC );
	graph[ vdC ].zone = 0;
	graph[ vdC ].flag = true;

	// assign topological distance from the center
	unsigned int maxZone = 0, maxEZone = 0;
	while( topoDist.size() != 0 ){

		SkeletonGraph::vertex_descriptor vdO = *topoDist.begin();
		topoDist.pop_front();

		SkeletonGraph::out_edge_iterator eo, eo_end;
		for( tie( eo, eo_end ) = out_edges( vdO, graph ); eo != eo_end; ++eo ) {

			SkeletonGraph::vertex_descriptor vdT = target( *eo, graph );
			if( graph[ vdT ].flag == false ){
#ifdef DEBUG
				cerr << " graph[ vdO ].width = " << graph[ vdO ].width << " graph[ vdO ].height = " << graph[ vdO ].height << endl
                     << " graph[ vdT ].width = " << graph[ vdT ].width << " graph[ vdT ].height = " << graph[ vdT ].height << endl;
#endif // DEBUG
				graph[ vdT ].zone = graph[ vdO ].zone + 1;
				graph[ vdT ].ezone = graph[ vdO ].ezone + *graph[ vdO ].widthPtr + *graph[ vdT ].widthPtr;
				maxZone = MAX2( maxZone, graph[ vdT ].zone +1 );
				maxEZone = MAX2( maxEZone, graph[ vdT ].ezone+1 );
				topoDist.push_back( vdT );
				graph[ vdT ].flag = true;
			}
		}
	}

	double interval = ( maxEZone - graph[ vdC ].ezone )/(double)(maxZone+1);
#ifdef DEBUG
	cerr << "minZone = " << graph[ vdC ].zone << endl;
    cerr << "maxZone = " << maxZone << endl;
    cerr << "interval = " << interval << endl;
#endif // DEBUG

	// assign zone to the non-connected node
	BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {
			if( graph[ vd ].flag == false ) {
				graph[ vd ].zone = maxZone;
				graph[ vd ].ezone = maxEZone;
			}
			graph[ vd ].ezone = ceil( (int)graph[ vd ].ezone/interval );
#ifdef DEBUG
			cerr << " zone( " << graph[ vd ].id << " ) = " << graph[ vd ].zone
             << ", ezone( " << graph[ vd ].id << " ) = " << graph[ vd ].ezone << endl;
#endif // DEBUG
		}
	cerr << "Calculated geodesic zones at vertices : Maximum zone = " << maxZone+1 << endl;

	return maxZone+1;
}


//------------------------------------------------------------------------------
//	Customized SkeletonGraph Functions
//------------------------------------------------------------------------------
//
//  SkeletonGraph::printGraph -- print the graph.
//
//  Inputs
//  g   : object of Grpah
//
//  Outputs
//  none
//
void printGraph( SkeletonGraph & graph )
{
	cerr << " SkeletonGraph( G ).nVertices = " << num_vertices( graph ) << endl;
	cerr << " SkeletonGraph( G ).nEdges = " << num_edges( graph ) << endl;

//#ifdef  DEBUG
    // print vertex information
    BGL_FORALL_VERTICES( vd, graph, SkeletonGraph ) {

        SkeletonGraph::degree_size_type      degrees         = out_degree( vd, graph );

        cerr << "vid = " << graph[vd].id << " coord = " << *graph[vd].coordPtr;
    }
//#endif  // DEBUG

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
//  g   : object of Grpah
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
