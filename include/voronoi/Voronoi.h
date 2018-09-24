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

#include <boost/lexical_cast.hpp>
#include <base/Polygon2.h>

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//Used to convert otherwise infinite rays into looooong line segments
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


//----------------------------------------------------------------------
//	Defining macros
//----------------------------------------------------------------------

class Voronoi
{

private:

    unsigned int _width, _height;
    vector< Coord2 > * _seedVecPtr;
    vector< double > * _seedWeightVecPtr;
    vector< vector< K::Point_2 > > _polyVec2D;
    map< unsigned int, Polygon2 >  *_polygonVecPtr;

protected:

    void    _init ( vector< Coord2 > & __seedVec, vector< double > & __seedWeightVec,
                    map< unsigned int, Polygon2 > &__polygonVec,
                    unsigned int __width, unsigned int __height ){
        _seedVecPtr = &__seedVec;
        _seedWeightVecPtr = &__seedWeightVec;
        _polygonVecPtr = &__polygonVec;
        _width = __width;
        _height = __height;
    }

public:

    Voronoi();                     // default constructor
    Voronoi( const Voronoi & obj ); // Copy constructor
    virtual ~Voronoi();            // Destructor

//------------------------------------------------------------------------------
//  Reference to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void init( vector< Coord2 > & __seedVec, vector< double > & __seedWeightVec,
               map< unsigned int, Polygon2 > &__polygonVec,
               unsigned int __width, unsigned int __height ){
        _init( __seedVec, __seedWeightVec, __polygonVec, __width, __height );
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
    void clear( void );

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
