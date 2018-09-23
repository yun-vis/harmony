//******************************************************************************
// BoundaryVertexProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_SkeletonVertexProperty_H
#define _SkeletonVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "base/Coord2.h"
#include "base/Common.h"
#include "base/UndirectedBaseVertexProperty.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class SkeletonVertexProperty: public UndirectedBaseVertexProperty {

  protected:

      void		    _init( void );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    SkeletonVertexProperty();				// constructor (default)
    SkeletonVertexProperty( const SkeletonVertexProperty & v ) {
    }					// copy constructor
    virtual ~SkeletonVertexProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    unsigned int                initID;
    //vector< unsigned int >      lineID;

    Coord2                      shift;       // displacement
    Coord2                      force;       // displacement of the force-directed algorithm
    Coord2                      place;       // displacement of the voronoi cells


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

    friend ostream &	operator << ( ostream & s, const SkeletonVertexProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, SkeletonVertexProperty & v );
				// Input
    virtual const char * className( void ) const { return "SkeletonVertexProperty"; }
				// class name

};

#endif // _SkeletonVertexProperty_H
