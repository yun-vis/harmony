//******************************************************************************
// Polygon2.cc
//	: program file for 2D coordinates
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

#include "base/Polygon2.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Polygon2::_init --	initialization
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
void Polygon2::_init( unsigned int __id, vector< Coord2 > __elements )
{
    _id = __id;
    _elements = __elements;
}

//
//  Polygon2::_clear --	clear elements
//
//  Inputs
//
//
//  Outputs
//  none
//
void Polygon2::_clear( void )
{
    _elements.clear();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Polygon2::Polygon2 -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Polygon2::Polygon2()
{
}

//
//  Polygon2::Polygon2 -- constructor
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
Polygon2::Polygon2( vector< Coord2 > __elements )
{
    _elements = __elements;
}

//
//  Polygon2::Polygon2 -- copy constructor
//
//  Inputs
//  polygon
//
//  Outputs
//  none
//
Polygon2::Polygon2( const Polygon2 & v )
{
    _id = v._id;
    _gid = v._gid;
    _area = v._area;
    _center = v._center;
    _elements = v._elements;
}


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//
//  Polygon2::operator = --	assignment
//
//  Inputs
//	v	: 2D coordinates
//
//  Outputs
//	reference to this object
//
Polygon2 & Polygon2::operator = ( const Polygon2 & p )
{
    if ( this != &p ) {
        _id = p._id;
        _gid = p._gid;
        _area = p._area;
        _center = p._center;
        _elements = p._elements;
    } 
    return *this;
}

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
void Polygon2::boundingBox( Coord2 &center, double &width, double &height )
{
    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
    for( unsigned int i = 0; i < _elements.size(); i++ ){
        if( minX > _elements[i].x() ) minX = _elements[i].x();
        if( maxX < _elements[i].x() ) maxX = _elements[i].x();
        if( minY > _elements[i].y() ) minY = _elements[i].y();
        if( maxY < _elements[i].y() ) maxY = _elements[i].y();
    }

    center.x() = ( minX + maxX )/2.0;
    center.y() = ( minY + maxY )/2.0;
    width = maxX - minX;
    height = maxY - minY;
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
ostream & operator << ( ostream & stream, const Polygon2 & obj )
{
    int i;		// loop counter
    // set the output formatting
    //stream << setiosflags( ios::showpoint );
    //stream << setprecision( 8 );
    //int width = 16;
    // print out the elements
    for ( i = 0; i < obj._elements.size(); i++ ) {
	    //stream << setw( width ) << obj._element[ i ];
    	stream << setw( 4 ) << obj._elements[ i ];
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
istream & operator >> ( istream & stream, Polygon2 & obj )
{
    int i;		// loop counter
    // reading the elements
    for ( i = 0; i < obj._elements.size(); i++ )
	stream >> obj._elements[ i ];
    return stream;
}




