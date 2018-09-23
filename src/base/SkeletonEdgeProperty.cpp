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

#include "base/SkeletonEdgeProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  SkeletonEdgeProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void SkeletonEdgeProperty::_init( void )
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
//  SkeletonEdgeProperty::SkeletonEdgeProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
SkeletonEdgeProperty::SkeletonEdgeProperty()
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
//	obj	: SkeletonEdgeProperty
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const SkeletonEdgeProperty & obj )
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
//	obj	: SkeletonEdgeProperty
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, SkeletonEdgeProperty & obj )
{
    return stream;
}
