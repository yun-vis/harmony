//******************************************************************************
// Polygon2.cc
//	: program file for 2D coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Sun Sep 16 15:02:45 2012
//
//******************************************************************************

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <algorithm>
using namespace std;

#include "base/Polygon2.h"

//------------------------------------------------------------------------------
//	Macro Definitions
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  Polygon2::_init --	initialization
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
void Polygon2::_init( unsigned int __id, vector< Coord2 > __elements )
{
    _id = __id;
    _elements = __elements;
}

//
//  Polygon2::_clear --	clear elements
//
//  Inputs
//
//
//  Outputs
//  none
//
void Polygon2::_clear( void )
{
    _elements.clear();
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  Polygon2::Polygon2 -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Polygon2::Polygon2()
{
}

//
//  Polygon2::Polygon2 -- constructor
//
//  Inputs
//  __elements
//
//  Outputs
//  none
//
Polygon2::Polygon2( vector< Coord2 > __elements )
{
    _elements = __elements;
}

//
//  Polygon2::Polygon2 -- copy constructor
//
//  Inputs
//  polygon
//
//  Outputs
//  none
//
Polygon2::Polygon2( const Polygon2 & v )
{
    _id = v._id;
    _gid = v._gid;
    _area = v._area;
    _center = v._center;
    _centroid = v._centroid;
    _elements = v._elements;
}


//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//
//  Polygon2::operator = --	assignment
//
//  Inputs
//	v	: 2D coordinates
//
//  Outputs
//	reference to this object
//
Polygon2 & Polygon2::operator = ( const Polygon2 & p )
{
    if ( this != &p ) {
        _id = p._id;
        _gid = p._gid;
        _area = p._area;
        _center = p._center;
        _centroid = p._centroid;
        _elements = p._elements;
    } 
    return *this;
}

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
//
//  Polygon2::updateCentroid --    update centroid of the polygon
//
//  Inputs
//  center: center of the bounding box
//  width: width of the bounding box
//  height: height of the bounding box
//
//  Outputs
//  none
//
void Polygon2::boundingBox( Coord2 &center, double &width, double &height )
{
    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
    for( unsigned int i = 0; i < _elements.size(); i++ ){
        if( minX > _elements[i].x() ) minX = _elements[i].x();
        if( maxX < _elements[i].x() ) maxX = _elements[i].x();
        if( minY > _elements[i].y() ) minY = _elements[i].y();
        if( maxY < _elements[i].y() ) maxY = _elements[i].y();
    }

    center.x() = ( minX + maxX )/2.0;
    center.y() = ( minY + maxY )/2.0;
    width = maxX - minX;
    height = maxY - minY;
}


//
//  Polygon2::updateCentroid --    update centroid of the polygon
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Polygon2::updateCentroid( void )
{
    CGAL::Polygon_2< K > polygon;
    _centroid.x() = 0.0;
    _centroid.y() = 0.0;

    // initialization
    for( unsigned int i = 0; i < _elements.size(); i++ ){
        polygon.push_back( K::Point_2( _elements[i].x(), _elements[i].y() ) );
    }

    // check if the polygon has at least three vertices
    assert( polygon.size() >= 3 );
    Vertex_circulator start = polygon.vertices_circulator();
    Vertex_circulator cur = start;
    Vertex_circulator next = cur;
    ++next;
    CGAL::Vector_2< K > center( 0, 0 );
    double a = 0.0, atot = 0.0;
    do {
        // cerr << CGAL::to_double( ((*cur).x()) * ((*next).y()) - ((*next).x()) * ((*cur).y()) ) << endl;
        a = CGAL::to_double( ((*cur).x()) * ((*next).y()) - ((*next).x()) * ((*cur).y()) );
        center = center + a * ((*cur - ORIGIN) + (*next - ORIGIN));
        atot = atot + a;
        cur = next;
        ++next;
    } while (cur != start);
    atot = 3*atot;
    center = center/atot;

    double x = CGAL::to_double( (ORIGIN + center).x() );
    double y = CGAL::to_double( (ORIGIN + center).y() );
    _centroid.x() = x;
    _centroid.y() = y;
}


//
//  Polygon2::updateOrientation --    update polygon elements if they are not counterclockwise
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Polygon2::updateOrientation( void )
{
    CGAL::Polygon_2< K > p;

    for( unsigned int i = 0; i < _elements.size(); i++ ){
        p.push_back( K::Point_2( _elements[i].x(), _elements[i].y() ) );
    }

    if( p.orientation() == -1 ){
        p.reverse_orientation();

        _elements.clear();
        for( unsigned int i = 0; i < p.size(); i++ ){
            _elements.push_back( Coord2( CGAL::to_double( p[i].x() ), CGAL::to_double( p[i].y() ) ) );
        }
    }
}

//
//  Polygon2::updateOrientation --    update polygon elements if they are not counterclockwise
//
//  Inputs
//  none
//
//  Outputs
//  none
//
bool Polygon2::inPolygon( const Coord2 &coord )
{
    K::Point_2 *points = new K::Point_2[ _elements.size() ];
    CGAL::Point_2< K > pt( coord.x(), coord.y() );

    for( unsigned int i = 0; i < _elements.size(); i++ ){
        points[i] = K::Point_2( _elements[i].x(), _elements[i].y() );
    }

    CGAL::Bounded_side bside = CGAL::bounded_side_2( points, points+_elements.size(), pt );

    if( bside == CGAL::ON_BOUNDED_SIDE ) {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
//
//  operator << --	output
//
//  Inputs
//	s	: reference to output stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to output stream
//
ostream & operator << ( ostream & stream, const Polygon2 & obj )
{
    int i;		// loop counter
    // set the output formatting
    //stream << setiosflags( ios::showpoint );
    //stream << setprecision( 8 );
    //int width = 16;
    // print out the elements
    for ( i = 0; i < obj._elements.size(); i++ ) {
	    //stream << setw( width ) << obj._element[ i ];
    	stream << setw( 4 ) << obj._elements[ i ];
	    if ( i != 1 ) stream << "\t";
    }
    stream << endl;

    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	s	: reference to input stream
//	v	: 2D coordinates
//
//  Outputs
//	reference to input stream
//
istream & operator >> ( istream & stream, Polygon2 & obj )
{
    int i;		// loop counter
    // reading the elements
    for ( i = 0; i < obj._elements.size(); i++ )
	stream >> obj._elements[ i ];
    return stream;
}




