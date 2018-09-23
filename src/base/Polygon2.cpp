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
        _elements = p._elements;
    } 
    return *this;
}

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------

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




