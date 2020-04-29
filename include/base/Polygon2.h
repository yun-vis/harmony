//******************************************************************************
// Polygon2.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Jun 19 02:36:37 2012
//
//******************************************************************************

#ifndef _Polygon2_H
#define _Polygon2_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Polygon_2< K >::Vertex_circulator Vertex_circulator;

using CGAL::ORIGIN;

#include "base/Coord2.h"
#include "base/Line2.h"
#include "ui/GraphicsPolygonItem.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Polygon2 {

protected:
	
	unsigned int _id;
	unsigned int _gid;
	double _area;
	Coord2 _center;        // average of the elements
	Coord2 _centroid;      // centroid of the elements
	vector< Coord2 > _elements;      // coordinates of end points
	Coord2 _boundingBox;   // width and height of the bounding box
	Coord2 _boxCenter;     // center of the bounding box
	CGAL::Polygon_2< K > _polygon;
	
	// graphics item pointer
	GraphicsPolygonItem *_itemPtr;
	
	virtual void _init( unsigned int __id, vector< Coord2 > __elements );    // initialize all coordinates to zero
	void _clear( void );

public:

//------------------------------------------------------------------------------
//	Constuructors
//------------------------------------------------------------------------------
	Polygon2();                // constructor (default)
	Polygon2( vector< Coord2 > __elements );
	
	// coordinates as input
	Polygon2( const Polygon2 &v );        // copy constructor
	virtual ~Polygon2() {}        // destructor

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
	Polygon2 &operator=( const Polygon2 &p );
	// assignment

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
	void init( unsigned int __id, vector< Coord2 > __elements ) { _init( __id, __elements ); }
	
	void clear( void ) { _clear(); }
	
	// id
	unsigned int &id( void ) { return _id; }
	
	const unsigned int &id( void ) const { return _id; }
	
	// group id
	unsigned int &gid( void ) { return _gid; }
	
	const unsigned int &gid( void ) const { return _gid; }
	
	// area
	double &area( void ) { return _area; }
	
	const double &area( void ) const { return _area; }
	
	// barycenter
	Coord2 &center( void ) { return _center; }
	
	const Coord2 &center( void ) const { return _center; }
	
	// centroid
	Coord2 &centroid( void ) { return _centroid; }
	
	const Coord2 &centroid( void ) const { return _centroid; }
	
	// pointer to an array of coordinates
	vector< Coord2 > &elements( void ) { return _elements; }
	
	const vector< Coord2 > &elements( void ) const { return _elements; }
	
	// width and height of the bounding box
	Coord2 &boundingBox( void ) { return _boundingBox; }
	
	const Coord2 &boundingBox( void ) const { return _boundingBox; }
	
	// center of the bounding box
	Coord2 &boxCenter( void ) { return _boxCenter; }
	
	const Coord2 &boxCenter( void ) const { return _boxCenter; }
	
	// graphics item
	void setItemPtr( GraphicsPolygonItem *ptr ) { _itemPtr = ptr; }
	
	GraphicsPolygonItem *getItemPtr( void ) { return _itemPtr; }

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
	void computeBoundingBox( void );
	
	void updateCentroid( void );
	
	void updateOrientation( void );
	
	bool inPolygon( const Coord2 &cood );
	
	double minDistToPolygon( const Coord2 &cood );
	
	double maxRadiusInPolygon( const Coord2 &coord );
	
	void cleanPolygon( void );

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &s, const Polygon2 &v );
	
	friend istream &operator>>( istream &s, Polygon2 &v );
	
	virtual const char *className( void ) const { return "Polygon2"; }
	
};


#endif // _Polygon2_H
