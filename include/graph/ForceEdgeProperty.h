//******************************************************************************
// ForceEdgeProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_ForceEdgeProperty_H
#define _ForceEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "base/Coord2.h"
#include "graph/BoundaryEdgeProperty.h"
#include "ui/GraphicsEdgeItem.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class ForceEdgeProperty: public BoundaryEdgeProperty {

  protected:

      void		    _init( void );


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    ForceEdgeProperty();				// constructor (default)
    ForceEdgeProperty( const ForceEdgeProperty & e )
        : BoundaryEdgeProperty ( e ) {
    }					                // copy constructor
    virtual ~ForceEdgeProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

    GraphicsEdgeItem *        itemPtr;        // pointer to graphic objects

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------

    void		                init( void )		      { _init(); }

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const ForceEdgeProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, ForceEdgeProperty & v );
				// Input
    virtual const char * className( void ) const { return "ForceEdgeProperty"; }
				// class name

};

#endif // _ForceEdgeProperty_H
