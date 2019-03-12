//******************************************************************************
// ForceVertexProperty.h
//	: header file for 2D coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_ForceVertexProperty_H
#define _ForceVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "base/Coord2.h"
#include "base/Common.h"
#include "graph/BoundaryVertexProperty.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class ForceVertexProperty: public BoundaryVertexProperty {

  protected:

      void		    _init( void );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    ForceVertexProperty();				// constructor (default)
    ForceVertexProperty( const ForceVertexProperty & v )
        : BoundaryVertexProperty ( v ) {
    }					// copy constructor
    virtual ~ForceVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

    unsigned int                leafID;         // quadtree id
    unsigned int                label;          // mcl clustering id

    Coord2 *                    prevCoordPtr;   // previous position
    Coord2 *                    forcePtr;       // spring force
    Coord2 *                    placePtr;       // voronoi force
    Coord2 *                    shiftPtr;       // displacement

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

    friend ostream &	operator << ( ostream & s, const ForceVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, ForceVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "ForceVertexProperty"; }
				// class name

};

#endif // _ForceVertexProperty_H
