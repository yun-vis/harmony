//******************************************************************************
// BaseEdgeProperty.cc
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

#include "base/UndirectedBaseEdgeProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  UndirectedBaseEdgeProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void UndirectedBaseEdgeProperty::_init( void )
{
	id = 0;
	weight = 1.0;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------

//
//  UndirectedBaseEdgeProperty::UndirectedBaseEdgeProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
UndirectedBaseEdgeProperty::UndirectedBaseEdgeProperty()
{
    _init();
}



//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

//
//  operator << --	output
//
//  Inputs
//	stream	: reference to output stream
//	obj	: UndirectedBaseEdgeProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const UndirectedBaseEdgeProperty & obj )
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
//	obj	: UndirectedBaseEdgeProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, UndirectedBaseEdgeProperty & obj )
{
    return stream;
}
