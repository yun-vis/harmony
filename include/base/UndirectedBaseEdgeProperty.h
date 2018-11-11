//******************************************************************************
// UndirectedBaseEdgeProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef	_UndirectedBaseEdgeProperty_H
#define _UndirectedBaseEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "base/Coord2.h"
#include "base/Line2.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class UndirectedBaseEdgeProperty {

  protected:

      void		    _init( void );


  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    UndirectedBaseEdgeProperty();				// constructor (default)
    UndirectedBaseEdgeProperty( const UndirectedBaseEdgeProperty & e ) {
	   id	    = e.id;
	   weight	= e.weight;
    }					// copy constructor
    virtual ~UndirectedBaseEdgeProperty() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    unsigned int                id;

    double                      angle;
    double                      weight;
    bool                        visit;

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
    friend ostream &	operator << ( ostream & s, const UndirectedBaseEdgeProperty & v );
				// Output
    friend istream &	operator >> ( istream & s, UndirectedBaseEdgeProperty & v );
				// Input
    virtual const char * className( void ) const { return "UndirectedBaseEdgeProperty"; }
				// class name

};

#endif // _UndirectedBaseEdgeProperty_H
