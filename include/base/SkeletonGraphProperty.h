//******************************************************************************
// SkeletonGraphProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_SkeletonGraphProperty_H
#define _SkeletonGraphProperty_H

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

class SkeletonGraphProperty : public BaseGraphProperty{

  protected:

      void		    _init( void );

  public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
    SkeletonGraphProperty();				// constructor (default)
    SkeletonGraphProperty( const SkeletonGraphProperty & v ) {
    }					// copy constructor
    virtual ~SkeletonGraphProperty() {}		// destructor

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
    friend ostream &	operator << ( ostream & s, const SkeletonGraphProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, SkeletonGraphProperty & v );
				// Input
    virtual const char * className( void ) const { return "SkeletonGraphProperty"; }
				// class name

};

#endif // _SkeletonGraphProperty_H
