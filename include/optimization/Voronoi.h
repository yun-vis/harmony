//==============================================================================
// Voronoi.h
//  : header file for the optimization
//
//==============================================================================

#ifndef _Voronoi_H        // begining of header file
#define _Voronoi_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_filtered_traits_2.h>
#include <CGAL/Regular_triangulation_adaptation_traits_2.h>
#include <CGAL/Regular_triangulation_adaptation_policies_2.h>
#include <CGAL/Regular_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/bounding_box.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/centroid.h>
#include <CGAL/double.h>

#include <boost/lexical_cast.hpp>
#include "base/Polygon2.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//Used to convert otherwise infinite rays into long line segments
const int RAY_LENGTH = 1000;


typedef CGAL::Exact_predicates_exact_constructions_kernel K;
//typedef CGAL::Exact_predicates_inexact_constructions_kernel KI;

#ifdef __linux__
typedef CGAL::Regular_triangulation_filtered_traits_2<K>  Traits;
typedef CGAL::Regular_triangulation_2<Traits> RT2;
#endif  // __linux__

#ifdef __APPLE__
typedef CGAL::Regular_triangulation_2<K> RT2;
#endif  // __MAC__

typedef CGAL::Regular_triangulation_adaptation_traits_2<RT2>         AT;
typedef CGAL::Regular_triangulation_degeneracy_removal_policy_2<RT2> DRP;
typedef CGAL::Voronoi_diagram_2<RT2, AT, DRP> VD;
typedef CGAL::Polygon_2< K >::Vertex_circulator                      Vertex_circulator;

using CGAL::ORIGIN;

//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------
class Seed
{
public:
    unsigned int    id;             // id
    Coord2          coord;          // position
    double          weight;         // weight
    Polygon2        cellPolygon;    // cell polygon
};

class Voronoi
{

private:

    // contour
    Polygon2                       *_contourPtr;        // contour of the voronoi diagram

    // seeds
    vector< Seed >                 *_seedVecPtr;
    vector< vector< K::Point_2 > >  _polyVec2D;         // map seed points and cell polygon

protected:

    void    _init ( vector< Seed > & __seedVec, Polygon2 &__contour );
    void    _clear();

public:

    Voronoi();                     // default constructor
    Voronoi( const Voronoi & obj ); // Copy constructor
    virtual ~Voronoi();            // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------
    const vector< Seed > *      seedVec( void )         const   { return _seedVecPtr; }
    vector< Seed > *            seedVec( void )                 { return _seedVecPtr; }

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void init( vector< Seed > & __seedVec, Polygon2 &__contour ){
        _init( __seedVec, __contour );
    }
    void createVoronoiDiagram( void );
    void createWeightedVoronoiDiagram( void );
    void mapSeedsandPolygons( void );

//------------------------------------------------------------------------------
//  Initialization functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Voronoi & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Voronoi & obj );
                                // Input

    virtual const char * className( void ) const { return "Voronoi"; }
                                // Class name
};

#endif // _Voronoi_H

// end of header file
// Do not add any stuff under this line.
