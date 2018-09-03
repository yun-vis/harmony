//******************************************************************************
// UndirectedBaseVertexProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_UndirectedBaseVertexProperty_H
#define _UndirectedBaseVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "base/Coord2.h"
#include "base/Common.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class UndirectedBaseVertexProperty {

  protected:

      void		    _init( void );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    UndirectedBaseVertexProperty();				// constructor (default)
    UndirectedBaseVertexProperty( const UndirectedBaseVertexProperty & v ) {
    }					// copy constructor
    virtual ~UndirectedBaseVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    unsigned int                id;

    double                      weight;
    string                      name;

    Coord2 *                    coordPtr;

    double *                    widthPtr;
    double *                    heightPtr;

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------

    void		                init( void )		{ _init(); }

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------

    friend ostream &	operator << ( ostream & s, const UndirectedBaseVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, UndirectedBaseVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "UndirectedBaseVertexProperty"; }
				// class name

};

#endif // _UndirectedBaseVertexProperty_H
