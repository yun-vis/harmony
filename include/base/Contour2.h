//******************************************************************************
// Contour2.h
//	: header file for Contour2
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Jun 19 02:36:37 2018
//
//******************************************************************************

#ifndef	_Contour2_H
#define _Contour2_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

#include "base/Polygon2.h"
#include "graph/UndirectedBaseGraph.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Contour2 {

  protected:

    unsigned int        _id;
    Polygon2            _contour;       // centroid of the elements
    vector< Polygon2 >  _polygons;      // coordinates of end points

    virtual void	    _init( unsigned int __id, vector< Polygon2 > __polygons );
    void                _clear( void );

  public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
    Contour2();				// constructor (default)
    Contour2( vector< Polygon2 > __polygons );
					        // coordinates as input
    Contour2( const Contour2 & v );		// copy constructor
    virtual ~Contour2() {}		// destructor

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    Contour2 &		operator = ( const Contour2 & p );
    // assignment

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
    void		init( unsigned int __id, vector< Polygon2 > __polygons )		{ _init( __id, __polygons ); }
    void        clear( void ) { _clear(); }

    // id
    unsigned int &	            id( void )	            { return _id; }
    const unsigned int &	    id( void ) const	    { return _id; }

    // contour
    Polygon2 &	                contour( void )	        { return _contour; }
    const Polygon2 &            contour( void ) const	{ return _contour; }

    // a vector of polygons
    vector< Polygon2 > &	    polygons( void )	    { return _polygons; }
    const vector< Polygon2 > &	polygons( void ) const	{ return _polygons; }

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
    bool findVertexInComplex( Coord2 &coord, UndirectedBaseGraph &complex,
                              UndirectedBaseGraph::vertex_descriptor &target );
    void createContour( void );

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream &	operator << ( ostream & s, const Contour2 & v );
    friend istream &	operator >> ( istream & s, Contour2 & v );
    virtual const char * className( void ) const { return "Contour2"; }

};


#endif // _Contour2_H
