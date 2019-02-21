//==============================================================================
// Package.h
//  : header file for the Package network
//
//==============================================================================

#ifndef _Package_H        // beginning of header file
#define _Package_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "base/Grid2.h"
#include "base/Contour2.h"
#include "base/Boundary.h"
#include "graph/BoundaryGraph.h"
#include "graph/ForceGraph.h"
#include "optimization/Force.h"
#include "optimization/Stress.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------
typedef pair< BoundaryGraph::vertex_descriptor, BoundaryGraph::edge_descriptor >	VEPair;
typedef pair< unsigned int, unsigned int >	        VVIDPair;
typedef map< Grid2, VEPair >                        VEMap;

//------------------------------------------------------------------------------
//	Defining macros
//------------------------------------------------------------------------------
class Package : public Boundary
{
protected:

    // skeleton
    ForceGraph                  _bone;

    // optimization
    Force                       _forceBone;     // force layout
    Stress                      _stressBone;    // stress layout

    map< unsigned int, Polygon2 >           _polygonComplex;    // for composite graph
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > > _polygonComplexVD;  // for graph bound

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void _init( void );
    void _clear( void );

public:
    
    Package();                        // default constructor
    Package( const Package & obj );   // Copy constructor
    virtual ~Package();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
    const ForceGraph &	        bone( void ) const          { return _bone; }
    ForceGraph &			    bone( void )	            { return _bone; }

    const Force &		        forceBone( void ) const     { return _forceBone; }
    Force &			            forceBone( void )	        { return _forceBone; }
    const Stress &		        stressBone( void ) const    { return _stressBone; }
    Stress &			        stressBone( void )	        { return _stressBone; }

    const map < unsigned int, Polygon2 > &	polygonComplex( void ) const    { return _polygonComplex; }
    map< unsigned int, Polygon2 > &			polygonComplex( void )	        { return _polygonComplex; }

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void createPolygonComplex( unsigned int nv );
    void updatePolygonComplex( void );      // update coordinates after optimization
    void buildBoundaryGraph( void );
    bool findVertexInComplex( Coord2 &coord, ForceGraph &complex,
                              ForceGraph::vertex_descriptor &target );

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
    void init( void ) { _init(); }
    void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Package & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Package & obj );
                                // Input

    virtual const char * className( void ) const { return "Package"; }
                                // Class name
};

#endif // _Package_H

// end of header file
// Do not add any stuff under this line.

