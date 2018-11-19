//******************************************************************************
// BoundaryGraphProperty.cc
//	: program file for 2D coordinatse
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:15:32 2017
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

#include "graph/BoundaryGraphProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  BoundaryGraphProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void BoundaryGraphProperty::_init( void )
{
    centerPtr   = NULL;
    width       = DEFAULT_WIDTH;
    height      = DEFAULT_HEIGHT;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  BoundaryGraphProperty::BoundaryGraphProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
BoundaryGraphProperty::BoundaryGraphProperty()
{
    _init();
}



//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	stream	: reference to output stream
//	obj	: BoundaryGraphProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const BoundaryGraphProperty & obj )
{
    // set the output formatting
    stream << setiosflags( ios::showpoint );
    stream << setprecision( 8 );
    stream << endl;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: reference to output stream
//	obj	: BoundaryGraphProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, BoundaryGraphProperty & obj )
{
    return stream;
}
