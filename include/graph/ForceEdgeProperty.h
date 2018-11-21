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
#include "graph/BaseEdgeProperty.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class ForceEdgeProperty: public BaseEdgeProperty {

  protected:

      void		    _init( void );


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    ForceEdgeProperty();				// constructor (default)
    ForceEdgeProperty( const ForceEdgeProperty & e )
        : BaseEdgeProperty ( e ) {
    }					                // copy constructor
    virtual ~ForceEdgeProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------

    void		                init( void )		      { _init(); }

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
    friend ostream &	operator << ( ostream & s, const ForceEdgeProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, ForceEdgeProperty & v );
				// Input
    virtual const char * className( void ) const { return "ForceEdgeProperty"; }
				// class name

};

#endif // _ForceEdgeProperty_H