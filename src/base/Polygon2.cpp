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
    _clear();

    _id = 0;
    _gid = 0;
    _area = 0.0;
    _center.zero();        // average of the elements
    _centroid.zero();      // centroid of the elements
    _boundingBox.zero();   // width and height of the bounding box
    _boxCenter.zero();     // center of the bounding box
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
    _boundingBox = v._boundingBox;
    _boxCenter = v._boxCenter;
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
        _boundingBox = p._boundingBox;
        _boxCenter = p._boxCenter;
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
void Polygon2::computeBoundingBox( void )
{
    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
    for( unsigned int i = 0; i < _elements.size(); i++ ){
        if( minX > _elements[i].x() ) minX = _elements[i].x();
        if( maxX < _elements[i].x() ) maxX = _elements[i].x();
        if( minY > _elements[i].y() ) minY = _elements[i].y();
        if( maxY < _elements[i].y() ) maxY = _elements[i].y();
    }

    _boxCenter.x() = ( minX + maxX )/2.0;
    _boxCenter.y() = ( minY + maxY )/2.0;
    _boundingBox.x() = maxX - minX;
    _boundingBox.y() = maxY - minY;
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
    if( !(polygon.size() >= 3) ){
        cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
        cerr << "polygon.size() = " << polygon.size() << endl;
        assert( polygon.size() >= 3 );
    }
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

    _area = CGAL::to_double( polygon.area() );
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


    map< pair< double, double >, unsigned int > elementMap;
    for( unsigned int i = 0; i < _elements.size(); i++ ){
        double x =_elements[i].x();
        double y =_elements[i].y();
        double nx =_elements[(i+1)%_elements.size()].x();
        double ny =_elements[(i+1)%_elements.size()].y();
        if( (_elements[i]-_elements[(i+1)%_elements.size()] ).norm() > 0.1 ){
            p.push_back( K::Point_2( x, y ) );
            elementMap.insert( pair< pair< double, double >, unsigned int >(
                    pair< double, double >( x, y ), i ) );
        }
        //cerr << "(x, y) = " << x << ", " << y << endl;
    }
    //cerr << endl;
    //cerr << "element.size() = " << _elements.size() << " eleMap.size() = " << elementMap.size() << endl;
    // assert( _elements.size() == elementMap.size() );
    if( p.is_simple() == false ){
        cerr << "polygon is not simple..." << endl
             << p << endl;
        assert( false );
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
//  Polygon2::inPolygon --    check if the coordinate is in the polygon
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

//
//  Polygon2::minDistToPolygon --    find the minimum distance of a vertex to the polygon boundary
//
//  Inputs
//  none
//
//  Outputs
//  none
//
double Polygon2::minDistToPolygon( const Coord2 &coord )
{
    double minDist = INFINITY;

    for( unsigned int i = 0; i < _elements.size(); i++ ){

        Coord2 &coordM = _elements[ i ];
        Coord2 &coordN = _elements[ (i+1)%_elements.size() ];
        Coord2 mnVec = coordN - coordM;
        Coord2 cmVec = coord - coordM;
        double D = ( mnVec * cmVec ) / mnVec.squaredNorm();
        Coord2 coordD = coordM + D*mnVec;

        if( Line2::isOnLine( coordD, coordM, coordN )){

            double dist = ( coord - coordD ).norm();
            if( dist < minDist ) minDist = dist;
        }
    }

    return minDist;
}

//
//  Polygon2::maxRadiusInPolygon --    find the maximun radius in the polygon
//
//  Inputs
//  none
//
//  Outputs
//  none
//
double Polygon2::maxRadiusInPolygon( const Coord2 &coord )
{
    double maxR = INFINITY;

    for( unsigned int i = 0; i < _elements.size(); i++ ){

        Coord2 &coordM = _elements[ i ];
        double dist = (coordM-coord).norm();
        // cerr << " dist = " << dist;
        if( dist < maxR ) maxR = dist;
    }
    // cerr << " return " << maxR << endl;

    for( unsigned int i = 0; i < _elements.size(); i++ ){

        Coord2 &coordM = _elements[ i ];
        Coord2 &coordN = _elements[ (i+1)%_elements.size() ];
        Coord2 mnVec = coordN - coordM;
        Coord2 cmVec = coord - coordM;
        double D = ( mnVec * cmVec ) / mnVec.squaredNorm();
        Coord2 coordD = coordM + D*mnVec;

        if( Line2::isOnLine( coordD, coordM, coordN )){

            double dist = ( coord - coordD ).norm();
            if( dist < maxR ) maxR = dist;
        }
    }

    return maxR;
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




