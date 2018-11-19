//******************************************************************************
// BoundaryEdgeProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_BoundaryEdgeProperty_H
#define _BoundaryEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "base/Coord2.h"
#include "base/Line2.h"
#include "graph/BaseEdgeProperty.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class BoundaryEdgeProperty: public BaseEdgeProperty {

  protected:

      void		    _init( void );


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    BoundaryEdgeProperty();				// constructor (default)
    BoundaryEdgeProperty( const BoundaryEdgeProperty & e ) {
	   id	    = e.id;
	   weight	= e.weight;
    }					// copy constructor
    virtual ~BoundaryEdgeProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    unsigned int                initID;
    vector< unsigned int >      lineID;

    double                      geoAngle;
    double                      smoothAngle;
    double                      targetAngle;

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
    friend ostream &	operator << ( ostream & s, const BoundaryEdgeProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, BoundaryEdgeProperty & v );
				// Input
    virtual const char * className( void ) const { return "BoundaryEdgeProperty"; }
				// class name

};

#endif // _BoundaryEdgeProperty_H
