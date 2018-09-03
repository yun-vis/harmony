//******************************************************************************
// UndirectedBaseGraphProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_UndirectedBaseGraphProperty_H
#define _UndirectedBaseGraphProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>

using namespace std;

#include "base/Coord2.h"
#include "base/Common.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class UndirectedBaseGraphProperty {

  protected:

      void		    _init( void );

  public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
    UndirectedBaseGraphProperty();				// constructor (default)
    UndirectedBaseGraphProperty( const UndirectedBaseGraphProperty & v ) {
    }					// copy constructor
    virtual ~UndirectedBaseGraphProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    Coord2 *                    centerPtr;
    double                      width;
    double                      height;

    void		                init( void )		{ _init(); }

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
    friend ostream &	operator << ( ostream & s, const UndirectedBaseGraphProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, UndirectedBaseGraphProperty & v );
				// Input
    virtual const char * className( void ) const { return "UndirectedBaseGraphProperty"; }
				// class name

};

#endif // _UndirectedBaseGraphProperty_H
