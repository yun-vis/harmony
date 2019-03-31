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
enum vertex_mypos_t               	{ vertex_mypos };         	// position
enum vertex_mylabel_t               { vertex_mylabel };         // label
enum vertex_mycolor_t               { vertex_mycolor };         // color

namespace boost {
	// vertex properties
	BOOST_INSTALL_PROPERTY( vertex, myx );
	BOOST_INSTALL_PROPERTY( vertex, myy );
	BOOST_INSTALL_PROPERTY( vertex, mypos );
    BOOST_INSTALL_PROPERTY( vertex, mylabel );
    BOOST_INSTALL_PROPERTY( vertex, mycolor );
	// edge properties
}

typedef property< vertex_myx_t, double >                        				MyVX;
typedef property< vertex_myy_t, double, MyVX >                        		    MyVY;
typedef property< vertex_mypos_t, string, MyVY >                     			MyVPos;
typedef property< vertex_mylabel_t, string, MyVPos >                     		MyVLabel;
typedef property< vertex_mycolor_t, string, MyVLabel >                  		MyVColor;
typedef property< vertex_index_t, unsigned int, MyVColor > 	                    MyVertexProperty;

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
typedef property_map< UndirectedPropertyGraph, vertex_mypos_t >::type        	VertexPosMap;
typedef property_map< UndirectedPropertyGraph, vertex_mylabel_t >::type        	VertexLabelMap;
typedef property_map< UndirectedPropertyGraph, vertex_mycolor_t >::type        	VertexColorMap;
typedef property_map< UndirectedPropertyGraph, vertex_index_t >::type           VertexIndexMap;

typedef property_map< UndirectedPropertyGraph, edge_weight_t >::type            EdgeWeightMap;
typedef property_map< UndirectedPropertyGraph, edge_index_t >::type             EdgeIndexMap;

void printGraph( UndirectedPropertyGraph & graph );
void clearGraph( UndirectedPropertyGraph & graph );

#endif  // _UndirectedPropertyGraph_H
