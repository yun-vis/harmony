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

#include "graph/TreeVertexProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  TreeVertexProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void TreeVertexProperty::_init( void ) {
	BaseVertexProperty::_init();
	
	leafID = -1;
	meanCoord.x() = 0.0;
	meanCoord.y() = 0.0;
	leafVec.clear();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  TreeVertexProperty::TreeVertexProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
TreeVertexProperty::TreeVertexProperty() {
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
//	obj	: TreeVertexProperty
//
//  Outputs
//	reference to output stream
//
ostream &operator<<( ostream &stream, const TreeVertexProperty &obj ) {
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
//	obj	: TreeVertexProperty
//
//  Outputs
//	reference to input stream
//
istream &operator>>( istream &stream, TreeVertexProperty &obj ) {
	return stream;
}
