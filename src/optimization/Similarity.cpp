//==============================================================================
// Similarity.cc
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Thu Sep  8 02:55:49 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <ctime>

using namespace std;

#include <boost/config.hpp>
#include <boost/graph/isomorphism.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

#include "optimization/Similarity.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------
//  Similarity::_isSimilarity --	return if the two graphs are similar
//
//
//  Inputs
//	none
//
//  Outputs
//	none
//
bool Similarity::_isSimilar( void )
{
    bool isSimilar = false;

#ifdef DEBUG
    _g1Ptr = new ForceGraph;
    _g2Ptr = new ForceGraph;

    ForceGraph::vertex_descriptor vd1 = add_vertex( *_g1Ptr );
    (*_g1Ptr)[vd1].id = 0;
    ForceGraph::vertex_descriptor vd2 = add_vertex( *_g1Ptr );
    (*_g1Ptr)[vd2].id = 1;
    ForceGraph::vertex_descriptor vd3 = add_vertex( *_g1Ptr );
    (*_g1Ptr)[vd3].id = 2;
    add_edge( vd1, vd2, *_g1Ptr );
    add_edge( vd2, vd3, *_g1Ptr );

    ForceGraph::vertex_descriptor vd4 = add_vertex( *_g2Ptr );
    (*_g2Ptr)[vd4].id = 0;
    ForceGraph::vertex_descriptor vd5 = add_vertex( *_g2Ptr );
    (*_g2Ptr)[vd5].id = 1;
    ForceGraph::vertex_descriptor vd6 = add_vertex( *_g2Ptr );
    (*_g2Ptr)[vd6].id = 2;
    add_edge( vd4, vd6, *_g2Ptr );
    add_edge( vd6, vd5, *_g2Ptr );

    cerr << "num_vertices( *_g1Ptr ) = " << num_vertices( *_g1Ptr ) << endl;
    cerr << "num_vertices( *_g2Ptr ) = " << num_vertices( *_g2Ptr ) << endl;
#endif // DEBUG

    if( num_vertices( *_g1Ptr ) != num_vertices( *_g2Ptr ) )
        return false;

    vector< graph_traits< ForceGraph >::vertex_descriptor > f( num_vertices( *_g1Ptr ) );

    isSimilar = isomorphism( *_g1Ptr, *_g2Ptr, isomorphism_map( make_iterator_property_map( f.begin(),
                                                                                  get( &ForceVertexProperty::id, *_g1Ptr ), f[0] ) )
            .vertex_index1_map( get( &ForceVertexProperty::id, *_g1Ptr ) )
            .vertex_index2_map( get( &ForceVertexProperty::id, *_g2Ptr ) ) );   // topology only
//            .vertex_invariant1( my_invariant1 )                               // name label
//            .vertex_invariant2( my_invariant2 ) );

    return isSimilar;
}

//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Similarity::_init --	initialize the network (called once when necessary)
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Similarity::_init( ForceGraph *__g1Ptr, ForceGraph *__g2Ptr )
{
    _g1Ptr = __g1Ptr;
    _g2Ptr = __g2Ptr;
}


//
//  Similarity::_clear --	clear the Similarity object
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void Similarity::_clear( void )
{
}

//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Similarity::Similarity --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Similarity::Similarity()
{
    _id = 0;
}


//
//  Similarity::Similarity --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
Similarity::Similarity( const Similarity & obj )
{
    _id = obj._id;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  Similarity::~Similarity --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
Similarity::~Similarity()
{
}


//------------------------------------------------------------------------------
//	Referring to members
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
//
//  Similarity::operator = --	assignment
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
Similarity & Similarity::operator = ( const Similarity & obj )
{
    _id = obj._id;

    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Argument
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const Similarity & obj )
{
    // stream << ( const Graph & )obj;	
    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, Similarity & obj )
{
    // stream >> ( Graph & )obj;
    return stream;
}


// end of header file
// Do not add any stuff under this line.
