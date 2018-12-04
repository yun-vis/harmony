#ifndef _UndirectedPropertyGraph_H
#define _UndirectedPropertyGraph_H


#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graphml.hpp>

using namespace boost;


//typedef adjacency_list< vecS, vecS, directedS,
//		property< vertex_index_t, int >,
//		property< edge_weight_t, int > > DirectedPropertyGraph;

// vertex attributes
enum vertex_myx_t                   { vertex_myx };             // current 2D coordinates
enum vertex_myy_t                   { vertex_myy };             // current 2D coordinates

namespace boost {
	// vertex properties
	BOOST_INSTALL_PROPERTY( vertex, myx );
	BOOST_INSTALL_PROPERTY( vertex, myy );
	// edge properties
}

typedef property< vertex_myx_t, double >                        				MyVX;
typedef property< vertex_myy_t, double, MyVX >                     				MyVY;
typedef property< vertex_index_t, unsigned int, MyVY > 		                    MyVertexProperty;

typedef property< edge_weight_t, double >                            			MyEWeight;
typedef property< edge_index_t, unsigned int, MyEWeight >                       MyEdgeProperty;

typedef adjacency_list< listS, vecS, undirectedS,
		MyVertexProperty, MyEdgeProperty > UndirectedPropertyGraph;

// define the storage type for the planar embedding
typedef vector< std::vector< graph_traits< UndirectedPropertyGraph >::edge_descriptor > > embedding_storage_t;
typedef boost::iterator_property_map < embedding_storage_t::iterator, property_map< UndirectedPropertyGraph, vertex_index_t >::type > embedding_t;

//------------------------------------------------------------------------------
//	Customized vertex maps
//------------------------------------------------------------------------------

// current 2D coordinates
typedef property_map< UndirectedPropertyGraph, vertex_myx_t >::type           	VertexXMap;
typedef property_map< UndirectedPropertyGraph, vertex_myy_t >::type           	VertexYMap;
typedef property_map< UndirectedPropertyGraph, vertex_index_t >::type           VertexIndexMap;


typedef property_map< UndirectedPropertyGraph, edge_weight_t >::type            EdgeWeightMap;
typedef property_map< UndirectedPropertyGraph, edge_index_t >::type             EdgeIndexMap;

void printGraph( UndirectedPropertyGraph & graph );
void clearGraph( UndirectedPropertyGraph & graph );

#endif  // _UndirectedPropertyGraph_H
