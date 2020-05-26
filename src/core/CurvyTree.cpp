//******************************************************************************
// CurvyTree.cc
//	: program file for 2D CurvyTree coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:15:32 2011
//
//******************************************************************************

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <algorithm>

using namespace std;

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "core/CurvyTree.h"

//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  CurvyTree::init -- initialzation
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void CurvyTree::_init( void ) {
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Pathway::Pathway -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
CurvyTree::CurvyTree() {
	_init();
}

//
//  Pathway::Pathway -- copy constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
CurvyTree::CurvyTree( const CurvyTree &v ) {
	_tree = v._tree;
	_paths = v._paths;
}

//
//  Pathway::Pathway -- destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
CurvyTree::~CurvyTree() {
}

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
bool CurvyTree::_isKeyOnPath( vector< UndirectedBaseGraph::vertex_descriptor > &pathVec,
                              vector< UndirectedBaseGraph::vertex_descriptor > &keyVec ) {
	bool isOn = false;
	
	for( unsigned int i = 1; i < pathVec.size() - 1; i++ ) {
		
		for( unsigned int j = 0; j < keyVec.size(); j++ ) {
			
			if( pathVec[ i ] == keyVec[ j ] ) isOn = true;
		}
	}
	
	return isOn;
}


void CurvyTree::_pathPartition( void ) {
	vector< UndirectedBaseGraph::vertex_descriptor > keyVec;
	
	// initialization
	_paths.clear();
	
	// find key points (non-degree-2 vertices)
	BGL_FORALL_VERTICES( vd, _tree, UndirectedBaseGraph ) {
			
			UndirectedBaseGraph::degree_size_type degrees = out_degree( vd, _tree );
			if( degrees != 2 ) keyVec.push_back( vd );
		}

#ifdef DEBUG
	cerr << "keyVec: ";
	for( unsigned int i = 0; i < keyVec.size(); i++ ){
		cerr << _tree[ keyVec[i] ].id << ", ";
	}
	cerr << endl;
#endif // DEBUG
	
	// find paths
	for( unsigned int i = 0; i < keyVec.size(); i++ ) {
		
		// compute the single-source shortest paths
		UndirectedBaseGraph::vertex_descriptor vdS = keyVec[ i ];
		vector< double > distances( num_vertices( _tree ) );
		vector< UndirectedBaseGraph::vertex_descriptor > predecessor( num_vertices( _tree ) );
		dijkstra_shortest_paths( _tree, vdS,
		                         vertex_index_map( get( &BaseVertexProperty::id, _tree ) )
				                         .predecessor_map( make_iterator_property_map( predecessor.begin(),
				                                                                       get( &BaseVertexProperty::id,
				                                                                            _tree ) ) )
				                         .distance_map( make_iterator_property_map( distances.begin(),
				                                                                    get( &BaseVertexProperty::id,
				                                                                         _tree ) ) )
				                         .weight_map( get( &BaseEdgeProperty::weight, _tree ) )
		);
		
		// retrieve the shortest path to the target
		for( unsigned int j = i + 1; j < keyVec.size(); j++ ) {
			
			vector< UndirectedBaseGraph::vertex_descriptor > pathVec;
			
			// cerr << "i = " << i << " j = " << j << endl;
			UndirectedBaseGraph::vertex_descriptor vdT = keyVec[ j ];
			while( true ) {
				
				if( vdT == vdS ) {
					break;
				}
				else {
					pathVec.push_back( vdT );
					vdT = predecessor[ _tree[ vdT ].id ];
				}
			}
			pathVec.push_back( vdT );

#ifdef DEBUG
			cerr << "path: ";
			for( unsigned int i = 0; i < pathVec.size(); i++ ){
				cerr << _tree[ pathVec[i] ].id << ", ";
			}
			cerr << endl;
#endif // DEBUG
			
			// add the path
			if( !_isKeyOnPath( pathVec, keyVec ) ) {
				
				Line2 path;
				for( unsigned int k = 0; k < pathVec.size(); k++ ) {
					path.addSample( *_tree[ pathVec[ k ] ].coordPtr );
				}
				_paths.push_back( path );
			}
		}
	}
}

void CurvyTree::computeFineCurve( int num, double unit ) {
	// initialization
	_pathPartition();
	
	// compute curve
	for( unsigned int i = 0; i < _paths.size(); i++ ) {
		
		_paths[ i ].computeChaikinCurve( num, unit );
	}
}


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	s	: reference to output stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to output stream
//
ostream &operator<<( ostream &stream, const CurvyTree &obj ) {
	// set the output formatting
	stream << setiosflags( ios::showpoint );
	stream << setprecision( 8 );
	
	int width = 8;
	
	// print tree
	printGraph( obj.tree() );
	
	// print out the elements
	for( unsigned int i = 0; i < obj.paths().size(); i++ ) {
		stream << setw( width ) << "(" << obj.paths()[ i ] << ") ";
	}
	stream << endl;
	
	return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	s	: reference to input stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to input stream
//
istream &operator>>( istream &stream, CurvyTree &obj ) {
	return stream;
}




