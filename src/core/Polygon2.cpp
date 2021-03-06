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

#include "core/Polygon2.h"

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
void Polygon2::_init( unsigned int __id, vector< Coord2 > __elements ) {
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
void Polygon2::_clear( void ) {
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
Polygon2::Polygon2() {
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
Polygon2::Polygon2( vector< Coord2 > __elements ) {
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
Polygon2::Polygon2( const Polygon2 &v ) {
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
Polygon2 &Polygon2::operator=( const Polygon2 &p ) {
	if( this != &p ) {
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
void Polygon2::computeBoundingBox( void ) {
	double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		if( minX > _elements[ i ].x() ) minX = _elements[ i ].x();
		if( maxX < _elements[ i ].x() ) maxX = _elements[ i ].x();
		if( minY > _elements[ i ].y() ) minY = _elements[ i ].y();
		if( maxY < _elements[ i ].y() ) maxY = _elements[ i ].y();
	}
	
	_boxCenter.x() = ( minX + maxX ) / 2.0;
	_boxCenter.y() = ( minY + maxY ) / 2.0;
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
void Polygon2::updateCentroid( void ) {
	CGAL::Polygon_2< K > polygon;
	_centroid.x() = 0.0;
	_centroid.y() = 0.0;
	
	// initialization
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		polygon.push_back( K::Point_2( _elements[ i ].x(), _elements[ i ].y() ) );
	}
	
	// check if the polygon has at least three vertices
	if( !( polygon.size() >= 3 ) ) {
		cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
		cerr << "polygon.size() = " << polygon.size() << endl;
		
		if( polygon.size() == 0 ) return;
		
		Vertex_circulator curr = polygon.vertices_circulator();
		Vertex_circulator next = curr;
		++next;
		CGAL::Vector_2< K > center = ( *curr - ORIGIN ) + ( *next - ORIGIN );
		double x = 0.5 * CGAL::to_double( center.x() );
		double y = 0.5 * CGAL::to_double( center.y() );
		_centroid.x() = x;
		_centroid.y() = y;
		
		return;
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
		a = CGAL::to_double( ( ( *cur ).x() ) * ( ( *next ).y() ) - ( ( *next ).x() ) * ( ( *cur ).y() ) );
		center = center + a * ( ( *cur - ORIGIN ) + ( *next - ORIGIN ) );
		atot = atot + a;
		cur = next;
		++next;
	} while( cur != start );
	atot = 3 * atot;
	center = center / atot;
	
	double x = CGAL::to_double( ( ORIGIN + center ).x() );
	double y = CGAL::to_double( ( ORIGIN + center ).y() );
	_centroid.x() = x;
	_centroid.y() = y;
	
	_area = CGAL::to_double( polygon.area() );
	
	// randomCentroid();
}

void Polygon2::randomCentroid( void ) {
	
	int num = 20;
	Coord2 c = _centroid;
	double min = minDistToPolygon( c );
	for( int i = 0; i < num; i++ ) {
		
		Coord2 coord( 0.0, 0.0 );
		coord.x() = c.x() + 20.0 * ( 2.0 * rand() / ( double ) RAND_MAX - 1.0 );
		coord.y() = c.y() + 20.0 * ( 2.0 * rand() / ( double ) RAND_MAX - 1.0 );
		if( min < minDistToPolygon( coord ) && inPolygon( coord ) ){
			c = coord;
			// cerr << "HERE min = " << min << " dist = " << minDistToPolygon( coord ) << endl;
		}
	}
	_centroid = c;
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
void Polygon2::updateOrientation( void ) {
	
	CGAL::Polygon_2< K > p;
	
	if( _elements.size() == 0 ) return;
	
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		double x = _elements[ i ].x();
		double y = _elements[ i ].y();
		double nx = _elements[ ( i + 1 ) % ( int ) _elements.size() ].x();
		double ny = _elements[ ( i + 1 ) % ( int ) _elements.size() ].y();
		
		p.push_back( K::Point_2( x, y ) );
	}

#ifdef DEBUG
	cerr << "polygon:" << endl << p << endl;
#endif // DEBUG
	if( p.is_simple() == false ) {
		cerr << "polygon is not simple..." << endl
		     << p << endl;
		assert( false );
	}
	
	if( p.orientation() == -1 ) {
		p.reverse_orientation();
		
		_elements.clear();
		for( unsigned int i = 0; i < p.size(); i++ ) {
			_elements.push_back( Coord2( CGAL::to_double( p[ i ].x() ), CGAL::to_double( p[ i ].y() ) ) );
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
bool Polygon2::inPolygon( const Coord2 &coord ) {
	
	K::Point_2 *points = new K::Point_2[_elements.size()];
	CGAL::Point_2< K > pt( coord.x(), coord.y() );
	
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		points[ i ] = K::Point_2( _elements[ i ].x(), _elements[ i ].y() );
	}
	
	CGAL::Bounded_side bside = CGAL::bounded_side_2( points, points + _elements.size(), pt );
	
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
double Polygon2::minDistToPolygon( const Coord2 &coord ) {
	double minDist = INFINITY;
	
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		
		Coord2 &coordM = _elements[ i ];
		Coord2 &coordN = _elements[ ( i + 1 ) % ( int ) _elements.size() ];
		Coord2 mnVec = coordN - coordM;
		Coord2 cmVec = coord - coordM;
		double D = ( mnVec * cmVec ) / mnVec.squaredNorm();
		Coord2 coordD = coordM + D * mnVec;
		
		if( Line2::isOnLine( coordD, coordM, coordN ) ) {
			
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
double Polygon2::maxRadiusInPolygon( const Coord2 &coord ) {
	double maxR = INFINITY;
	
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		
		Coord2 &coordM = _elements[ i ];
		double dist = ( coordM - coord ).norm();
		// cerr << " dist = " << dist;
		if( dist < maxR ) maxR = dist;
	}
	// cerr << " return " << maxR << endl;
	
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		
		Coord2 &coordM = _elements[ i ];
		Coord2 &coordN = _elements[ ( i + 1 ) % ( int ) _elements.size() ];
		Coord2 mnVec = coordN - coordM;
		Coord2 cmVec = coord - coordM;
		double D = ( mnVec * cmVec ) / mnVec.squaredNorm();
		Coord2 coordD = coordM + D * mnVec;
		
		if( Line2::isOnLine( coordD, coordM, coordN ) ) {
			
			double dist = ( coord - coordD ).norm();
			if( dist < maxR ) maxR = dist;
		}
	}
	
	return maxR;
}

//
//  Polygon2::isSimple --    is simple polygon
//
//  Inputs
//  none
//
//  Outputs
//  none
//
bool Polygon2::isSimple( void ) {
	
	CGAL::Polygon_2< K > poly;
	
	for( unsigned int i = 0; i < _elements.size(); i++ ) {
		
		poly.push_back( K::Point_2( _elements[ i ].x(), _elements[ i ].y() ) );
	}
	
	if( poly.is_simple() == false ) {
		cerr << "isSimple = " << poly.is_simple() << endl;
		cerr << "(overlap)::_polygon = " << *this << endl;
	}
	
	return poly.is_simple();
}

//
//  Polygon2::cleanPolygon --    clean up vertices if the polygon is not simple
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Polygon2::cleanPolygon( void ) {
	
	if( isSimple() == true ) return;
	
	bool isSimple = false;
	Polygon2 ori = _elements;
	bool isUpdated = false;
	int test = 0;

#ifdef DEBUG
	cerr << "(before)::_polygon = " << endl << *this << endl;
#endif // DEBUG
	
	while( isSimple == false && ( _elements.size() > 0 ) && test < 5 ) {
		
		test++;
		CGAL::Polygon_2< K > poly;
		
		// remove picks
		Polygon2 tmp;
		tmp.elements() = _elements;
		_elements.clear();
		// cerr << "size = " << tmp.elements().size() << endl;
		for( unsigned int i = 0; i < tmp.elements().size(); i++ ) {
			int idP = ( i - 1 + ( int ) tmp.elements().size() ) % ( int ) tmp.elements().size();
			int idN = ( i + 1 ) % ( int ) tmp.elements().size();
			Coord2 &coordP = tmp.elements()[ idP ];
			Coord2 &coordI = tmp.elements()[ i ];
			Coord2 &coordN = tmp.elements()[ idN ];
			Coord2 PI = coordP - coordI;
			Coord2 NI = coordN - coordI;
			Coord2 PN = coordP - coordN;
			double acosValue = acos(
					( PI.squaredNorm() + NI.squaredNorm() - PN.squaredNorm() ) / ( 2.0 * PI.norm() * NI.norm() ) );
			if( acosValue > 5.0 / 180.0 * M_PI ) {
				//if( ( acos( (PI.squaredNorm() + NI.squaredNorm() - PN.squaredNorm())/(2.0*PI.norm()*NI.norm()) ) > 1e-5 ) &&
				//    PN.norm()/NI.norm() > 1e-5  ){
				_elements.push_back( tmp.elements()[ i ] );
				poly.push_back( K::Point_2( tmp.elements()[ i ].x(), tmp.elements()[ i ].y() ) );
			}
			else {
#ifdef DEBUG
				cerr << "acosValue = " << acosValue << endl;
				cerr << "idP = " << idP << " i = " << i << " idN = " << idN << endl;
				cerr << "P = " << coordP;
				cerr << "I = " << coordI;
				cerr << "N = " << coordN;
				cerr << "PI = " << PI;
				cerr << "NI = " << NI;
				cerr << "PN =" << PN;
#endif // DEBUG
				isUpdated = true;
				
				// put the rest of the vector back and break
				for( unsigned int j = i + 1; j < tmp.elements().size(); j++ ) {
					_elements.push_back( tmp.elements()[ j ] );
					poly.push_back( K::Point_2( tmp.elements()[ j ].x(), tmp.elements()[ j ].y() ) );
				}
				break;
			}
		}
		
		// remove overlapped vertices
		poly.clear();
		for( unsigned int j = 0; j < _elements.size(); j++ ) {
			if( ( _elements[ j ] - _elements[ ( j + 1 ) % ( int ) _elements.size() ] ).norm() > MIN_VERTEX_DISTANCE )
				poly.push_back( K::Point_2( _elements[ j ].x(), _elements[ j ].y() ) );
			else {
#ifdef DEBUG
				cerr << "curr = " << _elements[ j ]
					 << "next = " << _elements[ ( j + 1 ) % ( int ) _elements.size() ] << endl;
				cerr << "idC = " << j << " idN = " << ( j + 1 ) % ( int ) _elements.size() << endl;
#endif // DEBUG
				isUpdated = true;
			}
		}
		
		_elements.clear();
		for( unsigned int j = 0; j < poly.size(); j++ ) {
			_elements.push_back( Coord2( CGAL::to_double( poly[ j ].x() ),
			                             CGAL::to_double( poly[ j ].y() ) ) );
		}
		
		isSimple = poly.is_simple();
		if( poly.is_simple() == false ) {
			cerr << "isSimple = " << poly.is_simple() << endl;
			cerr << "(overlap)::_polygon = " << *this << endl;
			cerr << "(original)::_polygon = " << ori << endl;
		}
		else {
			break;
		}
	}
	
	if( isUpdated == true )
		cerr << "(clean)::_polygon = " << endl << *this << endl;
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
ostream &operator<<( ostream &stream, const Polygon2 &obj ) {
	int i;        // loop counter
	// set the output formatting
	//stream << setiosflags( ios::showpoint );
	//stream << setprecision( 8 );
	//int width = 16;
	// print out the elements
	for( i = 0; i < obj._elements.size(); i++ ) {
		//stream << setw( width ) << obj._element[ i ];
		stream << setw( 4 ) << obj._elements[ i ];
		if( i != 1 ) stream << "\t";
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
istream &operator>>( istream &stream, Polygon2 &obj ) {
	int i;        // loop counter
	// reading the elements
	for( i = 0; i < obj._elements.size(); i++ )
		stream >> obj._elements[ i ];
	return stream;
}




