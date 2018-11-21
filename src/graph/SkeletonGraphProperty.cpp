//******************************************************************************
// SkeletonGraphProperty.cc
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

#include "graph/SkeletonGraphProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  SkeletonGraphProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void SkeletonGraphProperty::_init( void )
{
    BaseGraphProperty::_init();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  SkeletonGraphProperty::SkeletonGraphProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
SkeletonGraphProperty::SkeletonGraphProperty()
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
//	obj	: SkeletonGraphProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const SkeletonGraphProperty & obj )
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
//	obj	: SkeletonGraphProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, SkeletonGraphProperty & obj )
{
    return stream;
}
