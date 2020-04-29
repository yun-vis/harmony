//==============================================================================
// RegionBase.h
//  : header file for the RegionBase network
//
//==============================================================================

#ifndef _RegionBase_H        // beginning of header file
#define _RegionBase_H        // notifying that this file is included

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
#include "graph/ForceGraph.h"
#include "graph/BoundaryGraph.h"
#include "optimization/Force.h"
#include "optimization/Stress.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining macros
//------------------------------------------------------------------------------
class RegionBase
{
protected:

    // composite graph
    ForceGraph                  _forceGraph;
    // boundary of the composite graph
    Boundary                    _boundary;

    // force or stress
    ENERGYTYPE                  _energyType;

    // optimization
    Force                       _force;             // force layout
    Stress                      _stress;            // stress layout

    map< unsigned int, Polygon2 >                       _polygonComplex;    // for composite polygon
    map< unsigned int,
         vector< BoundaryGraph::vertex_descriptor > >   _polygonComplexVD;  // map to the boundary graph

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void _init( void );
    void _clear( void );

public:

    RegionBase();                               // default constructor
    RegionBase( const RegionBase & obj );       // Copy constructor
    virtual ~RegionBase();                      // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
    const ForceGraph &	        forceGraph( void ) const    { return _forceGraph; }
    ForceGraph &			    forceGraph( void )	        { return _forceGraph; }
    const ENERGYTYPE &	        energyType( void ) const    { return _energyType; }
    ENERGYTYPE &			    energyType( void )	        { return _energyType; }

    const Boundary &		    boundary( void ) const      { return _boundary; }
    Boundary &			        boundary( void )	        { return _boundary; }

    const Force &		        force( void ) const         { return _force; }
    Force &			            force( void )	            { return _force; }
    const Stress &		        stress( void ) const        { return _stress; }
    Stress &			        stress( void )	            { return _stress; }

    const map < unsigned int, Polygon2 > &	polygonComplex( void ) const    { return _polygonComplex; }
    map< unsigned int, Polygon2 > &			polygonComplex( void )	        { return _polygonComplex; }

    const map< unsigned int, vector< BoundaryGraph::vertex_descriptor > > & polygonComplexVD( void ) const  { return _polygonComplexVD; }
    map< unsigned int, vector< BoundaryGraph::vertex_descriptor > > &       polygonComplexVD( void )        { return _polygonComplexVD; }

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
    void createPolygonComplex( unsigned int nv );
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
    friend ostream & operator << ( ostream & stream, const RegionBase & obj );
                                // Output
    friend istream & operator >> ( istream & stream, RegionBase & obj );
                                // Input

    virtual const char * className( void ) const { return "RegionBase"; }
                                // Class name
};

#endif // _RegionBase_H

// end of header file
// Do not add any stuff under this line.

