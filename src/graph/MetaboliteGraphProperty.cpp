//******************************************************************************
// MetaboliteVertexProperty.cc
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

#include "graph/MetaboliteGraphProperty.h"


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
//  BaseVertexProperty::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void MetaboliteGraphProperty::_init( double &width, double &height ) {
	BaseGraphProperty::_init( width, height );
}

//
//  MetaboliteGraphProperty::MetaboliteGraphProperty -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
MetaboliteGraphProperty::MetaboliteGraphProperty() {
	//BaseGraphProperty::init();
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
//	obj	: MetaboliteGraphProperty
//
//  Outputs
//	reference to output stream
//
ostream &operator<<( ostream &stream, const MetaboliteGraphProperty &obj ) {
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
//	obj	: MetaboliteGraphProperty
//
//  Outputs
//	reference to input stream
//
istream &operator>>( istream &stream, MetaboliteGraphProperty &obj ) {
	return stream;
}
