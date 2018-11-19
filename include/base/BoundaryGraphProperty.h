//******************************************************************************
// BoundaryGraphProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_BoundaryGraphProperty_H
#define _BoundaryGraphProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "Coord2.h"
#include "Common.h"
#include "graph/BaseGraphProperty.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class BoundaryGraphProperty : public BaseGraphProperty{

  protected:

      void		    _init( void );

  public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
    BoundaryGraphProperty();				// constructor (default)
    BoundaryGraphProperty( const BoundaryGraphProperty & v ) {
    }					// copy constructor
    virtual ~BoundaryGraphProperty() {}		// destructor

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
    friend ostream &	operator << ( ostream & s, const BoundaryGraphProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, BoundaryGraphProperty & v );
				// Input
    virtual const char * className( void ) const { return "BoundaryGraphProperty"; }
				// class name

};

#endif // _BoundaryGraphProperty_H
