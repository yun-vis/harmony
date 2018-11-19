//******************************************************************************
// BaseVertexProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_BaseVertexProperty_H
#define _BaseVertexProperty_H

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

class BaseVertexProperty {

  protected:

      void		    _init( void );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    BaseVertexProperty();				// constructor (default)
    BaseVertexProperty( const BaseVertexProperty & v ) {
    }					// copy constructor
    virtual ~BaseVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    unsigned int                id;
    unsigned int                subGID;

    double                      weight;
    string *                    namePtr;
    double *                    namePixelWidthPtr;      // pixel width of the name
    double *                    namePixelHeightPtr;     // pixel height of the name

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

    friend ostream &	operator << ( ostream & s, const BaseVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, BaseVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "BaseVertexProperty"; }
				// class name

};

#endif // _BaseVertexProperty_H
