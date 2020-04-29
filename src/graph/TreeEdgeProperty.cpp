//******************************************************************************
// BaseEdgeProperty.cc
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

#include "graph/TreeEdgeProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  TreeEdgeProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void TreeEdgeProperty::_init( void ) {
	BaseEdgeProperty::_init();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  TreeEdgeProperty::TreeEdgeProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
TreeEdgeProperty::TreeEdgeProperty() {
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
//	obj	: TreeEdgeProperty
//
//  Outputs
//	reference to output stream
//
ostream &operator<<( ostream &stream, const TreeEdgeProperty &obj ) {
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
//	obj	: TreeEdgeProperty
//
//  Outputs
//	reference to input stream
//
istream &operator>>( istream &stream, TreeEdgeProperty &obj ) {
	return stream;
}
