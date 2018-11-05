//******************************************************************************
// Polygon2.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Jun 19 02:36:37 2012
//
//******************************************************************************

#ifndef	_Polygon2_H
#define _Polygon2_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

#include "base/Coord2.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Polygon2 {

  protected:

    unsigned int        _id;
    double              _area;
    Coord2              _center;
    vector< Coord2 >    _elements;      // coordinates of end points

    virtual void	    _init( unsigned int __id, vector< Coord2 > __elements );	// initialize all coordinates to zero

  public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
    Polygon2();				// constructor (default)
    Polygon2( vector< Coord2 > __elements );
					        // coordinates as input
    Polygon2( const Polygon2 & v );		// copy constructor
    virtual ~Polygon2() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Polygon2 &		operator = ( const Polygon2 & p );
    // assignment

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    void		init( unsigned int __id, vector< Coord2 > __elements )		{ _init( __id, __elements ); }

    unsigned int &	            id( void )	            { return _id; }
    const unsigned int &	    id( void ) const	    { return _id; }

    // area
    double &	                area( void )	        { return _area; }
    const double &	            area( void ) const	    { return _area; }

    // barycenter
    Coord2 &	                center( void )	        { return _center; }
    const Coord2 &	            center( void ) const	{ return _center; }

    // pointer to an array of coordinates
    vector< Coord2 > &	        elements( void )	    { return _elements; }
    const vector< Coord2 > &	elements( void ) const	{ return _elements; }

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
    friend ostream &	operator << ( ostream & s, const Polygon2 & v );
    friend istream &	operator >> ( istream & s, Polygon2 & v );
    virtual const char * className( void ) const { return "Polygon2"; }

};


#endif // _Polygon2_H
