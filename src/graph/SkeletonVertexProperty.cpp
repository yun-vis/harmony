//******************************************************************************
// BaseVertexProperty.cc
//	: program file for 2D coordinates
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

#include "graph/SkeletonVertexProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  SkeletonVertexProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void SkeletonVertexProperty::_init( void ) {
	id = 0;
	coordPtr = NULL;
	widthPtr = NULL;
	heightPtr = NULL;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  SkeletonVertexProperty::SkeletonVertexProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
SkeletonVertexProperty::SkeletonVertexProperty() {
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
//	obj	: SkeletonVertexProperty
//
//  Outputs
//	reference to output stream
//
ostream &operator<<( ostream &stream, const SkeletonVertexProperty &obj ) {
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
//	obj	: SkeletonVertexProperty
//
//  Outputs
//	reference to input stream
//
istream &operator>>( istream &stream, SkeletonVertexProperty &obj ) {
	return stream;
}
