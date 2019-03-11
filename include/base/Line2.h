//******************************************************************************
// Line2.h
//	: header file for 2D Line2 coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2017
//
//******************************************************************************

#ifndef	_Line2_H
#define _Line2_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <iostream>

using namespace std;

#include "Coord2.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Line2 {

  protected:

    vector< Coord2 >	_samples;	    // Line2 sample points

    virtual void	    _init( void );	// initialize all coordinates to zero

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Line2();				// constructor (default)
    Line2( const Line2 & v ) {
    	_samples	= v._samples;
	}					// copy constructor
    virtual ~Line2() {}	// destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    void		init( void )		{ _init(); }

    vector< Coord2> &	        samples( void ) 	    { return _samples; }
    const vector< Coord2 > &	samples( void ) const	{ return _samples; }


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
    void        addSample( Coord2 & coord );
    static bool isOnLine( Coord2 &a, Coord2 &b, Coord2 &c );

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const Line2 & v );
				// Output
    friend istream &	operator >> ( istream & s, Line2 & v );
				// Input
    virtual const char * className( void ) const { return "Line2"; }
				// Class name
};


#endif // _Line2_H
