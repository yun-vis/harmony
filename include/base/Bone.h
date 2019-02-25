//==============================================================================
// Bone.h
//  : header file for the Bone network
//
//==============================================================================

#ifndef _Bone_H        // beginning of header file
#define _Bone_H        // notifying that this file is included

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
class Bone
{
protected:

    // skeleton
    ForceGraph                  _bone;
    bool                        _isForce;       // force or stress

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
    
    Bone();                         // default constructor
    Bone( const Bone & obj );       // Copy constructor
    virtual ~Bone();                // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
    const ForceGraph &	        bone( void ) const          { return _bone; }
    ForceGraph &			    bone( void )	            { return _bone; }
    const bool &	            isForce( void ) const       { return _isForce; }
    bool &			            isForce( void )	            { return _isForce; }

    const Force &		        forceBone( void ) const     { return _forceBone; }
    Force &			            forceBone( void )	        { return _forceBone; }
    const Stress &		        stressBone( void ) const    { return _stressBone; }
    Stress &			        stressBone( void )	        { return _stressBone; }

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
    friend ostream & operator << ( ostream & stream, const Bone & obj );
                                // Output
    friend istream & operator >> ( istream & stream, Bone & obj );
                                // Input

    virtual const char * className( void ) const { return "Bone"; }
                                // Class name
};

#endif // _Bone_H

// end of header file
// Do not add any stuff under this line.

