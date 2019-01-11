//******************************************************************************
// MetaboliteGraphProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_MetaboliteGraphProperty_H
#define _MetaboliteGraphProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "base/Coord2.h"
#include "base/CommonClass.h"
#include "graph/BaseGraphProperty.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------
class MetaboliteGraphProperty : public BaseGraphProperty {

  protected:

	void _init( double &width, double &height );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    MetaboliteGraphProperty();				// constructor (default)
    MetaboliteGraphProperty( const MetaboliteGraphProperty & m ) {
    }					// copy constructor
    virtual ~MetaboliteGraphProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const MetaboliteGraphProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, MetaboliteGraphProperty & v );
				// Input
    virtual const char * className( void ) const { return "MetaboliteGraphProperty"; }
				// class name

};

#endif // _MetaboliteGraphProperty_H
