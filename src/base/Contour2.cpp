//******************************************************************************
// Contour2.cc
//	: program file for 2D contour
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Sun Sep 16 15:02:45 2012
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

#include "base/Contour2.h"

//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Contour2::_init --	initialization
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
void Contour2::_init( unsigned int __id, vector< Polygon2 > __polygons )
{
    _id = __id;
    _polygons = __polygons;
}

//
//  Contour2::_clear --	clear elements
//
//  Inputs
//
//
//  Outputs
//  none
//
void Contour2::_clear( void )
{
    _polygons.clear();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Contour2::Contour2 -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Contour2::Contour2()
{
}

//
//  Contour2::Contour2 -- constructor
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
Contour2::Contour2( vector< Polygon2 > __polygons )
{
    _polygons = __polygons;
}

//
//  Contour2::Contour2 -- copy constructor
//
//  Inputs
//  polygon
//
//  Outputs
//  none
//
Contour2::Contour2( const Contour2 & v )
{
    _id = v._id;
    _contour = v._contour;
    _polygons = v._polygons;
}


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//
//  Contour2::operator = --	assignment
//
//  Inputs
//	v	: 2D coordinates
//
//  Outputs
//	reference to this object
//
Contour2 & Contour2::operator = ( const Contour2 & p )
{
    if ( this != &p ) {
        _id = p._id;
        _contour = p._contour;
        _polygons = p._polygons;
    } 
    return *this;
}

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
bool Contour2::findVertexInComplex( Coord2 &coord, UndirectedBaseGraph &complex,
                                    UndirectedBaseGraph::vertex_descriptor &target )
{
    bool isFound = false;

    BGL_FORALL_VERTICES( vd, complex, UndirectedBaseGraph )
    {
        //cerr << " vd " << *complex[vd].coordPtr << endl;
        if( ( coord - *complex[vd].coordPtr ).norm() < 1e-2 ){
            target = vd;
            isFound = true;
        }
    }

    //cerr << "isFound = " << isFound << endl;
    return isFound;
}

void Contour2::createContour( void )
{
    // copy cells to a graph
    UndirectedBaseGraph complex;
    unsigned int id = 0;
    for( unsigned int j = 0; j < _polygons.size(); j++ ){

        Polygon2 &p = _polygons[j];
        vector< UndirectedBaseGraph::vertex_descriptor > vdVec;

        // add vertices
        for( unsigned int k = 0; k < p.elements().size(); k++ ) {
            UndirectedBaseGraph::vertex_descriptor vd = NULL;

            bool isFound = findVertexInComplex( p.elements()[k], complex, vd );
            if( isFound == true ){
                vdVec.push_back( vd );
            }
            else{
                vd = add_vertex( complex );
                complex[ vd ].id = id;
                complex[ vd ].coordPtr = new Coord2( p.elements()[k].x(), p.elements()[k].y() );
                vdVec.push_back( vd );
                id++;
            }
        }

        // add edges
        unsigned int eid = 0;
        for( unsigned int k = 1; k < vdVec.size()+1; k++ ){

            UndirectedBaseGraph::vertex_descriptor vdS = vdVec[ k-1 ];
            UndirectedBaseGraph::vertex_descriptor vdT = vdVec[ k%vdVec.size() ];

            bool isFound = false;
            UndirectedBaseGraph::edge_descriptor oldED;
            tie( oldED, isFound ) = edge( vdS, vdT, complex );

            if( isFound == true ){
                complex[ oldED ].weight += 1;
            }
            else{

                pair<UndirectedBaseGraph::edge_descriptor, unsigned int> foreE = add_edge( vdS, vdT, complex );
                UndirectedBaseGraph::edge_descriptor foreED = foreE.first;
                complex[ foreED ].id = eid;
                complex[ foreED ].weight = 0;
                eid++;
            }
        }
    }

    // remove inner edges
    vector< UndirectedBaseGraph::edge_descriptor > edVec;
    BGL_FORALL_EDGES( ed, complex, UndirectedBaseGraph )
    {
        if( complex[ed].weight > 0 ){
            edVec.push_back( ed );
            //cerr << "w = " << complex[ed].weight << endl;
        }
    }

#ifdef DEBUG
    cerr << "bnV = " << num_vertices( complex ) << " ";
    cerr << "bnE = " << num_edges( complex ) << endl;
#endif // DEBUG

    for( unsigned int j = 0; j < edVec.size(); j++ ){
        remove_edge( edVec[j], complex );
    }
    // reorder edge id
    unsigned int eid = 0;
    BGL_FORALL_EDGES( ed, complex, UndirectedBaseGraph )
    {
        complex[ed].id = eid;
        complex[ed].visit = false;
        eid++;
    }

    // find the vertex with degree > 0
#ifdef DEBUG
    cerr << "nV = " << num_vertices( complex ) << " ";
    cerr << "nE = " << num_edges( complex ) << endl;
#endif // DEBUG
    UndirectedBaseGraph::vertex_descriptor vdS;
    BGL_FORALL_VERTICES( vd, complex, UndirectedBaseGraph )
    {
#ifdef DEBUG
        if ( true ){
            cerr << " degree = " << out_degree( vd, complex );
        }
#endif // DEBUG
        if( out_degree( vd, complex ) > 0 ) {
            vdS = vd;
            break;
        }
    }

    // find the contour
    UndirectedBaseGraph::out_edge_iterator eo, eo_end;
    UndirectedBaseGraph::vertex_descriptor vdC = vdS;
    //cerr << "idC = " << complex[vdC].id << " ";
    _contour.elements().push_back( Coord2( complex[vdS].coordPtr->x(), complex[vdS].coordPtr->y() ) );
    while( true ){

        UndirectedBaseGraph::vertex_descriptor vdT = NULL;
        for( tie( eo, eo_end ) = out_edges( vdC, complex ); eo != eo_end; eo++ ){

            UndirectedBaseGraph::edge_descriptor ed = *eo;
            if( complex[ed].visit == false ) {
                vdT = target( ed, complex );
                complex[ed].visit = true;
                break;
            }
            //cerr << "(" << complex[source( ed, complex )].id << "," << complex[target( ed, complex )].id << ")" << endl;
        }

        if( vdS == vdT ) break;
        else assert( vdT != NULL );
        //cerr << complex[vdT].id << " ";
        _contour.elements().push_back( Coord2( complex[vdT].coordPtr->x(), complex[vdT].coordPtr->y() ) );
        vdC = vdT;
    }
    _contour.updateCentroid();
    _contour.updateOrientation();
    //cerr << "centroid = " << polygon.centroid() << endl;
    //cerr << "p[" << i << "] = " << polygon << endl;
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
ostream & operator << ( ostream & stream, const Contour2 & obj )
{
    int i;		// loop counter
    // set the output formatting
    //stream << setiosflags( ios::showpoint );
    //stream << setprecision( 8 );
    //int width = 16;
    // print out the elements
    for ( i = 0; i < obj._contour.elements().size(); i++ ) {
	    //stream << setw( width ) << obj._element[ i ];
    	stream << setw( 4 ) << obj._contour.elements()[ i ];
	    if ( i != 1 ) stream << "\t";
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
istream & operator >> ( istream & stream, Contour2 & obj )
{
    int i;		// loop counter
    // reading the elements
    for ( i = 0; i < obj._contour.elements().size(); i++ )
	stream >> obj._contour.elements()[ i ];
    return stream;
}




