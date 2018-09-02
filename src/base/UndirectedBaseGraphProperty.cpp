//******************************************************************************
// UndirectedBaseGraphProperty.cc
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

#include "base/UndirectedBaseGraphProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  UndirectedBaseGraphProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void UndirectedBaseGraphProperty::_init( void )
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
//  UndirectedBaseGraphProperty::UndirectedBaseGraphProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
UndirectedBaseGraphProperty::UndirectedBaseGraphProperty()
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
//	obj	: UndirectedBaseGraphProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const UndirectedBaseGraphProperty & obj )
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
//	obj	: UndirectedBaseGraphProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, UndirectedBaseGraphProperty & obj )
{
    return stream;
}
