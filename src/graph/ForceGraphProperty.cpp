//******************************************************************************
// ForceGraphProperty.cc
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

#include "graph/ForceGraphProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  ForceGraphProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void ForceGraphProperty::_init( void ) {
	//BaseGraphProperty::init();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------

//
//  ForceGraphProperty::ForceGraphProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
ForceGraphProperty::ForceGraphProperty() {
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
//	obj	: ForceGraphProperty
//
//  Outputs
//	reference to output stream
//
ostream &operator<<( ostream &stream, const ForceGraphProperty &obj ) {
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
//	obj	: ForceGraphProperty
//
//  Outputs
//	reference to input stream
//
istream &operator>>( istream &stream, ForceGraphProperty &obj ) {
	return stream;
}
