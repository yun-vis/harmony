//******************************************************************************
// MetaboliteEdgeProperty.cc
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

#include "graph/MetaboliteEdgeProperty.h"


//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------

//
//  MetaboliteEdgeProperty::MetaboliteEdgeProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
MetaboliteEdgeProperty::MetaboliteEdgeProperty() {
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
//	obj	: MetaboliteEdgeProperty
//
//  Outputs
//	reference to output stream
//
ostream &operator<<( ostream &stream, const MetaboliteEdgeProperty &obj ) {
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
//	obj	: MetaboliteEdgeProperty
//
//  Outputs
//	reference to input stream
//
istream &operator>>( istream &stream, MetaboliteEdgeProperty &obj ) {
	return stream;
}
